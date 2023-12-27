#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <map>

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

class ConnectionInfo
{
	public:
		struct pollfd pfd;
		std::vector<char> context;
};
// entityクラスの値をpublicで保持すべきかどうか？

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		void addConnection( const struct pollfd& pfd );
		void updateEvents( int fd, short revents );
		void removeConnection( int fd );
		void addContext( int fd, const std::vector<char>& context );
		const std::vector<char>& getContext( int fd ) const;
		std::vector<struct pollfd> fds;
	
	private:
		std::map<int, ConnectionInfo> connections;
		//std::map<struct pollfd, std::vector<char> context> connections; //これだとイベントが変わっただけで新しいキーが作成されるので使えない。
		//int connfd;
		//std::vector<char> context;
};

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class RequestHandler
{
	public:
		void handle( ConnectionManager &connManager, int target );
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		void setupSocket( ServerConfig *serverConfig );
		int receiveData( ConnectionManager& connManager, int target);
		void sendData( ConnectionManager& connManager, int target );
		void acceptConnection( ConnectionManager& connManager );
		void closeConnection( ConnectionManager& connManager, int target );
		int getListenfd();

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
