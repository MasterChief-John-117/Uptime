#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>	// socket
#include <sys/types.h>	// Not strictly needed, but potentially required for BSD
#include <netinet/in.h>	// sockaddr_in

const int PORT = 8080;

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
	// If socket create fails, denoted by returning -1, print an error and exit
	if (server_fd < 0) {
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}

	// Set up the address for the socket to bind to
	// sockaddr_in is a struct defined by the following:
	//
	// #include <netinet/in.h>
	//
	// struct sockaddr_in {
	// 	short		sin_family;	// e.g. AF_INET
	// 	unsigned short	sin_port;	// e.g. htons(3490)
	// 	struct in_addr	sin_addr;	// see struct in_addr, below
	// 	char		sin_zero[8];	// zero this if you want to
	// };
	//
	// struct in_addr {
	// 	unsigned long	s_addr;		// load with inet_aton()
	// };
	//
	// sin_family refers to the address family used to set up the socket, in our case AF_INET
	//
	// sin_port is the port used to connect to the server. Normally it is manually assigned, but if set to 0
	// the operating system will choose a random port
	//
	// sin_addr is a struct representing the listen address for the server. This is just the IP address to bind to.
	// There's several ways to define it
	// 	INADDR_ANY		binds to 0.0.0.0, or every availabile IPv4 address
	// 	INADDR_LOOPBACK		binds to 127.0.0.1, the loopback IPv4 address
	//	inet_addr("X.X.X.X") 	binds to X.X.X.X
	// It can also be set using inet_aton(); as shown below
	// inet_aton("X.X.X.X", &myaddr.sin_addr.s_addr);
	//
	// htonl and htons are function to convert the host address to network long address and host address to network short address
	//
	// #include <arpa/inet.h>
	// // OR
	// #include <netinet/in.h>
	// uint32_t htonl(uint32_t hostlong);
	// uint16_t htons(uint16_t hostshort);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	// Bind the socket to the address we set up. Returns 0 on success or -1 on failure.
	// Bind is defined by
	// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	// The first argument is the socket file descriptor created earlier.
	// The second argument is a pointer to a sockaddr struct, but since we constructed a 
	// sockaddr_in struct, it must be converted to the proper type.
	// The third argument is the size of the address
	if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("Could not bind to address or port");
		exit(1);
	}
	return 0;
}
