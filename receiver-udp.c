/*This C program is a compact IPv6 UDP server that listens for incoming datagrams, prints the sender's IPv6 address and port, 
along with the received message. It operates within a specified port range and ensures the port number is valid.*/


#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op) do { if ((op) == -1) { perror(#op); exit(EXIT_FAILURE); } } while (0)

#define IP "::1"
#define SIZE 100

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
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(ip, argv[2], &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "Name or service not known: %s\n",gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // Link socket to local IP and PORT
    CHECK(bind(sockfd, res->ai_addr, res->ai_addrlen));

    struct sockaddr_in6 client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[SIZE];

    // Wait for an incoming message
    ssize_t recvlen = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
    CHECK(recvlen);

    // Print sender address and port
    char sender_ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &(client_addr.sin6_addr), sender_ip, INET6_ADDRSTRLEN);
    
    buffer[recvlen] = '\0';
	printf("%s%s %d\n", buffer, sender_ip, ntohs(client_addr.sin6_port));

    // Close the socket
    close(sockfd);

    // Free memory
    freeaddrinfo(res);

    return 0;
}

