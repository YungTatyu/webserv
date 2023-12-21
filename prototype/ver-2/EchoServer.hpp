#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class ConnectionManager;
class RequestHandler;
class NetworkIOHandler;
class ServerConfig;

class EchoServer
{
	public:
		EchoServer();
		~EchoServer();
		void initializeServer();
		void eventLoop();
	
	private:
		int listenfd; // listenソケットのfd
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		ServerConfig *serverConfig;
};

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		void addConnection( int connfd );
		int getConnection();
		void removeConnection();
		void addContext( const std::vector<char>& context );
		const std::vector<char>& getContext() const;
	
	private:
		//std::map<int connfd, std:;string context> connections;
		int connfd;
		std::vector<char> context;
};

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler
{
	public:
		void handle( ConnectionManager &connManager );
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ServerConfig *serverConfig );
		void receiveData( ConnectionManager& connManager );
		void sendData( ConnectionManager& connManager );
		void acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager );

	private:
		int listenfd;
};

/* Confファイルの設定を管理する */
class ServerConfig
{
	public:
		void loadConfiguration();	

		int getServPort();
		int getListenQ();

	private:
		int serv_port; /*port*/
		int listen_q; /*maximum number of client connections */
};
