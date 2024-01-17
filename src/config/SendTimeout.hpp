#ifndef CONFIG_SEND_TIMEOUT_HPP
#define CONFIG_SEND_TIMEOUT_HPP

#include "Time.hpp"

namespace config
{
class SendTimeout
{
	private:
		Time	time_;
	public:
		SendTimeout() {} // default time: 60s
		~SendTimeout() {}
};
} // namespace config


#endif