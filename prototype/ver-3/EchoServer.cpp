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

#include <iostream>
void printpfdvector( std::vector<struct pollfd> src, int level )
{
	std::cout << level << "-----------------" << std::endl;
	std::cout << "num of vec size: " << src.size() << std::endl;
	int i = 0;
	for ( std::vector<struct pollfd>::iterator src_it = src.begin(); src_it != src.end(); ++src_it )
	{
		std::cout << "element----" << i << std::endl;
		std::cout << "fd: " << src_it->fd << std::endl;
		std::cout << "events: " << src_it->events << std::endl;
		std::cout << "revents: " << src_it->revents << std::endl;
		++i;
	}
	std::cout << "-----------------" << std::endl;
}

void EchoServer::eventLoop()
{
	this->connManager->fds.push_back( {this->ioHandler->getListenfd(), POLLIN, 0} );
	// printpfdvector(this->connManager->fds, 0);

	for ( ; ; )
	{
		// printpfdvector(this->connManager->fds, 1);
		poll ( this->connManager->fds.data(), this->connManager->fds.size(), -1 );

		//　ここをイテレータで走査したら、要素を追加したときにイテレータが無効になったりしてバグる。
		size_t tmpsize = this->connManager->fds.size();
		for ( size_t i = 0; i < tmpsize; ++i )
		{
			// printpfdvector(this->connManager->fds, 2);

			if ( this->connManager->fds[i].revents & POLLIN )
			{
				if ( this->connManager->fds[i].fd == this->ioHandler->getListenfd() )	
				{
					this->ioHandler->acceptConnection( *this->connManager );
				}
				else
				{
					if ( this->ioHandler->receiveData( *this->connManager, this->connManager->fds[i].fd ) == -1 )
					{
						this->ioHandler->closeConnection( *this->connManager, this->connManager->fds[i].fd );
						// fd_it = this->connManager->fds.erase( fd_it );
						continue ;
					}
					this->requestHandler->handle( *this->connManager, this->connManager->fds[i].fd );
					this->ioHandler->sendData( *this->connManager, this->connManager->fds[i].fd  );
				}
			}
		}
	}
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
void ConnectionManager::addConnection( const struct pollfd& pfd )
{
	fds.push_back( pfd );
	connections[pfd.fd] = std::vector<char>();
}

void ConnectionManager::updateEvents( int fd, short revents )
{
	/*
	if ( events == POLLIN )
		connections[fd].pollfd.events = POLLOUT;
	else if ( events == POLLOUT )
		connections[fd].pollfd.events = POLLIN;
	*/
	for ( std::vector<struct pollfd>::iterator cur = fds.begin(); cur != fds.end(); ++cur )
	{
		if ( cur->fd == fd )
		{
			cur->events = POLLIN;
		}
	}
}

void ConnectionManager::removeConnection( int fd )
{
	connections.erase( fd );
}

void ConnectionManager::addContext( int fd, const std::vector<char>& context )
{
	connections[fd] = context;
}

const std::vector<char>& ConnectionManager::getContext( int fd ) const
{
	return connections.at(fd);
}

/* RequestHandlerクラスの実装 */
void RequestHandler::handle( ConnectionManager &connManager, int target )
{
	const std::vector<char>& context = connManager.getContext( target );
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
	
	printf("%s%d\n","Server running...waiting for connections on port ", servConfig->getServPort());
}

int NetworkIOHandler::receiveData( ConnectionManager& connManager, int target )
{
	// char *buf[MAXLINE];
	std::vector<char> buffer(1024);
	if ( recv( target, buffer.data(), buffer.size(), 0 ) <= 0 )
		return -1;
	connManager.addContext( target, buffer );
	return 0;
}

void NetworkIOHandler::sendData( ConnectionManager &connManager, int target )
{	
	send(target, connManager.getContext( target ).data(), connManager.getContext( target ).size(), 0);
}

void NetworkIOHandler::acceptConnection( ConnectionManager& connManager )
{
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t client;

	client = sizeof(cliaddr);
	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &client);
	connManager.addConnection({ connfd, POLLIN, 0 });
	printf("%s\n","New connection created.");
}

void NetworkIOHandler::closeConnection( ConnectionManager& connManager, int target )
{
	close( target );
	connManager.removeConnection( target );
	printf("Client disconnected.\n");
}

int NetworkIOHandler::getListenfd()
{
	return this->listenfd;
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
