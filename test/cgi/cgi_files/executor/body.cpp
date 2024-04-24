#include <stdio.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char *argv[], char *envp[]) {
  printf("Status: 200\r\nContent-Type: text/html\r\n\r\n");
  const char *length = std::getenv("CONTENT_LENGTH");
  if (length == NULL) {
    std::cerr << "CONTENT_LENGTH not found\n";
    return 1;
  }
  size_t len = std::stoll(length);
  char buffer[len];

  if (read(0, buffer, len) == -1) {
    std::cerr << "read " << std::strerror(errno) << "\n";
  }
  printf("%s", buffer);
  return 0;
}
