/*This C program is a compact IPv6 UDP client that sends a "hello world" message to a specified server using the given IPv6 address and port number. 
It validates the command-line arguments, establishes a connection, sends the message, and then closes the connection.*/

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define CHECK(op) do { if ((op) == -1) { perror (#op); exit (EXIT_FAILURE); } } while (0)

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s ip_addr port_number\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    if (port < 10000 || port > 65000)
    {
        fprintf(stderr, "Invalid port number. Please use a port number in the range [10000; 65000].\n");
        exit(EXIT_FAILURE);
    }

    // Create an IPv6 UDP socket
    int sockfd;
    CHECK(sockfd = socket(AF_INET6, SOCK_DGRAM, 0));

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(ip, argv[2], &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // Send message to the remote peer
    const char *msg = "hello world";
    ssize_t sendmsg = sendto(sockfd, msg, strlen(msg), 0, res->ai_addr, res->ai_addrlen);
    CHECK(sendmsg);

    freeaddrinfo(res);

    // Close the socket
    close(sockfd);

    return 0;
}

