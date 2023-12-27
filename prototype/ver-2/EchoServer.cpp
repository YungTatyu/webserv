#include "EchoServer.hpp"

/* EchoServerクラスの実装 */
EchoServer::EchoServer()
{
	this->serverConfig = new ServerConfig();
	
	this->serverConfig->loadConfiguration(); 
	this->initializeServer();
}

void EchoServer::initializeServer()
{
	this->ioHandler = new NetworkIOHandler();
	this->ioHandler->setupSocket( this->serverConfig );

	this->requestHandler = new RequestHandler();
	this->connManager = new ConnectionManager();
}

void EchoServer::eventLoop()
{
	for ( ; ; )
	{
		this->ioHandler->acceptConnection( *this->connManager );
		this->ioHandler->receiveData( *this->connManager );
		this->requestHandler->handle( *this->connManager );
		this->ioHandler->sendData( *this->connManager );
		this->ioHandler->closeConnection( *this->connManager );
	};
}

EchoServer::~EchoServer()
{
	close( this->listenfd ); // リスニングソケットのクローズ 
	delete this->ioHandler;
	delete this->requestHandler;
	delete this->connManager;
	delete this->serverConfig;
}

/* ConnectionManagerクラスの実装 */
void ConnectionManager::addConnection( int connfd )
{
	this->connfd = connfd;
}

int ConnectionManager::getConnection()
{
	return this->connfd;
}

void ConnectionManager::removeConnection()
{
	this->connfd = 0;
}

void ConnectionManager::addContext( const std::vector<char>& context )
{
	this->context = context;
}

const std::vector<char>& ConnectionManager::getContext() const
{
	return this->context;
}

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager )
{
	const std::vector<char>& context = connManager.getContext();
	puts( context.data() );
}

/* NetworkIOHandlerクラスの実装 */
void NetworkIOHandler::setupSocket( ServerConfig *servConfig )
{
	struct sockaddr_in servaddr;

	//creation of the socket
	this->listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
	//preparation of the socket address
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons( servConfig->getServPort() );
	
	bind (this->listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen (this->listenfd, servConfig->getListenQ());
	
	printf("%s\n","Server running...waiting for connections.");
}

void NetworkIOHandler::receiveData( ConnectionManager& connManager )
{
	// char *buf[MAXLINE];
	std::vector<char> buffer(1024);
	recv( connManager.getConnection(), buffer.data(), buffer.size(), 0 );
	connManager.addContext( buffer );
}

void NetworkIOHandler::sendData( ConnectionManager &connManager )
{	
	send(connManager.getConnection(), connManager.getContext().data(), connManager.getContext().size(), 0);
}

void NetworkIOHandler::acceptConnection( ConnectionManager& connManager )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);
	connManager.addConnection( connfd );
	printf("%s\n","Received request...");
}

void NetworkIOHandler::closeConnection( ConnectionManager& connManager )
{
	close( connManager.getConnection() );
	connManager.removeConnection();
}

/* ServerConfigクラスの実装 */
void ServerConfig::loadConfiguration()
{
	/* ConfファイルをパースしてデータをServConfigクラスにセットする */
	this->serv_port = 3001;
	this->listen_q = 8;
}

int ServerConfig::getServPort()
{
	return this->serv_port;
}

int ServerConfig::getListenQ()
{
	return this->listen_q;

}
