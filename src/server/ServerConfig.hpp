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
 * 　 この場合、すべてのクラスがこのクラスのオブジェクトを持たなければならない
 * 3. すべてのディレクティブのgetterを作る。
 *    メリットは使う側のコードはすっきりすること。
 * 4. sigletonクラスで作る
*/ 


// 1 static class
class ConfigReadUtils
{
	public:
		//補助関数
		const config::Server	&getServer(); // address port server_name
		const config::Loaction	&getLocation(const std::string& url);
		std::string	getFullPath();
		void	writeErrorLog();
	private:
		ConfigReadUtils();
}


// 2
class ConfigReader
{
	public:
		ConfigReader( config::Main config ) : config_(config) {};
		const config::Main	config_;

		// 補助関数
};

// 4 singleton
class ConfigSingleton
{
	public:
		static ConfigSingleton& getInstance() {
			if (!instanceInitialized_)
			{
				initSingleton();
				instanceInitialized_ = true;
			}
			return *instance_;
		}

	private:
		ConfigSingleton() {}
		ConfigSingleton(const ConfigSingleton& copy);
		const ConfigSingleton&	operato=(const ConfigSingleton& copy);

		static void	initSingleton() {
			instance = new ConfigSingleton;
		}
		static void lexerConfigSingleton();
		static void parseConfigSingleton();

		static const ConfigSingleton* instance_;
		static bool	instanceInitialized_;
		const std::string& config_file_;
		config::Token	&tokens_;
};

#endif
