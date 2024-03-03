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
		/**
		 * >0 の値はfdと被るので避ける
		 * -10> の値はシステムコールのエラーと被るので避ける
		*/
		enum UPDATE_STATUS
		{
			NONE = -10,
			UPDATE_READ = -11,
			UPDATE_WRITE = -12,
			UPDATE_CLOSE = -13
		};
		RequestHandler();
		int handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);
		int handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);
		int handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);

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
