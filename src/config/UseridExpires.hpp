#ifndef CONFIG_USERID_EXPIRES_HPP
#define CONFIG_USERID_EXPIRES_HPP

#include "conf.hpp"
#include "Time.hpp"

namespace config
{
class UseridExpires
{
	private:
		Time	time_;
		bool	is_userid_expires_on_;
	public:
		UseridExpires() : is_userid_expires_on_(this->kDefaultIsUseridExpiresOn) {}
		~UseridExpires() {}
		const static bool	kDefaultIsUseridExpiresOn = false;
		const Time	&getTime() const { return this->time_; }
		bool	getIsUseridExpiresOn() const { return this->is_userid_expires_on_; }
		void	setTime(const unsigned long &time_in_ms) { this->time_.time_in_ms_ = time_in_ms; }
		void	setIsUseridExpiresOn(const bool is_userid_expires_on) { this->is_userid_expires_on_ = is_userid_expires_on; }
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1|CONF_UNIQUE;
};
} // namespace config

#endif