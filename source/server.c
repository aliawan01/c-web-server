#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/ip.h>

int main(void) {
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket < 0) {
		printf("There was an error when trying to CREATE the socket\n");
		exit(-1);
	}

	struct sockaddr_in address = {0};
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;

	int bind_socket = bind(tcp_socket, (const struct sockaddr*)&address, sizeof(address));
	if (bind_socket < 0) {
		printf("There was an error when trying to BIND the socket\n");
		exit(-1);
	}

	printf("Server listening on port 8080...\n");
	int listen_socket = listen(tcp_socket, 10);
	if (listen_socket < 0) {
		printf("There was an error when running the LISTEN function to put the socket into passive mode\n");
		exit(-1);
	}

	int num_of_connections = 0;
	while (true) {
		num_of_connections += 1;
		int addr_length = sizeof(address);
		int client_socket = accept(tcp_socket, (struct sockaddr*)&address, (socklen_t*)&addr_length);
		if (client_socket < 0) {
			printf("Error when running ACCEPT function, could be because there are no pending connections in the socket right now\n");
			exit(-1);
		}
		printf("Connected with client!\n");

		char information[1024];
		ssize_t read_status = read(client_socket, information, 1024);
		if (read_status < 0) {
			printf("There was an error when trying to run the READ function.");
			exit(-1);
		}
		else {
			printf("%s\n", information);
		}

		if (strncmp(information, "GET /favicon.ico", 16) == 0) {
			printf("Client asked for FAVICON\n");

			char* data_to_send = "HTTP/1.1 404 Not Found\r\n";
			ssize_t send_status = write(client_socket, (char*)data_to_send, strlen((char*)data_to_send));
			if (send_status < 0) {
				printf("Could not SEND message to client.\n");
			}
			else {
				printf("Get /favicon.ico request was responded with code 404: Not Found.\n");
			}
		}
		else {
			char buffer[1024];
			time_t time_since_epoch = time(NULL);
			struct tm* tm = gmtime(&time_since_epoch);
			strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %T %Z", tm);

			char* data_to_send = malloc(1000);
			sprintf(data_to_send, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n%s\r\n\r\n<html>\n<head><link rel='icon' href='data:;base64,='></head>\n<h1>Finally rendered the html</h1><p>This is some more information!</p>\n</html>", buffer);
			printf("%s\n", data_to_send);

			ssize_t send_status = write(client_socket, (char*)data_to_send, strlen((char*)data_to_send));
			if (send_status < 0) {
				printf("Could not SEND message to client.\n");
			}
			else {
				printf("Sent information to the client.\n");
			}

			free(data_to_send);
		}
		close(client_socket);
		printf("Disconnected client.\n");
		printf("Number of connections: %d\n", num_of_connections);
	}

	close(tcp_socket);

	return 0;
}


