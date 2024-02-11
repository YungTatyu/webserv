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

void RequestHandler::handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd)
{
    	ssize_t re = ioHandler.receiveRequest( connManager, pollfd.fd );
		// リスニングソケットへの新規リクエスト
		if (pollfd.fd == ioHandler.getListenfd())
		{
			ioHandler.acceptConnection(connManager);
			return;
		}
		// クライアントソケットへのリクエスト（既存コネクション）
		if (re == -1) //ソケット使用不可。
			return;
		if (re == 0) // クライアントが接続を閉じる
		{
			ioHandler.closeConnection( connManager, pollfd.fd );
			connManager.removeConnection(pollfd.fd);
			return;
		}
		handle( connManager, pollfd.fd );
		connManager.setEvent(pollfd.fd, ConnectionData::WRITE); // writeイベントに更新
}

void RequestHandler::handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd)
{
    if (ioHandler.sendResponse( connManager, pollfd.fd ) != -1)
		connManager.setEvent(pollfd.fd, ConnectionData::READ); // readイベントに更新
}

void RequestHandler::handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd)
{
	ioHandler.closeConnection( connManager, pollfd.fd );
	connManager.removeConnection(pollfd.fd);
}
