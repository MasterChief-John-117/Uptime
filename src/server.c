#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/socket.h>	// socket
#include <sys/types.h>	// Not strictly needed, but potentially required for BSD
#include <netinet/in.h>	// sockaddr_in

const int PORT = 8080;
const int BACKLOG = 1024;

int main() {
	/*
	* Create the server socket
	* The call for socket is as follows:
	* int server_fd = socket(domain, type, protocol);
	* domain refers to the address family. Some examples that could be used are
	* 	AF_INET		used for IPv4
	* 	AF_INET6	used for IPv6
	* 	AF_UNIX		used for local sockets, similar to pipes
	* type refers to the socket type, which could be
	* 	SOCK_STREAM	used for TCP
	* 	SOCK_DGRAM	used for UDP
	* 	SOCK_RAW	used to create a raw socket
	* protocol indicates a specific protocol for the domain and type of socket. 
	* In this case, there's only one protocol possible, so 0 is used
	*/
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// If socket create fails, denoted by returning -1, print an error and exit
	if (server_fd < 0) {
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}

	/*
	* Set up the address for the socket to bind to
	* sockaddr_in is a struct defined by the following:
	*
	* #include <netinet/in.h>
	*
	* struct sockaddr_in {
	* 	short		sin_family;	// e.g. AF_INET
	* 	unsigned short	sin_port;	// e.g. htons(3490)
	* 	struct in_addr	sin_addr;	// see struct in_addr, below
	* 	char		sin_zero[8];	// zero this if you want to
	* };
	*
	* struct in_addr {
	* 	unsigned long	s_addr;		// load with inet_aton()
	* };
	*
	* sin_family refers to the address family used to set up the socket, in our case AF_INET
	*
	* sin_port is the port used to connect to the server. Normally it is manually assigned, but if set to 0
	* the operating system will choose a random port
	*
	* sin_addr is a struct representing the listen address for the server. This is just the IP address to bind to.
	* There's several ways to define it
	* 	INADDR_ANY		binds to 0.0.0.0, or every availabile IPv4 address
	* 	INADDR_LOOPBACK		binds to 127.0.0.1, the loopback IPv4 address
	*	inet_addr("X.X.X.X") 	binds to X.X.X.X
	* It can also be set using inet_aton(); as shown below
	* inet_aton("X.X.X.X", &myaddr.sin_addr.s_addr);
	*
	* htonl and htons are function to convert the host address to network long address and host address to network short address
	*
	* #include <arpa/inet.h>
	* * OR
	* #include <netinet/in.h>
	* uint32_t htonl(uint32_t hostlong);
	* uint16_t htons(uint16_t hostshort);
	*/
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	/*
	* Bind the socket to the address we set up. Returns 0 on success or -1 on failure.
	* Bind is defined by
	* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	* The first argument is the socket file descriptor created earlier.
	* The second argument is a pointer to a sockaddr struct, but since we constructed a 
	* sockaddr_in struct, it must be converted to the proper type.
	* The third argument is the size of the address
	*/
	if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("Could not bind to address or port");
		exit(EXIT_FAILURE);
	}

	/*
	* listen() tells a server to listen to a socket and is defined as below:
	* int listen(int sockfd, int backlog);
	* It returns 0 on success and -1 on failure 
	* The backlog argument specifies the number of connections that can be waiting before
	* the system stops accepting new connection. If backlog is greater than the value in /proc/sys/net/ipv4/tcp_max_syn_backlog,
	* it will be reduced to that value. 
	*/
	if (listen(server_fd, BACKLOG) < 0) {
		perror("Error listening to socket");
		exit(EXIT_FAILURE);
	}

	/*
	 * Accept the new connection. accept is defined as below:
	 * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	 * The first argument is the original socket descriptor
	 * The second argument is the address struct that will be populated with the connecting client's address
	 * The third argument is the length of this struct, but cast to a socklen_t pointer
	 * addr and addrlen can be set to NULL. If addr is NULL, addrlen should be NULL as well.
	 * the accept() call will block until a client connects
	 */
	int client_fd, client_addr_len;
	struct sockaddr_in client_addr;
	client_addr_len = sizeof(client_addr);
	if ((client_fd = accept(server_fd, (struct sockaddr*) &client_addr, (socklen_t*)&client_addr_len)) < 0) {
		perror("Error accepting new connection");
		exit(EXIT_FAILURE);
	}

	/*
	 * If we don't do this, looping over read will block forever or until the client terminates the connection
	 * I stole this from https://stackoverflow.com/a/22339017 and I'm not quite sure how it works, but it does
	 */
	if (fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
		perror("Error making client socket non-blocking");
		exit(EXIT_FAILURE);
	}

	/*
	 * First allocate a buffer of 4096 bytes and set all values to \0
	 * Read a maxiumum of 4096 bytes from the client socket into buffer and print that buffer.
	 * It's worth noting read is blocking, so if done in a loop it will block indefinitely or until the 
	 * client closes the connection. We'll fix that later.
	 */
	char* buffer = calloc(sizeof(char), 256);
	while(read(client_fd, buffer, 255) > 0) {
		printf("%s", buffer);
		buffer = calloc(sizeof(char), 256);
	}
	puts("\n");
	// Close the connection to the client and exit
	close(client_fd);
	return 0;
}
