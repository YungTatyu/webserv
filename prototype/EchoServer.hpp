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

class EchoServer
{
	public:
		EchoServer();
		void setUp();
		int acceptRequest();
		void sendResponse( int connfd );
		void closeConnection(int connfd );
		void down();
	
	private:
		int listenfd;
		struct sockaddr_in servaddr;
		// RequestHandler *requestHandler;
};

/*
class RequestHandler
{
	public:
		void readMessage();
};
*/
