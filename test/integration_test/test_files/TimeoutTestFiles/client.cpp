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
	if (ac != 5)
	{
		std::cout << "Usage: %s <ip address> <port> <msg> <sleep time>" << std::endl;
		return (0);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "Error: socket:" << std::strerror(errno);
		exit (1);
	}

	int option_value = 5;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &option_value, sizeof(option_value)) == -1)
	{
		std::cerr << "Error: setsockopt:" << std::strerror(errno);
		exit(1);
	}
	//struct linger lin;
	//if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin)) == -1)
	//{
	//	std::cerr << "Error: setsockopt:" << std::strerror(errno);
	//	exit(1);
	//}


	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(av[1]);
	addr.sin_port = htons(atoi(av[2])); // network byte order (big endian) に変換

	// ソケット接続要求
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
	{
		std::cerr << "Error: connect:" << std::strerror(errno);
		exit(1);
	}

	std::cout << "here" << std::endl;

	// 送信
	//char request[] = "GET /timeout5/ HTTP/1.1\r\nHost: _\r\n\r\n";
	char *request = av[3];
	//std::vector<unsigned char>	request = static_cast<std::vector<unsigned char>>(av[3]);
	//std::vector<unsigned char>	request;
	//while (*data != '\0') {
	//	request.push_back(static_cast<unsigned char>(*data));
	//	++data;
	//}
	//std::cout << strlen(request) << std::endl;
	send(sockfd, request, strlen(request), 0);
	//send(sockfd, request.data(), request.size(), 0);
	std::cout << "send: " << request << std::endl;

	// 受信
	char r_str[BUF_SIZE];
	int ret;
	//while (1)
	//{
		ret = recv(sockfd, r_str, BUF_SIZE, 0);
		if (ret == 0)
		{
			exit(1);
		}
		std::cout << "accept: " << r_str << std::endl;
		sleep(atoi(av[4]) + 1);
	//}
	if (send(sockfd, request, strlen(request), 0) == 0)
	{
		std::cout << "connection timeout." << std::endl;
		close(sockfd);
		exit(1);
	}

	int err = 0;
	socklen_t len = sizeof(err);
	while (1)
	{
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) != 0)
		{
			std::cerr << "Error: getsockopt:" << std::strerror(errno);
			return errno;
		}
		//std::cout << "err: " << err << std::endl;
		if (err != 0)
		{
			std::cout << "connection timeout." << std::endl;
			close(sockfd);
			exit(1);
		}
	}

	close(sockfd);

	return (0);
}
