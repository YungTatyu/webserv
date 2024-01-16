#ifndef CONFIG_LISTEN_HPP
#define CONFIG_LISTEN_HPP

#include <string>

namespace config
{
class Listen
{
	private:
		std::string	address_;
		unsigned int	port_;
		bool	is_default_server_;
	public:
		Listen() {}
		~Listen() {}
		const static char	*kDefaultAddress_ = "127.0.0.1";
		const static unsigned int	kDefaultPort_ = 80;
		const static bool	kDefaultIsDefaultServer_ = false;
		const std::string	&getAddress() const;
		const unsigned int	&getport() const;
		bool	getIsDefaultServer() const;
		void	setAddress(const std::string &address);
		void	setport(const unsigned int &port);
		void	setIsDefaultServer(const bool is_default_server);
};
} // namespace config

#endif
