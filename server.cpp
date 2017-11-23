#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<netinet/in.h>
#include <string.h>
#include <system_error>
#include <errno.h>
#include <iostream>

#define SERVPORT 7809
#define MAXSIZE 70000
#define SYSTERR(info) std::system_error(errno, std::system_category(), info)

int main() {
	int servfd, connectfd;
	char filename[100];
	uint8_t buf[MAXSIZE];

	//creat socket fd
	if (-1 == (servfd = socket(AF_INET, SOCK_STREAM, 0))) {
		throw SYSTERR("server socket fd");
	}

	//bind
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVPORT);
	if (-1 == bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
		throw SYSTERR("server bind");
	}

	//listen
	if (-1 == listen(servfd, 0)) {
		throw SYSTERR("server listen");
	}

	//communication
	while(true) {
		//accept
		if (-1 == (connectfd = accept(servfd, (struct sockaddr *)NULL, NULL))) {
			std::cout << "server accept socket error: " << strerror(errno) << "(errno: " << errno << ")\n";
			continue;
		}

		//receive query: file name
		int size = recv(connectfd, filename, 100, 0);
		if (-1 == size) {
			throw SYSTERR("server receive");
		}
		filename[size] = '\0';

		//read image
		int imgfd = open(filename, O_RDONLY);
		if (-1 == imgfd) {
			throw SYSTERR("server open image");
		}
		if (-1 == read(imgfd, (void *)buf, MAXSIZE)) {
			throw SYSTERR("server send image");
		}

		//send
		if (!fork()) {
			if (-1 == send(connectfd, (void *)buf, sizeof(buf), 0)) {
				throw SYSTERR("server send");
			}
		}

		close(imgfd);
		close(connectfd);
	}
	close(servfd);

	return 0;
}
