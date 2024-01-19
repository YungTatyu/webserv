#ifndef CONFIG_USERID_HPP
#define CONFIG_USERID_HPP

namespace config
{
class Userid
{
	private:
		bool	is_userid_on_;
	public:
		Userid();
		~Userid();
		const static bool	kIsUseridOn = false;
		const bool	getIsUseridOn() const;
		void	setIsUseridOn(const bool is_userid_on);
};
} // namespace config

#endif