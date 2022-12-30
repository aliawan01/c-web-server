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
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dict.h"
#include "parse_uri.h"
#include "get.h"

enum Colors {
	BLACK = 0,
	RED = 1,
	YELLOW = 2,
	GREEN = 3,
	BLUE = 4
};

typedef struct {
	int socket;
	struct sockaddr_in address;
	dict* get_routes;
	dict* post_routes;
} Http_Server;

void http_log(int color, char* msg) {
	if (color == BLACK) {
		printf("\033[0;30m");
	} 
	else if (color == RED) {
		printf("\033[0;31m");
	} 
	else if (color == GREEN) {
		printf("\033[0;32m");
	} 
	else if (color == YELLOW) {
		printf("\033[0;33m");
	} 
	else if (color == BLUE) {
		printf("\033[0;34m");
	}

	printf("%s", msg);
	printf("\033[0m");
}

void http_server_create(Http_Server* server, char* ip_address, int port) {
	server->socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server->socket < 0) {
		http_log(RED, "Could not CREATE the server, socket could be being used by another program.\n");
		exit(-1);
	}

	server->address.sin_family = AF_INET;
	server->address.sin_port = htons(port);
	if (strcmp(ip_address, "localhost") == 0) {
		server->address.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		server->address.sin_addr.s_addr = inet_addr(ip_address);
	}

	int bind_socket = bind(server->socket, (const struct sockaddr*)&server->address, sizeof(server->address));
	if (bind_socket < 0) {
		http_log(RED, "Error could not BIND the socket\n");
		exit(-1);
	}
}

void http_route_handle(Http_Server* server, char* method, char* route, char* (*response)(dict*)) {
	if (strncmp(method, "GET", 3) == 0) {
		if (server->get_routes == NULL) {
			server->get_routes = create_dict(route, (void*)response);
		}
		else {
			dict_add_element(server->get_routes, route, (void*)response);
		}
	}
	else if (strncmp(method, "POST", 4) == 0) {
		if (server->post_routes == NULL) {
			server->post_routes = create_dict(route, (void*)response);
		}
		else {
			dict_add_element(server->post_routes, route, (void*)response);
		}
	}
}

void http_server_run(Http_Server* server) {
	http_log(GREEN, "Server listening on port 8080...\n");
	int listen_socket = listen(server->socket, 10);
	if (listen_socket < 0) {
		http_log(RED, "There was an error when running the LISTEN function to put the socket into passive mode\n");
		exit(-1);
	}

	int num_of_connections = 0;
	while (true) {
		num_of_connections += 1;
		int addr_length = sizeof(server->address);
		int client_socket = accept(server->socket, (struct sockaddr*)&server->address, (socklen_t*)&addr_length);
		if (client_socket < 0) {
			http_log(RED, "Error when running ACCEPT function, could be because there are no pending connections in the socket right now\n");
			exit(-1);
		}

		http_log(GREEN, "Connected to the client!\n");

		char* information = malloc(5024);
		ssize_t read_status = read(client_socket, information, 1024);
		if (read_status < 0) {
			http_log(RED, "There was an error when trying to run the READ function.\n");
			exit(-1);
		}
		else {
			printf("%s\n", information);
		}

		// Getting the route specified in the request
		char current_route[100];
		int current_route_i = 0;
		int first_space = 0;
		for (int i = 0; i < strlen(information); i++) {
			if ((i != 0) && (information[i-1] == ' ') && (first_space == 0)) {
				current_route[current_route_i] = information[i];
				++current_route_i;
				first_space = 1;
			}
			else if ((i != 0) && (information[i] == ' ') && (first_space != 0)) {
				current_route[current_route_i] = '\0';
				break;
			}
			else if (first_space != 0) {
				current_route[current_route_i] = information[i];
				++current_route_i;
			}
		}

		char* data_to_send = malloc(5000);
		memset(data_to_send, 0, (size_t)5000);
		int found = 0;
		if (strncmp(information, "GET", 3) == 0) {
			dict* dictionary = server->get_routes;
			char** keys = dict_get_keys(dictionary);
			int num_of_keys = dict_get_num_of_keys(dictionary);
			
			for (int i = 0; i < num_of_keys; i++) {
				char* key = keys[i];
				if (strcmp(current_route, key) == 0) {
					typedef char* (*response)(dict*);
					void* void_response = dict_get_value(server->get_routes, key);
					response output = (response)void_response;
					data_to_send = output(NULL);
					found = 1;
				}
			}

			free(keys);
		}
		else if (strncmp(information, "POST", 4) == 0) {
			char request_body[600];
			for (int i = 0; i < strlen(information); i++) {
				if ((i > 5) &&
					(information[i] == '\n') &&
					(information[i-1] == '\r') && 
					(information[i-2] == '\n') && 
					(information[i-3] == '\r')) {

					strcpy(request_body, information+i);
				}
			}

			dict* data_in_post_request = parse_uri_into_dict(request_body);

			dict* dictionary = server->post_routes;
			char** keys = dict_get_keys(dictionary);
			int num_of_keys = dict_get_num_of_keys(dictionary);
			
			for (int i = 0; i < num_of_keys; i++) {
				char* key = keys[i];
				if (strcmp(current_route, key) == 0) {
					typedef char* (*response)(dict*);
					void* void_response = dict_get_value(server->post_routes, key);
					response output = (response)void_response;
					data_to_send = output(data_in_post_request);
					found = 1;
				}
			}

			dict_delete_dict(data_in_post_request);
			data_in_post_request = NULL;
			free(keys);
		}

		if (found == 0) {
			strcpy(data_to_send, "<html><h1>404</h1><p>Page not found</p>");
		}

		char* send_data = malloc(5150);
		strcpy(send_data, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\r\n\r\n");
		strcat(send_data, data_to_send);

		ssize_t send_status = write(client_socket, (char*)send_data, strlen((char*)send_data));
		if (send_status < 0) {
			http_log(RED, "Could not SEND message to client.\n");
		}
		else {
			http_log(GREEN, "Sent information to the client.\n");
		}

		free(send_data);
		free(data_to_send);
		free(information);

		send_data = NULL;
		data_to_send = NULL;
		information = NULL;

		close(client_socket);
		http_log(YELLOW, "Disconnected client.\n");
		printf("Number of connections: %d\n", num_of_connections);
	}

	close(server->socket);
}
