#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
#include "HttpMessage.hpp"
#include <sys/types.h>
#include <algorithm>

RequestHandler::RequestHandler()
{
	// this->handler_map[IActiveEventManager::isReadEvent] = &RequestHandler::handleReadEvent;
	// this->handler_map[ActiveEventManager::isWriteEvent] = &RequestHandler::handleWriteEvent;
	// this->handler_map[ActiveEventManager::isErrorEvent] = &RequestHandler::handleErrorEvent;
}

int RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd)
{
		// リスニングソケットへの新規リクエスト
		if (sockfd == ioHandler.getListenfd())
		{
			return ioHandler.acceptConnection(connManager);
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
		const std::vector<char>& context = connManager.getRawRequest( sockfd );
		std::string requestData = context.data();
		HttpRequest request = HttpMessage::requestParser( requestData );
		connManager.setRequest( sockfd, request );

		std::string	final_response = HttpResponse::generateResponse( request, connManager.getResponse(sockfd), connManager.getTiedServer(sockfd), sockfd, configHandler );
		if (!final_response.empty())
			connManager.setFinalResponse( sockfd, std::vector<char> (final_response.begin(), final_response.end()));

		connManager.setEvent( sockfd, ConnectionData::WRITE ); // writeイベントに更新
		return RequestHandler::UPDATE_WRITE;
}

int RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	if (ioHandler.sendResponse( connManager, sockfd ) == -1)
		return RequestHandler::NONE;
	connManager.setEvent(sockfd, ConnectionData::READ); // readイベントに更新
	return RequestHandler::UPDATE_READ;
}

int RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	ioHandler.closeConnection( connManager, sockfd );
	connManager.removeConnection( sockfd );
	return RequestHandler::UPDATE_CLOSE;
}
