#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <vector>

#define BUF_SIZE 1024

int	main(int ac, char *av[])
{
	if (ac < 5)
	{
		std::cout << "Usage: %s <ip address> <port> <sleep time> <request> ..." << std::endl;
		return (0);
	}

	const char*	server_addr = av[1];
	uint16_t	server_port = atoi(av[2]);
	int	sleep_time = atoi(av[3]);
	// request作成
	std::string	str;
	for (int i = 4; i < ac; i++)
	{
		str += av[i];
		str += "\r\n";
	}
	str += "\r\n";
	std::vector<unsigned char> request(str.begin(), str.end());

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "Error: socket:" << std::strerror(errno);
		exit (1);
	}

	//SOCK_MIN_RCVBUFのようなマクロが見つかればそれに差し替える
	int option_value = 1024; // 環境によって最小値はこれよりも大きいがそちらに合わせられるだけで問題はない。
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &option_value, sizeof(option_value)) == -1)
	{
		std::cerr << "Error: setsockopt:" << std::strerror(errno);
		exit(1);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(server_addr);
	addr.sin_port = htons(server_port);// network byte order (big endian) に変換

	// ソケット接続要求
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
	{
		std::cerr << "Error: connect:" << std::strerror(errno);
		exit(1);
	}

	// 送信
	if (send(sockfd, request.data(), request.size(), 0) == 0)
	{
		std::cerr << "Error: recv:" << std::strerror(errno);
		exit(1);
	}
	std::cout << "send: " << str << std::endl;

	// 受信せずにkeepalive_timeout + 1秒sleep
	sleep(sleep_time + 1);

	// 一度目のsendはserver側で接続がcloseされていても成功する
	// close されている場合RESETパケットが送られる。
	int ret = send(sockfd, request.data(), request.size(), 0);
	std::cout << "send byte: " << ret << std::endl;
	// 2回目のsendはcloseされていればsendは失敗する。
	ret = send(sockfd, request.data(), request.size(), 0);
	std::cout << "send byte: " << ret << std::endl;
	if (errno == ECONNRESET || errno == EPIPE)
	{
		std::cout << "connection timed out." << std::endl;
		close(sockfd);
		exit(1);
	}
	else
		std::cout << "connection didn't timeout." << std::endl;

	// send_timeoutで死ななかった場合は10秒だけ待ってcloseする
	sleep(10);
	close(sockfd);

	return (0);
}
