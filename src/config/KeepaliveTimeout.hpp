#ifndef CONFIG_KEEPALIVE_TIMEOUT_HPP
#define CONFIG_KEEPALIVE_TIMEOUT_HPP

#include "Time.hpp"

namespace config
{
class KeepaliveTimeout
{
private:
	Time	time;
public:
	KeepaliveTimeout();
	~KeepaliveTimeout();
};
} // namespace config


#endif