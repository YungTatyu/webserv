#ifndef CONFIG_SIZE_HPP
#define CONFIG_SIZE_HPP

#include <limits>

namespace config
{
class Size
{
	private:
		long	size_in_bytes_;
	public:
		Size();
		~Size();
		// 設定できる最大値 nginxを参照
		const static long	kMaxSizeInBytes = std::numeric_limits<long>::max();
};
} // namespace config

#endif