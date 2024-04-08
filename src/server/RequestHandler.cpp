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
		config::Time	timeout;
		// この時点ではどのサーバーに属すかも決まっていないので、http コンテキストの値を適用する
		// ただし0に指定されていた場合無限に接続することになるので、
		// keepalive_timeoutではなく、何かデフォルトの時間を適用してもいいかもしれない。
		//timeout = config::Time(5000);
		timeout = configHandler.config_->http.keepalive_timeout.getTime();
		if (timeout.isNoTime())
		{
			ioHandler.closeConnection(connManager, accept_sock);
			return UPDATE_NONE;
		}
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
	// TODO: cgi read event, cgi write eventに更新する際は、返り値で返す必要がある
	// HttpResponseで呼ぶ？
	// bodyが存在する場合は、cgiにbodyを送る必要がある
	std::string	final_response = HttpResponse::generateResponse( connManager.getRequest(sockfd), connManager.getResponse(sockfd), connManager.getTiedServer(sockfd), sockfd, configHandler );
	if (!final_response.empty())
		connManager.setFinalResponse( sockfd, std::vector<unsigned char> (final_response.begin(), final_response.end()));

	connManager.setEvent( sockfd, ConnectionData::EV_WRITE ); // writeイベントに更新
	return RequestHandler::UPDATE_WRITE;
}

int RequestHandler::handleCgiReadEvent(
	NetworkIOHandler &ioHandler,
	ConnectionManager &connManager,
	ConfigHandler& configHandler,
	const int sockfd
)
{
	ioHandler.receiveCgiResponse(connManager, sockfd);
	const cgi::CGIHandler&	cgi_handler = connManager.getCgiHandler(sockfd);
	if (cgiProcessExited(cgi_handler.getCgiProcessId()))
	{
		const std::vector<unsigned char>&	v = connManager.getCgiResponse(sockfd);
		connManager.callCgiParser(sockfd, connManager.getResponse(sockfd),
			std::string(reinterpret_cast<const char*>(v.data())));
		int re = handleResponse(connManager, configHandler, sockfd);
		ioHandler.closeConnection(connManager, sockfd);
		return re;
	}
	return RequestHandler::UPDATE_NONE;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd)
{
	if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
		return handleCgiWriteEvent(ioHandler, connManager, sockfd);
	if (ioHandler.sendResponse( connManager, sockfd ) == -1)
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
	else
	{
		timerTree.addTimer(
			Timer(sockfd, timeout)
		);
	}

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
	if (re == 0 || re == -1)
		return false;
	return true;
}
