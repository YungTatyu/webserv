#ifndef CONFIG_AUTOINDEX_HPP
#define CONFIG_AUTOINDEX_HPP

namespace config
{
class Autoindex
{
private:
	bool	is_autoindex_on;
public:
	Autoindex();
	~Autoindex();
	const static bool	kDefaultIsAutoindexOn = false;
};
} // namespace config

#endif