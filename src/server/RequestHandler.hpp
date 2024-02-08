#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>
# include <string>
# include <iostream>
# include "ConnectionManager.hpp"
# include "NetworkIOHandler.hpp"

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler
{
	public:
		void handle( ConnectionManager &connManager, const int target );
		void handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd);
		void handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd);
};

#endif
