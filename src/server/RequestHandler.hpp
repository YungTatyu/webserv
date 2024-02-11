#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>
# include <map>
# include <string>
# include <iostream>
# include "ConnectionManager.hpp"
# include "NetworkIOHandler.hpp"

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler
{
	public:
		RequestHandler();
		void handle( ConnectionManager &connManager, const int target );
		void handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd);
		void handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd);
		void handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const struct pollfd pollfd);

		typedef bool (*whichEvent)(const struct pollfd& pfd);
		typedef void (RequestHandler::*eventHandler)(
			NetworkIOHandler &ioHandler,
			ConnectionManager &connManager,
			const struct pollfd pollfd
		);
		// key: eventを判別する関数 value: そのイベントのhandler
		std::map<whichEvent, eventHandler> handler_map;
};

#endif
