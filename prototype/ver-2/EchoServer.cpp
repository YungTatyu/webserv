#include "EchoServer.hpp"

Webserver::EchoServer()
{
	this->serverConfig = new ServerConfig();
	
	this->configManager->loadConfiguration(); 
	this->initializeServer();
}

void EchoServer::InitializeServer()
{
	this->ioHandler = new NetworkIOHandler();
	ioHandler->setupSocket(this->serverConfig->getPort(), this->serverConfig->getIPAddress());
}

void EchoServer::eventLoop()
{
	for ( ; ; )
	{
		int clientSocket = this->ioHandler->acceptConnection();
		this->connManager->addConnection(clientSocket);
		std::string requestData = this->ioHandler->receiveData(clientSocket);
		std::string responseData = this->requestHandler->handle(requestData);
		this->ioHandler->sendData(clientSocket, responseData);
		this->connManager->closeConnection(clientSocket);
	};
}

void EchoServer::~EchoServer()
{
	close( listenfd ); // ここはServerConfigのデストラクタでやってもいいかも
	delete this->ioHandler;
	delete this->requestHandler;
	delete this->connManager;
	delete this->serverConfig;
}

void RequestHandler::handle( ConnectionManager &connManager)
{
	char *buf[MAXLINE]
	recv(this->connManager.readConnfd(), buf. MAXLINE, 0);
	connManager.addContext(buf);
	puts(buf);
}

void NetworkIOHandler::sendData()
{	
	send(this->connManager->readConnfd(), buf, connManager.connections.value.size(), 0);
}

void NetworkIOHandler::acceptConnection()
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);
	printf("%s\n","Received request...");
}

void NetworkIOHandler::setupSocket()
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
