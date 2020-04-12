#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main() {
	// Create the server socket
	// The call for socket is as follows:
	// int server_fd = socket(domain, type, protocol);
	// domain refers to the address family. Some examples that could be used are
	// 	AF_INET		used for IPv4
	// 	AF_INET6	used for IPv6
	// 	AF_UNIX		used for local sockets, similar to pipes
	// type refers to the socket type, which could be
	// 	SOCK_STREAM	used for TCP
	// 	SOCK_DGRAM	used for UDP
	// 	SOCK_RAW	used to create a raw socket
	// protocol indicates a specific protocol for the domain and type of socket. 
	// In this case, there's only one protocol possible, so 0 is used
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}
}
