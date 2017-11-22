#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <string.h>
#include <system_error>
#include <errno.h>
#include <iostream>
#include <string>

#define SYSTERR(info) std::system_error(errno, std::system_category(), info)
#define MAXSIZE 4096

int main(int argc, char ** argv) {
	int clientfd;
	char buf[4096];
	std::string filename;

	if (argc < 2) {
		throw SYSTERR("need server address");
	}

	if (-1 == (clientfd = socket(AF_INET, SOCK_STREAM, 0))) {
		throw SYSTERR("client socket fd");
	}

	struct sockaddr_in clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(7809);
	if (inet_pton(AF_INET, argv[1], &clientaddr.sin_addr) < 0) {
		throw SYSTERR("server address is error");
	}
	if (-1 == connect(clientfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr))) {
		throw SYSTERR("client connect");
	}

	std::cout << "connect server(addr: " << argv[1] << ":" << clientaddr.sin_port << ")\n";
	std::cout << "enter the message: ";
	std::cin.getline(filename, 100);

	if (-1 == send(clientfd, (const void *)filename.c_str(), filename.size(), 0)) {
		throw SYSTERR("client send");
	}

	int n = recv(clientfd, buf, MAXSIZE, 0);
	if (-1 == n) {
		throw SYSTERR("client receive");
	}
	buf[n] = '\0';
	std::cout << buf << std::endl;
	close(clientfd);

	return 0;
}
