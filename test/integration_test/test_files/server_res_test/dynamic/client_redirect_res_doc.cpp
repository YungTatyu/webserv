#include <iostream>

int main() {
  const std::string response_header = std::string("Location: /\r\nContent-Type: text/html\r\n\r\n");
  const std::string body = "<h1>cgi response</h1><h2>client-redirdoc-response<h2>";
  std::cout << response_header << body << std::endl;
  return 0;
}