#include "EchoServer.hpp"

EchoServer::EchoServer()
{
//	this->RequestHandler = new RequestHandler;
}

void EchoServer::setUp()
{
	//creation of the socket
	listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
	//preparation of the socket address
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen (listenfd, LISTENQ);
	
	printf("%s\n","Server running...waiting for connections.");
	
}

int EchoServer::acceptRequest()
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);
	printf("%s\n","Received request...");
	return connfd;
}

void EchoServer::sendResponse( int connfd )
{
	int n;
	char buf[MAXLINE];

//	this->requestHandler();
	while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)
	{
		printf("%s","String received from and resent to the client:");
		puts(buf);
		send(connfd, buf, n, 0);
	}

	if (n < 0)
	{
		perror("Read error");
		exit(1);
	}
}

void EchoServer::closeConnection( int connfd )
{
	close(connfd);
}

void EchoServer::down()
{
	close(listenfd);
}

/*
std::string RequestHandler::readMessage()
{
	return 
}
*/
