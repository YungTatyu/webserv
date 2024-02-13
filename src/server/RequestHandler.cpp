#include "RequestHandler.hpp"
#include "HttpMessage.hpp"
#include <sys/types.h>
#include <algorithm>

RequestHandler::RequestHandler()
{
    this->handler_map[ActiveEventManager::isReadEvent] = &RequestHandler::handleReadEvent;
    this->handler_map[ActiveEventManager::isWriteEvent] = &RequestHandler::handleWriteEvent;
    this->handler_map[ActiveEventManager::isErrorEvent] = &RequestHandler::handleErrorEvent;
}

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager, const int target )
{
    const std::vector<char>& context = connManager.getRawRequest( target );
	// std::cout << "----- request -----" << std::endl;
	std::cout << context.data() << std::endl; // std::cout << "------- end -------" << std::endl;

    std::string requestData = context.data();
    HttpRequest request = HttpMessage::requestParser( requestData );
    std::string response = HttpMessage::responseGenerater( request );

    std::vector<char> vec(response.begin(), response.end());
    connManager.setResponse( target, vec );
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
			connManager.removeConnection(sockfd);
			return;
		}
		connManager.setEvent(sockfd, ConnectionData::WRITE); // writeイベントに更新
}

void RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	// response作成
	handle( connManager, sockfd );
    if (ioHandler.sendResponse( connManager, sockfd ) != -1)
		connManager.setEvent(sockfd, ConnectionData::READ); // readイベントに更新
}

void RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd)
{
	ioHandler.closeConnection( connManager, sockfd );
	connManager.removeConnection(sockfd);
}
