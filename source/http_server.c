// Don't forget to include things when you compile it separatley
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "dict.h"

char* get_html_from_file(const char* path) {
	FILE* html_file = fopen(path, "rb");

	if (!html_file) {
		printf("No html file exists in the path you specified: %s\n", path);
		exit(-1);
	}

	fseek(html_file, 0, SEEK_END);
	int file_size = ftell(html_file);
	rewind(html_file);
	char* html_code = malloc(file_size + 1);
	html_code[file_size] = '\0';
	fread(html_code, sizeof(char), file_size, html_file);
	return html_code;
}

enum Colors {
	BLACK = 0,
	RED = 1,
	YELLOW = 2,
	GREEN = 3
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

		char information[1024];
		ssize_t read_status = read(client_socket, information, 1024);
		if (read_status < 0) {
			http_log(RED, "There was an error when trying to run the READ function.\n");
			exit(-1);
		}
		else {
			printf("%s\n", information);
		}


		close(client_socket);
		http_log(YELLOW, "Disconnected client.\n");
		printf("Number of connections: %d\n", num_of_connections);
	}

	close(server->socket);
}



		/* if (strncmp(information, "GET /favicon.ico", 16) == 0) { */
		/* 	printf("----------------------------\n"); */
		/* 	printf("Client asked for FAVICON\n"); */
		/* 	printf("----------------------------\n"); */

		/* 	char* image_data = get_image_from_file("favicon.ico"); */
		/* 	int image_data_size = get_image_file_size("favicon.ico"); */
		/* 	char* data_to_send = malloc(20000); */
		/* 	sprintf(data_to_send, "HTTP/1.1 200 OK\nContent-Length: %d\r\n\r\n%s", image_data_size, image_data); */

		/* 	ssize_t send_status = write(client_socket, (char*)data_to_send, strlen((char*)data_to_send)); */
		/* 	if (send_status < 0) { */
		/* 		printf("Could not SEND message to client.\n"); */
		/* 	} */
		/* 	else { */
		/* 		printf("Get /favicon.ico request was responded with the favicon.ico image.\n"); */
		/* 	} */

		/* 	free(data_to_send); */
		/* } */
		/* if (strncmp(information, "POST", 4) == 0) { */
		/* 	http_log(YELLOW, "got information\n"); */
		/* } */
		/* else { */
		/* 	/1* char buffer[1024]; *1/ */
		/* 	/1* time_t time_since_epoch = time(NULL); *1/ */
		/* 	/1* struct tm* tm = gmtime(&time_since_epoch); *1/ */
		/* 	/1* strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %T %Z", tm); *1/ */

		/* 	char* data_to_send = malloc(10000); */
		/* 	/1* char* html_data = get_html_from_file("static/first_page.html"); *1/ */

		/* 	/1* sprintf(data_to_send, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n%s\r\n\r\n%s", buffer, html_data); *1/ */

		/* 	strcpy(data_to_send, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><head><link rel='shortcut icon' href='data:,' /></head><h1>Hello World!</h1></html>"); */
		/* 	printf("%s\n", data_to_send); */

		/* 	ssize_t send_status = write(client_socket, (char*)data_to_send, strlen((char*)data_to_send)); */
		/* 	if (send_status < 0) { */
		/* 		http_log(RED, "Could not SEND message to client.\n"); */
		/* 	} */
		/* 	else { */
		/* 		http_log(GREEN, "Sent information to the client.\n"); */
		/* 	} */

		/* 	free(data_to_send); */
		/* 	free(html_data); */
		/* } */
