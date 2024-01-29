#include "ServerConfig.hpp"

/* ServerConfigクラスの実装 */
void ServerConfig::loadConfiguration()
{
	/* ConfファイルをパースしてデータをServConfigクラスにセットする */
	this->servPort_ = 3001;
	this->listenQ_ = 8;
}

int ServerConfig::getServPort()
{
	return this->servPort_;
}

int ServerConfig::getListenQ()
{
	return this->listenQ_;
}

