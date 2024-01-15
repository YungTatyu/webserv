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
		std::vector<std::string>	&get_file_list() const;
		std::string	&get_uri() const;
		unsigned int	&get_code() const;
		void	add_file(std::string &file);
		void	set_uri(std::string &uri);
		void	set_code(unsigned int &code);
};
} // namespace config


#endif
