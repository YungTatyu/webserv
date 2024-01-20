#ifndef CONFIG_LISTEN_HPP
#define CONFIG_LISTEN_HPP

#include <string>

#include "conf.hpp"

namespace config
{
class Listen
{
	private:
		std::string	address_;
		unsigned int	port_;
		bool	is_default_server_;
	public:
		Listen() :
			address_(this->kDefaultAddress_),
			port_(this->kDefaultPort_),
			is_default_server_(this->kDefaultIsDefaultServer_)
			{}
		~Listen() {}
		const static char	*kDefaultAddress_;
		const static unsigned int	kDefaultPort_ = 80;
		const static bool	kDefaultIsDefaultServer_ = false;
		const std::string	&getAddress() const { return this->address_; }
		const unsigned int	&getport() const { return this->port_; }
		const bool	getIsDefaultServer() const { return this->is_default_server_; }
		void	setAddress(const std::string &address) { this->address_ = address; }
		void	setport(const unsigned int &port) { this->port_ = port; }
		void	setIsDefaultServer(const bool is_default_server) { this->is_default_server_ = is_default_server; }
		const static unsigned int	type_ = CONF_HTTP_SERVER|CONF_TAKE12;
};

const char	*Listen::kDefaultAddress_ = "127.0.0.1";
} // namespace config

#endif
