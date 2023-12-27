#include "EchoServer.hpp"

EchoServer::EchoServer()
{
//	this->RequestHandler = new RequestHandler;
	this->connManager = new ConnManager();
}

EchoServer::~EchoServer()
{
	delete this->connManager;
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

void EchoServer::acceptRequest()
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);
	this->connManager->addConnection( connfd );
	printf("%s\n","Received request...");
}

void EchoServer::sendResponse()
{
	int n;
	char buf[MAXLINE];

	while ( (n = recv(this->connManager->readConnfd(), buf, MAXLINE,0)) > 0)
	{
		printf("%s","String received from and resent to the client:");
		puts(buf);
		send(this->connManager->readConnfd(), buf, n, 0);
	}

	if (n < 0)
	{
		perror("Read error");
		exit(1);
	}
}

void EchoServer::closeConnection()
{
	close(this->connManager->readConnfd());
	this->connManager->removeConnection();	
}

void EchoServer::down()
{
	close(listenfd);
}

void ConnManager::addConnection( int connfd )
{
	this->connfd = connfd;
}

int ConnManager::readConnfd()
{
	return this->connfd;
}

void ConnManager::removeConnection()
{
	this->connfd = 0;
}
