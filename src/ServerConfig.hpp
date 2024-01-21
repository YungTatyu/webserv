#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

/* Confファイルの設定を管理する */
class ServerConfig
{
	public:
		void loadConfiguration();	

		int getServPort();
		int getListenQ();

	private:
		int servPort_; /*port*/
		int listenQ_; /*maximum number of client connections */
};

#endif
