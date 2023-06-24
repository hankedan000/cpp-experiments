// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	const char* hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    while (1) {
        printf("accepting connection...\n");
        if ((new_socket
            = accept(server_fd, (struct sockaddr*)&address,
                    (socklen_t*)&addrlen))
            < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("reading\n");
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        const char *resp = "HTTP/1.1 200 OK\n"
            "Date: Sat, 09 Oct 2010 14:28:02 GMT\n"
            "Server: Apache\n"
            "Last-Modified: Tue, 01 Dec 2009 20:18:22 GMT\n"
            "ETag: \"51142bc1-7449-479b075b2891b\"\n"
            "Accept-Ranges: bytes\n"
            "Content-Length: 4\n"
            "Content-Type: text/html\n"
            "\n"
            "hello";
        send(new_socket, resp, strlen(resp), 0);
        printf("Hello message sent\n");

        // closing the connected socket
        printf("closing connection\n");
        close(new_socket);
    }

	// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}
