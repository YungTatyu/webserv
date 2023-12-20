#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

class ConnManager;

class EchoServer
{
	public:
		EchoServer();
		~EchoServer();
		void setUp();
		void acceptRequest();
		void sendResponse();
		void closeConnection();
		void down();
	
	private:
		int listenfd;
		struct sockaddr_in servaddr;
		// RequestHandler *requestHandler;
		ConnManager *connManager;
};

/*
class RequestHandler
{
	public:
		void readMessage();
};
*/

class ConnManager
{
	public:
		void addConnection( int connfd );
		int readConnfd();
		void removeConnection();
	
	private:
		int connfd;
};
