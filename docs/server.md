



class ClientSession()
{
	private:
		int	sock;
	public:

}

class WebServer
{
	private:
		ServerConfig				config_;
		std::vector<ClientSession>	clients_;
		WebServer();
	public:
		WebServer(ServerConfig server_config);
		~WebServer();

		void	setUp(); // bind(), listen()
		void	eventLoop(); // eventを待ちながら、accept() ClientSessionオブジェクトつくる。
		
		void	registerFd(); // select
		
};

class	ServerConfig //値オブジェクト
{
	private:
		細かく値オブジェクトにしたほうが、柔軟性はあがる。
		なぜなら、エラーログを出すときなどに出力先の情報を渡す必要があるが、この大きなオブジェクトを渡すと必要ない情報も相手に渡ることになるため。
		かといって、このオブジェクトから、このオブジェクトが持っているプロパティの一部を切り離して渡すことができるかはわからない。
}

class	Request()
{

}

class	Response()
{

}

