#include "EchoServer.hpp"

int main()
{
	EchoServer server = EchoServer();

	server.setUp();
	for ( ; ; )
	{
		int connfd = server.acceptRequest();
		server.sendResponse( connfd );
		server.closeConnection( connfd );
	}
	server.down();

	return 0;
}
