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
		UseridExpires() : is_userid_expires_on_(kDefaultIsUseridExpiresOn) {}
		~UseridExpires() {}
		const static bool	kDefaultIsUseridExpiresOn = false;
		const Time	&getTime() const { return this->time_; }
		const bool	getIsUseridExpiresOn() const { return this->is_userid_expires_on_; }
		void	setTime(
			const unsigned int &d,
			const unsigned int &h,
			const unsigned int &m,
			const unsigned int &s,
			const unsigned int &ms
		)
		{ this->time_.setTime(d, h, m, s, ms); }
		void	setIsUseridExpiresOn(const bool is_userid_expires_on) { this->is_userid_expires_on_ = is_userid_expires_on; }
};
} // namespace config

#endif