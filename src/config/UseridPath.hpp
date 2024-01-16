#ifndef CONFIG_USERID_PATH_HPP
#define CONFIG_USERID_PATH_HPP

#include <string>

namespace config
{
class UseridPath
{
	private:
		std::string	path_;
	public:
		UseridPath();
		~UseridPath();
		const static char	*kDefaultPath_ = "/";
		const std::string	&getPath() const;
		void	setPath(const std::string &path);
};
} // namespace config

#endif