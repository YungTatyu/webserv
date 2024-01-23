#ifndef CONFIG_SIZE_HPP
#define CONFIG_SIZE_HPP

#include <limits>

namespace config
{
class Size
{
	public:
		Size() {}
		Size(const unsigned long &size_in_bytes) : size_in_bytes_(size_in_bytes) {}
		~Size() {}
		unsigned long	size_in_bytes_;
		const static unsigned int	kilobytes = 1000;
		const static unsigned int	megabytes = kilobytes * 1000;
		// 設定できる最大値 nginxを参照
		const static unsigned long	kMaxSizeInBytes_;
};
const unsigned long	Size::kMaxSizeInBytes_ = std::numeric_limits<long>::max();
} // namespace config

#endif