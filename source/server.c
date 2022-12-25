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
#include "dict.h"
#include "parse_uri.h"
#include "http_server.c"

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


char* get_image_from_file(const char* path) {
	FILE* image_file = fopen(path, "rb");

	if (!image_file) {
		printf("No image exists in the path you specified: %s\n", path);
		exit(-1);
	}

	fseek(image_file, 0, SEEK_END);
	int file_size = ftell(image_file);
	rewind(image_file);
	char* image_bin_data = malloc(file_size + 1);
	image_bin_data[file_size] = '\0';
	fread(image_bin_data, sizeof(char), file_size, image_file);
	fclose(image_file);

	return image_bin_data;
}

int get_image_file_size(const char* path) {
	FILE* image_file = fopen(path, "rb");

	if (!image_file) {
		printf("No image exists in the path you specified: %s\n", path);
		exit(-1);
	}

	fseek(image_file, 0, SEEK_END);
	int file_size = ftell(image_file);
	rewind(image_file);
	fclose(image_file);

	return file_size + 1;
}

int main(void) {
	Http_Server tcp_socket;
	http_create_server(&tcp_socket, "localhost", 8080);

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
		if (strncmp(information, "POST", 4) == 0) {
			printf("got information\n");
		}
		else {
			char buffer[1024];
			time_t time_since_epoch = time(NULL);
			struct tm* tm = gmtime(&time_since_epoch);
			strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %T %Z", tm);

			char* data_to_send = malloc(1000);
			char* html_data = get_html_from_file("static/first_page.html");

			sprintf(data_to_send, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n%s\r\n\r\n%s", buffer, html_data);
			printf("%s\n", data_to_send);

			ssize_t send_status = write(client_socket, (char*)data_to_send, strlen((char*)data_to_send));
			if (send_status < 0) {
				printf("Could not SEND message to client.\n");
			}
			else {
				printf("Sent information to the client.\n");
			}

			free(data_to_send);
			free(html_data);
		}

		close(client_socket);
		printf("Disconnected client.\n");
		printf("Number of connections: %d\n", num_of_connections);
	}

	close(tcp_socket);

	return 0;
}

int thing(void) {
	char uri_data[] = "fname=1%431%38d&lname=morningb1&something=print hello world";

	dict* dictionary = parse_uri_into_dict(uri_data);

	char** keys = dict_get_keys(dictionary);
	int num_of_keys = dict_get_num_of_keys(dictionary);
	
	char** values_list = malloc(num_of_keys);
	for (int i = 0; i < num_of_keys; i++) {
		char* key = keys[i];
		char* value = (char*)dict_get_value(dictionary, key);

		values_list[i] = malloc(100);
		handle_escape_characters(values_list[i], value);
		printf("new_value: %s\n", values_list[i]);

		dict_update_value(dictionary, key, (void*)values_list[i]);
	}
	
	free(values_list);

	dict_print_dict(dictionary, "char*");

	dict_delete_dict(dictionary);

	return 0;
}
