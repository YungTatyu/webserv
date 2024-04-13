#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
#include <sys/types.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

RequestHandler::RequestHandler() {}

int RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler& configHandler, TimerTree &timerTree, const int sockfd)
{
	if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_READ)
		return handleCgiReadEvent(ioHandler, connManager, configHandler, sockfd);
	// リスニングソケットへの新規リクエスト
	if (ioHandler.isListenSocket(sockfd))
	{
		int	accept_sock = ioHandler.acceptConnection(connManager, sockfd);
		if (accept_sock == -1)
			return accept_sock;
		// timeout追加
		// client_header_timeout directiveの値をセットする
		// 本サーバーではデフォルト値として30秒にセットする
		config::Time	timeout;
		// ToDo: 本来client_header_timeoutだが、現状では定数
		timeout = config::Time(60 * config::Time::seconds);
		timerTree.addTimer(
			Timer(accept_sock, timeout)
		);
		return accept_sock;
	}

	// keepalive_timeout消す。
	timerTree.deleteTimer(sockfd);

	// クライアントソケットへのリクエスト（既存コネクション）
	ssize_t re = ioHandler.receiveRequest( connManager, sockfd );
	if (re == -1) //ソケット使用不可。
		return RequestHandler::UPDATE_NONE;
	if (re == 0) // クライアントが接続を閉じる
	{
		ioHandler.closeConnection( connManager, sockfd );
		return RequestHandler::UPDATE_CLOSE;
	}
	if (re == 2) // buffer分以降を読む
	       return RequestHandler::UPDATE_NONE;
	const std::vector<unsigned char>& context = connManager.getRawRequest( sockfd );
	std::string requestData = std::string(reinterpret_cast<const char*>(context.data()));

	HttpRequest::parseRequest( requestData, connManager.getRequest(sockfd) );

	HttpRequest::ParseState state = connManager.getRequest(sockfd).parseState;
	if (state != HttpRequest::PARSE_COMPLETE && state != HttpRequest::PARSE_ERROR) // 新しいHttpRequestを使う時にここを有効にしてchunk読み中はreadイベントのままにする
		return RequestHandler::UPDATE_NONE;
	return handleResponse(connManager, configHandler, sockfd);
}

int	RequestHandler::handleResponse(ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd)
{
	HttpRequest	&request = connManager.getRequest(sockfd);
	HttpResponse	&response = connManager.getResponse(sockfd);
	std::string	final_response = HttpResponse::generateResponse( request, response, connManager.getTiedServer(sockfd), sockfd, configHandler );

	if (response.state_ == HttpResponse::RES_EXECUTE_CGI)
		return handleCgi(connManager, configHandler, sockfd);
	connManager.setFinalResponse( sockfd, std::vector<unsigned char> (final_response.begin(), final_response.end()));

	connManager.setEvent( sockfd, ConnectionData::EV_WRITE ); // writeイベントに更新
	return RequestHandler::UPDATE_WRITE;
}

int	RequestHandler::handleCgi(ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd)
{
	HttpRequest	&request = connManager.getRequest(sockfd);
	HttpResponse	&response = connManager.getResponse(sockfd);

	bool	re = connManager.callCgiExecutor(sockfd, response.res_file_path_, request);
	if (!re)
	{
		connManager.resetCgiSockets(sockfd);
		response.state_ = HttpResponse::RES_CGI_ERROR;
		return handleResponse(connManager, configHandler, sockfd);
	}
	// bodyが空なら、bodyをsendしない
	if (request.body.empty())
	{
		connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_READ);
		return RequestHandler::UPDATE_CGI_READ;
	}
	connManager.setCgiConnection(sockfd, ConnectionData::EV_CGI_WRITE);
	return RequestHandler::UPDATE_CGI_WRITE;
}

int RequestHandler::handleCgiReadEvent(
	NetworkIOHandler &ioHandler,
	ConnectionManager &connManager,
	ConfigHandler& configHandler,
	const int sockfd
)
{
	int	bytes = ioHandler.receiveCgiResponse(connManager, sockfd);
	if (bytes == -1 || bytes == 2) // recv errorまたはbuffer size分recv
		return RequestHandler::UPDATE_NONE;
	const cgi::CGIHandler&	cgi_handler = connManager.getCgiHandler(sockfd);
	if (!cgiProcessExited(cgi_handler.getCgiProcessId()))
		return RequestHandler::UPDATE_NONE;

	const std::vector<unsigned char>&	v = connManager.getCgiResponse(sockfd);
	HttpResponse	&response = connManager.getResponse(sockfd);
	std::string res(reinterpret_cast<const char*>(v.data()), v.size());
	bool parse_suc = connManager.callCgiParser(sockfd, response, res);
	response.state_ = parse_suc == true ? HttpResponse::RES_PARSED_CGI : HttpResponse::RES_CGI_ERROR;
	int re = handleResponse(connManager, configHandler, sockfd);
	ioHandler.closeConnection(connManager, sockfd); // delete cgi event
	return re;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd)
{
	if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
		return handleCgiWriteEvent(ioHandler, connManager, sockfd);
	int re = ioHandler.sendResponse( connManager, sockfd );
	if (re == -1) // send error, retry later
		return RequestHandler::UPDATE_NONE;
	if (re == -2) // send not complete, send remainder later
		return RequestHandler::UPDATE_NONE;

	// Connectionヘッダーを見てcloseならコネクションを閉じる
	std::map<std::string, std::string>::iterator	it = connManager.getResponse(sockfd).headers_.find("Connection");
	if (it != connManager.getResponse(sockfd).headers_.end()
		&& it->second == "close")
	{
		ioHandler.closeConnection(connManager, sockfd);
		return UPDATE_CLOSE;
	}

	// keep-alive timeout 追加
	// 400エラーがerror_pageで拾われて内部リダイレクトする可能性があるので以下の処理は必要。
	it = connManager.getRequest(sockfd).headers.find("Host");
	config::Time	timeout;
	std::string host_name;
	if (it == connManager.getRequest(sockfd).headers.end())
		host_name = "_";
	else
		host_name = it->second;
	timeout = configHandler.searchKeepaliveTimeout(
					connManager.getTiedServer(sockfd),
					host_name,
					connManager.getRequest(sockfd).uri
				);
	if (timeout.isNoTime())
	{
		// keepaliveが無効なので接続を閉じる
		ioHandler.closeConnection(connManager, sockfd);
		return UPDATE_CLOSE;
	}

	timerTree.addTimer(
		Timer(sockfd, timeout)
	);

	// readイベントに更新
	connManager.setEvent(sockfd, ConnectionData::EV_READ);
	// connection dataを削除
	connManager.clearConnectionData(sockfd);
	return RequestHandler::UPDATE_READ;
}

int RequestHandler::handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	ssize_t	re = ioHandler.sendRequestBody(connManager, sockfd);
	
	const std::string	body = connManager.getRequest(sockfd).body;
	const cgi::CGIHandler	cgi_handler = connManager.getCgiHandler(sockfd);
	if (connManager.getSentBytes(sockfd) == body.size() || // bodyを全て送ったら
		(re == -1 && cgiProcessExited(cgi_handler.getCgiProcessId()))) // cgi processがすでに死んでいたら
	{
		connManager.resetSentBytes(sockfd);
		connManager.setEvent(sockfd, ConnectionData::EV_CGI_READ);
		return RequestHandler::UPDATE_CGI_READ;
	}
	return RequestHandler::UPDATE_NONE;
}

int RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, TimerTree &timerTree, const int sockfd)
{
	ioHandler.closeConnection( connManager, sockfd );
	timerTree.deleteTimer( sockfd );
	return RequestHandler::UPDATE_CLOSE;
}


void	RequestHandler::handleTimeoutEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timer_tree)
{
	configHandler.writeErrorLog("webserv: [debug] enter timeout handler\n");
	// timeoutしていない最初のイテレータを取得
	Timer	current_time(-1, Timer::getCurrentTime());
	std::multiset<Timer>::iterator upper_bound = timer_tree.getTimerTree().upper_bound(current_time);

	Timer::updateCurrentTime();
	// timeout している接続をすべて削除
	for (std::multiset<Timer>::iterator it = timer_tree.getTimerTree().begin();
		it != upper_bound;
		)
	{
		int client_fd = it->getFd();
		ioHandler.closeConnection(connManager, client_fd);
		// timeoutの種類によってログ出力変える
		std::string	timeout_reason = "waiting for client request.";
		configHandler.writeErrorLog("webserv: [info] client timed out while " + timeout_reason + "\n");
		/*
		switch (it.type_) {
			case TM_KEEPALIVE:
				timeout_reason = "waiting for client request.";
				break;
			case TM_SEND:
				timeout_reason = "sending response to client.";
				break;
		}
		configHandler.writeErrorLog("webserv: [info] client timed out while " + timeout_reason + "\n");
		// これはconnManager.removeConnectonでやるべき？
		configHandler.writeErrorLog("webserv: [debug] close http connection: " + client_fd + "\n");
		*/

		// timer tree から削除
		std::multiset<Timer>::iterator next = it;
		next++;
		timer_tree.deleteTimer(client_fd);
		it = next;
	}
}

/**
 * @brief cgi processが生きているか確認
 * 
 * @param process_id 
 * @return true cgi processが終了している
 * @return false 
 */
bool	RequestHandler::cgiProcessExited(const pid_t process_id) const
{
	int status;
	pid_t re = waitpid(process_id, &status, WNOHANG);
	// errorまたはprocessが終了していない
	// TODO: errorのときの処理はあやしい
	if (re == 0 || re == -1)
		return false;
	return true;
}
