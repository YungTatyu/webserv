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
		Index() : file_(this->kDefaultFile_) {}
		~Index() {}
		const static char	*kDefaultFile_;
		const std::string	&getFile() const { return this->file_; }
		void	setFile(const std::string &file) { this->file_ = file; }
};

const char	*Index::kDefaultFile_ = "index.html";
} // namespace config

#endif
