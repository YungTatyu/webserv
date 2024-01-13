#ifndef CONFIG_AUTOINDEX_HPP
#define CONFIG_AUTOINDEX_HPP

namespace config
{
class Autoindex
{
	private:
		bool	is_autoindex_on_;
	public:
		Autoindex();
		~Autoindex();
		bool	get_is_autoindex_on() const;
		void	set_is_autoindex_on(const bool is_autoindex_on);
		const static bool	kDefaultIsAutoindexOn_ = false;
};
} // namespace config

#endif