#ifndef CONFIG_USERID_HPP
#define CONFIG_USERID_HPP

namespace config
{
class Userid
{
	private:
		bool	is_userid_on_;
	public:
		Userid() : is_userid_on_(this->kIsUseridOn_) {}
		~Userid() {}
		const static bool	kIsUseridOn_ = false;
		const bool	getIsUseridOn() const { return this->is_userid_on_; }
		void	setIsUseridOn(const bool is_userid_on) { this->is_userid_on_ = is_userid_on; }
};
} // namespace config

#endif