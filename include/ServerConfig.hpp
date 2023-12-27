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
		int serv_port; /*port*/
		int listen_q; /*maximum number of client connections */
};

#endif
