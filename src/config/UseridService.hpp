#ifndef CONFIG_USERID_SERVICE_HPP
#define CONFIG_USERID_SERVICE_HPP

#include "conf.hpp"

namespace config
{
class UseridService
{
	private:
		unsigned long	userid_service_;
	public:
		UseridService() {}
		~UseridService() {}
		const unsigned long	&getUseridService() const { return this->userid_service_; }
		void	setUseridService(const unsigned long userid_service) { this->userid_service_ = userid_service; }
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1|CONF_UNIQUE;
};
} // namespace config

#endif