#include "ServerConfig.hpp"

/* ServerConfigクラスの実装 */
void ServerConfig::loadConfiguration()
{
	/* ConfファイルをパースしてデータをServConfigクラスにセットする */
	this->serv_port = 3001;
	this->listen_q = 8;
}

int ServerConfig::getServPort()
{
	return this->serv_port;
}

int ServerConfig::getListenQ()
{
	return this->listen_q;

}
