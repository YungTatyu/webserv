#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
#include <sys/types.h>
#include <algorithm>

RequestHandler::RequestHandler() {}

int RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd)
{
		// リスニングソケットへの新規リクエスト
		if (ioHandler.isListenSocket(sockfd))
		{
			return ioHandler.acceptConnection(connManager, sockfd);
		}
		// クライアントソケットへのリクエスト（既存コネクション）
		ssize_t re = ioHandler.receiveRequest( connManager, sockfd );
		if (re == -1) //ソケット使用不可。
			return RequestHandler::NONE;
		if (re == 0) // クライアントが接続を閉じる
		{
			ioHandler.closeConnection( connManager, sockfd );
			connManager.removeConnection( sockfd );
			return RequestHandler::UPDATE_CLOSE;
		}
		const std::vector<unsigned char>& context = connManager.getRawRequest( sockfd );
		std::string requestData = std::string(reinterpret_cast<const char*>(context.data()));

		HttpRequest::parseRequest( requestData, connManager.getRequest(sockfd) );

		if (connManager.getRequest(sockfd).parseState == HttpRequest::PARSE_ERROR)
		{
		}
		else if ( connManager.getRequest(sockfd).parseState != HttpRequest::PARSE_COMPLETE) // 新しいHttpRequestを使う時にここを有効にしてchunk読み中はreadイベントのままにする
			return RequestHandler::NONE;

		std::string	final_response = HttpResponse::generateResponse( connManager.getRequest(sockfd), connManager.getResponse(sockfd), connManager.getTiedServer(sockfd), sockfd, configHandler );
		if (!final_response.empty())
			connManager.setFinalResponse( sockfd, std::vector<unsigned char> (final_response.begin(), final_response.end()));

		connManager.setEvent( sockfd, ConnectionData::EV_WRITE ); // writeイベントに更新
		return RequestHandler::UPDATE_WRITE;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	if (ioHandler.sendResponse( connManager, sockfd ) == -1)
		return RequestHandler::NONE;
	connManager.setEvent(sockfd, ConnectionData::EV_READ); // readイベントに更新
	return RequestHandler::UPDATE_READ;
}

int RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	ioHandler.closeConnection( connManager, sockfd );
	connManager.removeConnection( sockfd );
	return RequestHandler::UPDATE_CLOSE;
}
