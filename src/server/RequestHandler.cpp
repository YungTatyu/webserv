#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
#include <sys/types.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

RequestHandler::RequestHandler() {}

int RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd)
{
	if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_READ)
		return handleCgiReadEvent(ioHandler, connManager, sockfd);
	// リスニングソケットへの新規リクエスト
	if (ioHandler.isListenSocket(sockfd))
		return ioHandler.acceptConnection(connManager, sockfd);
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

	// TODO: cgi read event / cgi write eventに更新する際は、返り値で返す必要がある
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
	const int sockfd
)
{
	ioHandler.receiveCgiResponse(connManager, sockfd);
	const cgi::CGIHandler	cgi_handler = connManager.getCgiHandler(sockfd);
	if (cgiProcessExited(cgi_handler.getCgiProcessId()))
	{
		connManager.setEvent(sockfd, ConnectionData::EV_WRITE);
		ioHandler.closeConnection(connManager, sockfd);
		return RequestHandler::UPDATE_WRITE;
	}
	return RequestHandler::UPDATE_NONE;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	if (connManager.getEvent(sockfd) == ConnectionData::EV_CGI_WRITE)
		return handleCgiWriteEvent(ioHandler, connManager, sockfd);
	if (ioHandler.sendResponse( connManager, sockfd ) == -1)
		return RequestHandler::UPDATE_NONE;
	connManager.setEvent(sockfd, ConnectionData::EV_READ); // readイベントに更新
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

int RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	ioHandler.closeConnection( connManager, sockfd );
	return RequestHandler::UPDATE_CLOSE;
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
