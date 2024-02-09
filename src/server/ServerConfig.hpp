#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include "Main.hpp"

/**
 * ServerConfig (ConfigReader) 方針
 * 基本的に、直接publicのconfig_を見てもらう
 * 個々のメソッドは補助関数のようなものだけ
 * この場合、すべてのクラスがこのクラスのオブジェクトを持たなければならない
*/ 

/* Confファイルの設定を管理する */
class ServerConfig
{
	public:
		// data
		const config::Main*	config_;

		// initialize
		ServerConfig() : config_(NULL) {};
		void	loadConfiguration( const config::Main* config );

		// method
		bool	allowReqest( const config::Server& server, config::Location& location ) const;
		const std::string&	getFile() const;
		void	writeAcsLog( const config::Server& server, const config::Location& location );
		void	writeErrLog( const config::Server& server, const config::Location& location );
		const config::Time&	getKeepaliveTimeout();
		const config::Time&	getSendTimeout();
		const config::Time&	getUseridExpires();
		const config::Size&	getClientMaxBodySize();

	private:
		//utils
		config::Server&	getServer( const std::string& server_name, const std::string& address, const unsigned int port );
		config::Location&	getLocation( const config::Server&server, const std::string& path );
		const std::string&	getFullPath( const config::Server& server, const config::Location& location );

	public:
		int		getServPort();
		int		getListenQ();

	private:
		int		servPort_; /*port*/
		int		listenQ_; /*maximum number of client connections */
};

#endif
