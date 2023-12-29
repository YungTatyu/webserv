#ifndef ACCESS_LOG_HPP
#define ACCESS_LOG_HPP

#include <string>

namespace config
{
class AccessLog
{
private:
	std::string	file;
public:
	AccessLog();
	~AccessLog();
	const static char	*kDefaultFile = "logs/access.log";
};
} // namespace config


#endif