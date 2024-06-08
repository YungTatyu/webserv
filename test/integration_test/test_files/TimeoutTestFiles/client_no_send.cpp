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
  if (ac < 3) {
    std::cout << "Usage: " << av[0] << " <ip address> <port> <sleep time> <request> ..." << std::endl;
    return (0);
  }

  const char *server_addr = av[1];
  uint16_t server_port = atoi(av[2]);
  int sleep_time = atoi(av[3]);

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

  // 送信しないでsend_timeout + 1秒待機
  sleep(sleep_time + 1);

  // 一度目のsendはserver側で接続がcloseされていても成功する
  // close されている場合RESETパケットが送られる。
  char request[] = "request";
  int ret = send(sockfd, request, strlen(request), 0);
  std::cout << "send byte: " << ret << std::endl;
  // 2回目のsendはcloseされていればsendは失敗する。
  ret = send(sockfd, request, strlen(request), 0);
  std::cout << "send byte: " << ret << std::endl;
  if (errno == ECONNRESET || errno == EPIPE) {
    std::cout << "connection timed out." << std::endl;
    close(sockfd);
    exit(1);
  }
  std::cout << "connection didn't timeout." << std::endl;

  // send_timeoutで死ななかった場合は10秒だけ待ってcloseする
  sleep(10);
  close(sockfd);

  return (0);
}
