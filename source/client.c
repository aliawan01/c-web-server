#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main(void) {
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		printf("There was an error when trying to CREATE the client socket\n");
		exit(-1);
	}

	struct sockaddr_in server_address = {0};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080);

	int server_addr_status = inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
	if (server_addr_status < 0) {
		printf("error when running the inet_pton() function\n");
		exit(-1);
	}

	int client_server_connection_status = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
	if (client_server_connection_status < 0 ) {
		printf("Error with the CONNECT function, could not connect the client to the server\n");
		exit(-1);
	}

	const char* client_to_server_msg = "hi this is a  message from me the client!";
	send(client_socket, client_to_server_msg, strlen(client_to_server_msg), 0);

	char server_msg[1024];
    read(client_socket, server_msg, 1024);
	printf("%s\n", server_msg);

	close(client_socket);

	return 0;
}
