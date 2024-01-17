#ifndef CONFIG_TRY_FILES_HPP
#define CONFIG_TRY_FILES_HPP

#include <vector>
#include <string>

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
		void	addFile(std::string &file) { this->file_list_.push_back(file); }
		void	setUri(std::string &uri) { this->uri_ = uri; }
		void	setCode(unsigned int &code) { this->code_ = code; }
};
} // namespace config


#endif
