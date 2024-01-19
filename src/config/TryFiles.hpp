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
		std::vector<std::string>	&getFileList() const;
		std::string	&getUri() const;
		unsigned int	&getCode() const;
		void	addFile(std::string &file);
		void	setUri(std::string &uri);
		void	setCode(unsigned int &code);
};
} // namespace config


#endif
