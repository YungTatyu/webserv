#ifndef CONFIG_TRY_FILES_HPP
#define CONFIG_TRY_FILES_HPP

#include <vector>
#include <string>

#include "conf.hpp"

namespace config
{
class TryFiles
{
	private:
		std::vector<std::string>	file_list_;
		std::string	uri_;
		unsigned int	code_;
	public:
		TryFiles() {}
		~TryFiles() {}
		const std::vector<std::string>	&getFileList() const { return this->file_list_; }
		const std::string	&getUri() const { return this->uri_; }
		const unsigned int	&getCode() const { return this->code_; }
		void	addFile(const std::string &file) { this->file_list_.push_back(file); }
		void	setUri(const std::string &uri) { this->uri_ = uri; }
		void	setCode(const unsigned int &code) { this->code_ = code; }
		const static unsigned int	kType_ = CONF_HTTP_SERVER|CONF_HTTP_LOCATION|CONF_2MORE|CONF_UNIQUE;
};
} // namespace config


#endif
