/*This C program is a compact file transfer client that sends a file's content to a server using an IPv6 connection. 
It checks for valid command-line arguments, establishes a connection with the server, and efficiently sends the file's content over the network.*/

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#define CHECK(op) do { if ((op) == -1) { perror(#op); exit(EXIT_FAILURE); } } while (0)

#define IP "::1"
#define SIZE 1024 // Taille des blocs de lecture/écriture

void cpy(int in, int out)
{
    char buffer[SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(in, buffer, SIZE)) > 0)
    {
        ssize_t bytes_written = write(out, buffer, bytes_read);
        if (bytes_written < 0)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "usage: %s ip_addr port_number filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    const char *filename = argv[3];

    if (port < 10000 || port > 65000)
    {
        fprintf(stderr, "Invalid port number. Please use a port number in the range [10000; 65000].\n");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    CHECK(sockfd = socket(AF_INET6, SOCK_STREAM, 0));

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

    int status = getaddrinfo(ip, argv[2], &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "%s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    CHECK(connect(sockfd, res->ai_addr, res->ai_addrlen));

    int file_fd;
    CHECK(file_fd = open(filename, O_RDONLY));

    // Send the file content
    cpy(file_fd, sockfd);

    close(file_fd);
    close(sockfd);

    freeaddrinfo(res);
    return 0;
}

