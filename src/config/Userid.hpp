#ifndef CONFIG_USERID_HPP
#define CONFIG_USERID_HPP

namespace config
{
class Userid
{
	private:
		bool	is_userid_on;
	public:
		Userid();
		~Userid();
		const static bool	kIsUseridOn = false;
};
} // namespace config

#endif