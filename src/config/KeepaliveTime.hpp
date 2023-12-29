#ifndef CONFIG_KEEPALIVE_TIME_HPP
#define CONFIG_KEEPALIVE_TIME_HPP

#include "Time.hpp"

namespace config
{
class KeepaliveTime
{
private:
	Time	time;
public:
	KeepaliveTime();
	~KeepaliveTime();
};
} // namespace config


#endif