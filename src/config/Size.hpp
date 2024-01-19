#ifndef CONFIG_SIZE_HPP
#define CONFIG_SIZE_HPP

#include <limits>

namespace config
{
class Size
{
	private:
		unsigned long	size_in_bytes_;
	public:
		Size();
		~Size();
		// 設定できる最大値 nginxを参照
		const static unsigned long	kMaxSizeInBytes = std::numeric_limits<long>::max();
		const unsigned long	&getSizeInBytes() const;
		void	setSizeInBytes(const unsigned long &size_in_bytes);
};
} // namespace config

#endif