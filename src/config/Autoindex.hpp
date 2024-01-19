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
		const static bool	kDefaultIsAutoindexOn_ = false;
		const bool	getIsAutoindexOn() const;
		void	setIsAutoindexOn(const bool is_autoindex_on);
};
} // namespace config

#endif