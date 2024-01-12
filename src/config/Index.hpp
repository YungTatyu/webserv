#ifndef CONFIG_INDEX_HPP
#define CONFIG_INDEX_HPP

#include <string>

namespace config
{
class Index
{
	private:
		std::string	file;
	public:
		Index();
		~Index();
		const static char	*kDefaultFile = "index.html";
};
} // namespace config


#endif