#include "ConfigHandler.hpp"

/* ConfigHandlerクラスの実装 */
void ConfigHandler::loadConfiguration( const config::Main* config )
{
	/* ConfファイルをパースしてデータをServConfigクラスにセットする */
	this->config_ = config;
	this->servPort_ = 3001;
	this->listenQ_ = 8;
}

int ConfigHandler::getServPort()
{
	return this->servPort_;
}

int ConfigHandler::getListenQ()
{
	return this->listenQ_;
}

