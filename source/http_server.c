// Don't forget to include things when you compile it separatley
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

enum Colors {
	BLACK = 0,
	RED = 1,
	YELLOW = 2,
	GREEN = 3
};

typedef struct {
	int socket,
	struct sockaddr_in address;
} Http_Server;

void http_log(int color, char* msg) {
	if (color == BLACK) {
		printf("\033[0;30m");
	} 
	else if (color == RED) {
		printf("\033[0;31m");
	} 
	else if (color == YELLOW) {
		printf("\033[0;32m");
	} 
	else if (color == GREEN) {
		printf("\033[0;33m");
	} 

	printf("%s", msg);
	printf("\033[0m");
}

void http_create_server(Http_Server* server, char* ip_address, int port) {
	server->socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server->socket < 0) {
		http_log(RED, "Could not CREATE the server, socket could be being used by another program.\n");
		exit(-1);
	}

	server->address = {0};
	server->address.sin_family = AF_INET;
	server->address.sin_port = htons(port);
	if (strcmp(ip_address, "localhost")) {
		server->address.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		server->address.sin_addr.s_addr = inet_addr(ip_address);
	}

	int bind_socket = bind(server->socket, (const struct sockaddr*)&server->address, sizeof(server->address));
	if (bind_socket < 0) {
		http_log(RED, "Error could not bind the socket BIND the socket\n");
		exit(-1);
	}
}

