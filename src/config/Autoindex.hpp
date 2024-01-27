#ifndef CONFIG_AUTOINDEX_HPP
#define CONFIG_AUTOINDEX_HPP

namespace config
{
class Autoindex
{
	private:
		bool	is_autoindex_on_;
	public:
		Autoindex() : is_autoindex_on_(this->kDefaultIsAutoindexOn_) {}
		~Autoindex() {}
		const static bool	kDefaultIsAutoindexOn_ = false;
		const bool	getIsAutoindexOn() const { return is_autoindex_on_; }
		void	setIsAutoindexOn(const bool is_autoindex_on) { this->is_autoindex_on_ = is_autoindex_on; }
};
} // namespace config

#endif