#ifndef CONFIG_INDEX_HPP
#define CONFIG_INDEX_HPP

#include <string>

#include "conf.hpp"

namespace config
{
class Index
{
	private:
		std::string	file_;
	public:
		Index() : file_(this->kDefaultFile_) {}
		~Index() {}
		const static char	*kDefaultFile_;
		const std::string	&getFile() const { return this->file_; }
		void	setFile(const std::string &file) { this->file_ = file; }
		const static unsigned int	kType_ = CONF_HTTP|CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_1MORE|CONF_NOT_UNIQUE;
};

const char	*Index::kDefaultFile_ = "index.html";
} // namespace config

#endif
