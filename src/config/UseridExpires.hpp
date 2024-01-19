#ifndef CONFIG_USERID_EXPIRES_HPP
#define CONFIG_USERID_EXPIRES_HPP

#include "Time.hpp"

namespace config
{
class UseridExpires
{
	private:
		Time	time_;
		bool	is_userid_expires_on_;
	public:
		UseridExpires();
		~UseridExpires();
		const static bool	kDefaultIsUseridExpiresOn = false;
		const Time	&getTime() const;
		const bool	getIsUseridExpiresOn() const;
		void	setTime(
			const unsigned int &d,
			const unsigned int &h,
			const unsigned int &m,
			const unsigned int &s,
			const unsigned int &ms
		);
		void	setIsUseridExpiresOn(const bool is_userid_expires_on);
};
} // namespace config

#endif