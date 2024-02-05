#ifndef CONFIG_SEND_TIMEOUT_HPP
#define CONFIG_SEND_TIMEOUT_HPP

#include "conf.hpp"
#include "Time.hpp"

namespace config
{
class SendTimeout
{
	private:
		Time	time_;
	public:
		SendTimeout() : time_(this->kDefaultTime_) {}
		~SendTimeout() {}
		const static unsigned long	kDefaultTime_;
		const Time	&getTime() const { return this->time_; }
		void	setTime(const unsigned long &time_in_ms) { this->time_.time_in_ms_ = time_in_ms;}
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_TAKE1|CONF_UNIQUE;
};
} // namespace config


#endif
