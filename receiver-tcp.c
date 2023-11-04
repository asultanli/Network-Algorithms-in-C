/*This C program is a simple TCP server that listens for incoming connections, 
receives a message from a client, and then prints the sender's IP address and 
port along with the received message. It operates within a specified port range, 
ensuring the port is valid. The program uses the socket, bind, and accept functions to establish and manage connections.*/

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP   "127.0.0.1"
#define SIZE 100
#define QUEUE_LENGTH 1

int main (int argc, char *argv [])
{
    /* test arg number */
    if (argc != 3) {
        fprintf(stderr, "usage: %s ip_addr port_number\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* convert and check port number */
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    if (port < 10000 || port > 65000) {
        fprintf(stderr, "Invalid port number. Please use a port number in the range [10000; 65000].\n");
        exit(EXIT_FAILURE);
    }
    /* create socket */
    int sockfd;
    CHECK(sockfd = socket(AF_INET, SOCK_STREAM, 0));
    /* SO_REUSEADDR option allows re-starting the program without delay */
    int iSetOption = 1;
    CHECK(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &iSetOption, sizeof iSetOption));

    /* complete struct sockaddr */
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

    int status = getaddrinfo(ip, argv[2], &hints, &res);
    if (status != 0) {
        fprintf(stderr, "%s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    /* link socket to local IP and PORT */
    CHECK(bind(sockfd, res->ai_addr, res->ai_addrlen));
    /* set queue limit for incoming connections */
    CHECK(listen(sockfd, QUEUE_LENGTH));
    /* wait for incoming TCP connection */
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int s = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
    char buffer[SIZE];
    /* print sender addr and port */
    ssize_t recvlen = recv(s, buffer, SIZE, 0);
    CHECK(recvlen);

    /* wait for incoming message */
    char host[NI_MAXHOST], serv[NI_MAXSERV];
    int get_name = getnameinfo((struct sockaddr*)&client_addr, client_addr_len, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
    CHECK(get_name);
    buffer[recvlen] = '\0';
    printf("%s %s\n",host, serv);
    printf("%s",buffer);
    // Close the socket
    close(s);

    // Free memory
    freeaddrinfo(res);

    /* print received message */
    //s = s + 1;
    return 0;
}

