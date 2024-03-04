#include "RequestHandler.hpp"
#include "HttpMessage.hpp"
#include <sys/types.h>
#include <algorithm>

RequestHandler::RequestHandler()
{
	// this->handler_map[IActiveEventManager::isReadEvent] = &RequestHandler::handleReadEvent;
	// this->handler_map[ActiveEventManager::isWriteEvent] = &RequestHandler::handleWriteEvent;
	// this->handler_map[ActiveEventManager::isErrorEvent] = &RequestHandler::handleErrorEvent;
}

void RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
		// リスニングソケットへの新規リクエスト
		if (sockfd == ioHandler.getListenfd())
		{
			ioHandler.acceptConnection(connManager);
			return;
		}
		// クライアントソケットへのリクエスト（既存コネクション）
		ssize_t re = ioHandler.receiveRequest( connManager, sockfd );
		if (re == -1) //ソケット使用不可。
			return;
		if (re == 0) // クライアントが接続を閉じる
		{
			ioHandler.closeConnection( connManager, sockfd );
			connManager.removeConnection( sockfd );
			return;
		}
		const std::vector<char>& context = connManager.getRawRequest( sockfd );
		std::string requestData = context.data();

		HttpRequest request = HttpMessage::requestParser( requestData );
		// HttpRequest request = HttpRequest::parseRequest( requestData, connManager.getRequest(sockfd) );

		connManager.setRequest( sockfd, request );

		connManager.setEvent( sockfd, ConnectionData::WRITE ); // writeイベントに更新
}

void RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	// response作成
	HttpRequest request = connManager.getRequest( sockfd );
	std::string response = HttpMessage::responseGenerater( request );

	std::vector<char> vec( response.begin(), response.end()) ;
	connManager.setResponse( sockfd, vec );

	if (ioHandler.sendResponse( connManager, sockfd ) != -1)
		connManager.setEvent(sockfd, ConnectionData::READ); // readイベントに更新
}

void RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	ioHandler.closeConnection( connManager, sockfd );
	connManager.removeConnection( sockfd );
}
