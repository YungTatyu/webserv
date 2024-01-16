#ifndef CONFIG_INDEX_HPP
#define CONFIG_INDEX_HPP

#include <string>

namespace config
{
class Index
{
	private:
		std::string	file_;
	public:
		Index();
		~Index();
		const static char	*kDefaultFile_ = "index.html";
		const std::string	&getFile() const;
		void	setFile(const std::string &file);
};
} // namespace config

#endif
