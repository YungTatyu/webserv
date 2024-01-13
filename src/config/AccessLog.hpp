#ifndef ACCESS_LOG_HPP
#define ACCESS_LOG_HPP

#include <string>

namespace config
{
class AccessLog
{
	private:
		std::string	file_;
	public:
		AccessLog();
		~AccessLog();
		const std::string	&get_file() const;
		void	set_file(const std::string	&file);
		const static char	*kDefaultFile_ = "logs/access.log";
};
} // namespace config


#endif