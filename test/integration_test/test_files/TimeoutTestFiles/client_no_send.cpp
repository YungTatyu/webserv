#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

#define BUF_SIZE 1024

int main(int ac, char *av[]) {
  if (ac < 2) {
    std::cout << "Usage: " << av[0] << " <ip address> <port> [<request>] ..." << std::endl;
    return (0);
  }

  const char *server_addr = av[1];
  uint16_t server_port = atoi(av[2]);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cerr << "Error: socket:" << std::strerror(errno);
    exit(1);
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(server_addr);
  addr.sin_port = htons(server_port);  // network byte order (big endian) に変換

  // ソケット接続要求
  if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    std::cerr << "Error: connect:" << std::strerror(errno);
    exit(1);
  }

  // サーバーに切断されるまでブロック
  std::vector<unsigned char> buffer(1024);
  while (1) {
    int ret = recv(sockfd, buffer.data(), 1024, 0);
    if (ret == -1) {
      std::cerr << "Error: recv: " << std::strerror(errno) << std::endl;
      break;
    } else if (ret == 0) {
      std::cout << "Connection closed by peer" << std::endl;
      break;
    } else {
      std::cout << "Received " << ret << " bytes" << std::endl;
    }
  }

  close(sockfd);

  return (0);
}
