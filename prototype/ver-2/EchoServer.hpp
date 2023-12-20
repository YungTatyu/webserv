#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <template>>

#define MAXLINE 4096 /*max text line length*/
class ConnectionManager;

class EchoServer
{
	public:
		EchoServer();
		initializeServer();
		eventLoop();
	
	private:
		NetworkIOHandler *ioHandler;
		RequestHandler *requestHandler;
		ConnectionManager *connManager;
		ServerConfig *serverConfig;
};

/* コネクションの疎通したソケットとその直近のリクエストメッセージ情報を管理する */
class ConnectionManager
{
	public:
		addConnection();
		getConnectionData();
		removeConnection();
	
	private:
		std::map<int connfd, std:;string context> connections;
};

/* NetworkIOHandlerで受け取ったリクエストを処理する。リクエストデータはコネクションデータを介して受け取る */
class ReuqestHandler
{
	public:
		handle();
};

/* クライアントとデータの送受信を行う */
class NetworkIOHandler
{
	public:
		setUpSocket();
		receiveData();
		sendData();
		acceptConnection();
};

/* Confファイルの設定を管理する */
class ServerConfig
{
	public:
		// configParser();
		loadConfiguration();	

		getPort();
		getIPAddress();

	private:
		int listenfd; // ソケットのfd
		struct sockaddr_in servaddr; // ポートとかの情報
		static int SERV_PORT = 3001; /*port*/
		static int LISTENQ = 8; /*maximum number of client connections */
};
