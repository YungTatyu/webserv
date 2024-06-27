#ifndef CONFIG_SIZE_HPP
#define CONFIG_SIZE_HPP

namespace config {
class Size {
 public:
  Size() : size_in_bytes_(0) {}
  Size(unsigned long size_in_bytes) : size_in_bytes_(size_in_bytes) {}
  Size(const Size& other) { *this = other; }
  ~Size() {}
  Size& operator=(const Size& other) {
    if (this != &other) {
      this->size_in_bytes_ = other.size_in_bytes_;
    }
    return *this;
  }
  unsigned long size_in_bytes_;
  const static unsigned int kilobytes = 1024;
  const static unsigned int megabytes = kilobytes * 1024;
  const static unsigned long kMaxSizeInBytes_;
};
}  // namespace config

#endif
