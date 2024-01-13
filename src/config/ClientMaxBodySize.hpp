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
};
} // namespace config


#endif