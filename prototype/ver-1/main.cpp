#include "EchoServer.hpp"

int main()
{
	EchoServer server = EchoServer();

	server.setUp();
	for ( ; ; )
	{
		server.acceptRequest();
		server.sendResponse();
		server.closeConnection();
	}
	server.down();

	return 0;
}
