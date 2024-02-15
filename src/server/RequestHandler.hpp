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
		void handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);
		void handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);
		void handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);

		typedef bool (*whichEvent)(const struct pollfd& pfd);
		typedef void (RequestHandler::*eventHandler)(
			NetworkIOHandler &ioHandler,
			ConnectionManager &connManager,
			const int sockfd
		);
		// key: eventを判別する関数 value: そのイベントのhandler
		std::map<whichEvent, eventHandler> handler_map;
};

#endif
