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
		ServerConfig() : config_(NULL) {};
		void	loadConfiguration( const config::Main* config );

		const config::Main*	config_;
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

#endif
