
#include "network.h"

#include <iostream>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h> // close

int mk_bind_socket(const char* my_port) {
    int sockfd;
    int rv;
    addrinfo hints;
    addrinfo *servinfo;
    addrinfo *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; //

    if ((rv = getaddrinfo(NULL, my_port, &hints, &servinfo)) != 0) {
        std::cout << "error making addr info";
        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cout << "error making socket \n";
            continue;
        }
//        bind the socket to our address info
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            std::cout << "error binding \n";
            continue;
        }
        break;
    }
    if (p == NULL) {
        std::cout << "failed binding to socket \n";
        return -1;
    }

    return sockfd;
}
