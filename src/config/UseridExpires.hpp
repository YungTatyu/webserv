#ifndef CONFIG_USERID_EXPIRES_HPP
#define CONFIG_USERID_EXPIRES_HPP

#include "Time.hpp"

namespace config
{
class UseridExpires
{
	private:
		Time	time;
		bool	is_userid_expires_on;
	public:
		UseridExpires();
		~UseridExpires();
		const static bool	kDefaultIsUseridExpiresOn = false;
};
} // namespace config

#endif