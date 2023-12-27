#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 7100
#define BACKLOG 5
#define BUF_SIZE 1024

int main()
{
    int sockfd, clientfd;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;
    char buf[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, BACKLOG) < 0) {
        perror("listen error");
        close(sockfd);
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        client_size = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_size);
        if (clientfd < 0) {
            perror("accept error");
            continue;
        }

        printf("Client connected.\n");

        ssize_t bytes_read;
        while ((bytes_read = read(clientfd, buf, BUF_SIZE - 1)) > 0) {
            buf[bytes_read] = '\0';  // null-terminate the buffer
            printf("Received data: %s\n", buf);

            if (write(clientfd, buf, bytes_read) < 0) {
                perror("write error");
                break;
            }
        }

        if (bytes_read < 0) {
            perror("read error");
        }

        close(clientfd);
        printf("Client disconnected.\n");
    }

    close(sockfd);
    return 0;
}

