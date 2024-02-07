#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

/* Confファイルの設定を管理する */
class ServerConfig
{
	public:
		void	loadConfiguration();

		int		getServPort();
		int		getListenQ();

	private:
		int		servPort_; /*port*/
		int		listenQ_; /*maximum number of client connections */
};

/**
 * ServerConfig (ConfigReader) 方針
 * 基本的に、直接config_を見てもらって、個々のメソッドは補助関数のようなものだけ
 * 1. ServerConfigクラスをstaticクラスにする
 *    本体のconfig_はグローバル変数にする
 *    exterm const config::Main* config_;
 * 2. publicでconfig_もつ
 * 　 この場合、すべてのクラスがこのクラスを持たなければならない
*/ 


// 1
class ConfigReadUtils
{
	public:
		//補助関数
	private:
		ConfigReadUtils();
		ConfigReadUtils( const ConfigReadUtils& copy );
		ConfigReadUtils&	ConfigReadUtils( const ConfigReadUtils& copy );
}


// 2
class ConfigReader
{
	public:
		ConfigReader( config::Main config ) : config_(config) {};
		const config::Main	config_;

		// 補助関数
};

#endif
