#include <iostream>
#include <string>

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Socket.h"
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

#define BACKLOG	1024

Socket::Socket(Socket &&other): sockfd(other.sockfd) {
    other.sockfd = -1;
};

Socket& Socket::operator=(Socket &&other) {
    if (this != &other){
        Close();
        sockfd = other.sockfd;
        other.sockfd = -1;
    }

    return *this;
}

bool Socket::RecvAll(char *buffer, int n) {
    int received = 0;

    while(received < n) { 
        int length = recv(sockfd, buffer + received, n - received, 0);
        if (length <= 0) {
            return false;
        }
        received += length;
    }

    return true;
}

bool Socket::IsValid() const {
    return sockfd >= 0;
};

bool Socket::SendAll(const char *buffer, int n) {
    int sent = 0;

    while(sent < n) { 
        int length = send(sockfd, buffer + sent, n - sent, MSG_NOSIGNAL);
        if (length <= 0) {
            return false;
        }
        sent += length;
    }

    return true;
}

bool Socket::Connect(std::string ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    if (sockfd < 0) {
        perror("ERROR: failed to create socket");
        return false;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("ERROR: failed to connect");
        Close();
        return false;
    }
    
    return true;
}

bool Socket::Listen(int port) {
    int opt = 1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in my_addr;

    if (sockfd < 0) {
        perror("ERROR: failed to create socket");
        return false;
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("ERROR: setsockopt failed");
        Close();
        return false;
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr))) < 0) {
        perror("ERROR: failed to bind");
        Close();
        return false;
	}

    if (listen(sockfd, BACKLOG) < 0) {
        perror("ERROR: failed to listen");
        Close();
        return false;
    }
    std::cout << "Waiting for client to connect." << std::endl;
    
    return true;
}

Socket Socket::Accept() {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    
    Socket new_socket;

    int newfd = accept(sockfd, (struct sockaddr *) &addr, &addr_size);
    if (newfd < 0) {
        perror("ERROR: failed to accept");
        return new_socket;
    }

    new_socket.sockfd = newfd; 
    return new_socket;
}

bool Socket::Close() {
    if (sockfd < 0){ 
        return false;
    }
    close(sockfd);
    sockfd = -1;
    return true;
}