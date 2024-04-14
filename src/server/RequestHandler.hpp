#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>
# include <map>
# include <string>
# include <iostream>
# include "ConnectionManager.hpp"
# include "NetworkIOHandler.hpp"
# include "TimerTree.hpp"

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler
{
	public:
		/**
		 * >0 の値はfdと被るので避ける
		 * -1 はシステムコールのエラーと被るので避ける
		*/
		enum UPDATE_STATUS
		{
			UPDATE_NONE = -10,
			UPDATE_READ = -11,
			UPDATE_WRITE = -12,
			UPDATE_CGI_READ = -13,
			UPDATE_CGI_WRITE = -14,
			UPDATE_CLOSE = -15
		};
		RequestHandler();
		int handleReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd);
		int handleWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd);
		int handleCgiReadEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd);
		int handleCgiWriteEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, const int sockfd);
		int handleErrorEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, TimerTree &timerTree, const int sockfd);
		void handleTimeoutEvent(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timerTree);

	private:
		bool cgiProcessExited(const pid_t process_id) const;
		int	handleResponse(ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd);
		int	handleCgi(ConnectionManager &connManager, ConfigHandler& configHandler, const int sockfd);
		bool	addTimerByType(NetworkIOHandler &ioHandler, ConnectionManager &connManager, ConfigHandler &configHandler, TimerTree &timerTree, const int sockfd, enum Timer::TimeoutType type);
		void	deleteTimerAndConnection(NetworkIOHandler &ioHandler, ConnectionManager &connManager, TimerTree &timerTree, int socket);
		bool	isOverWorkerConnections(ConnectionManager &connManager, ConfigHandler &configHandler);
};

#endif
