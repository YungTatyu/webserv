#include <unistd.h>
#include <stdio.h>

int main() {
    char *args[] = {"document_response.py", NULL};
    execve("/Users/tatyu/Documents/webserv/test/cgi/cgi_files/executor/document_response.py", args, NULL);
    perror("execve failed");
    return 1;
}
