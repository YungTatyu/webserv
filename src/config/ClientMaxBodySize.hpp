#ifndef CONFIG_CLIENT_MAX_BODY_SIZE
#define CONFIG_CLIENT_MAX_BODY_SIZE

#include "Size.hpp"

namespace config
{
class ClientMaxBodySize
{
	private:
		Size	size_;
	public:
		ClientMaxBodySize();
		~ClientMaxBodySize();
		const static unsigned long	kDefaultSize_ = Size::megabytes; // 1m
		const Size	&getSize() const { return this->size_; }
		void	setSize(unsigned long &size_in_bytes) { this->size_.size_in_bytes_ = size_in_bytes; }
};
} // namespace config


#endif