#ifndef CONFIG_AUTOINDEX_HPP
#define CONFIG_AUTOINDEX_HPP

#include "conf.hpp"

namespace config
{
class Autoindex
{
	private:
		bool	is_autoindex_on_;
	public:
		Autoindex() : is_autoindex_on_(this->kDefaultIsAutoindexOn_) {}
		~Autoindex() {}
		const static bool	kDefaultIsAutoindexOn_ = false;
		bool	getIsAutoindexOn() const { return this->is_autoindex_on_; }
		void	setIsAutoindexOn(const bool is_autoindex_on) { this->is_autoindex_on_ = is_autoindex_on; }
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1;
};
} // namespace config

#endif