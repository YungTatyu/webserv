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
		bool	getIsAutoindexOn() const;
		void	setIsAutoindexOn(const bool is_autoindex_on);
		const static bool	kDefaultIsAutoindexOn_ = false;
};
} // namespace config

#endif