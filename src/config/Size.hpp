#ifndef CONFIG_SIZE_HPP
#define CONFIG_SIZE_HPP

namespace config {
class Size {
 public:
  Size() {}
  Size(unsigned long size_in_bytes) : size_in_bytes_(size_in_bytes) {}
  ~Size() {}
  unsigned long size_in_bytes_;
  const static unsigned int kilobytes = 1024;
  const static unsigned int megabytes = kilobytes * 1024;
  const static unsigned long kMaxSizeInBytes_;
};
}  // namespace config

#endif