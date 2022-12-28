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

char* show_first_page(dict* data) {
	return "<html><head><link rel='shortcut icon' href='data:,' /></head><h1>This is the first page!</h1></html>";
}

int main(void) {
	Http_Server server = {0};
	http_server_create(&server, "localhost", 8080);

	http_route_handle(&server, "GET", "/", &show_first_page);
	http_route_handle(&server, "GET", "/something", &show_first_page);

	char* current_route = "/";

	dict* dictionary = server.get_routes;
	char** keys = dict_get_keys(dictionary);
	int num_of_keys = dict_get_num_of_keys(dictionary);
	
	for (int i = 0; i < num_of_keys; i++) {
		char* key = keys[i];
		if (strcmp(current_route, key) == 0) {
			printf("Found it...\n");

			typedef char* (*response)(dict*);
			void* thing = dict_get_value(server.get_routes, key);
			response output = (response)thing;
			printf("%s\n", output(NULL));
		}

	}
	
	http_server_run(&server);

	return 0;
}

/* void handle_route(char* "GET", char* route, char* run_func()) */
/* void handle_route(char* "POST", char* route, char* run_func(dict* data_received)) */


int thing(void) {
	/* char uri_data[] = "fname=1%431%38d&lname=morningb1&something=print hello world"; */

	/* dict* dictionary = parse_uri_into_dict(uri_data); */

	/* char** keys = dict_get_keys(dictionary); */
	/* int num_of_keys = dict_get_num_of_keys(dictionary); */
	
	/* char** values_list = malloc(num_of_keys); */
	/* for (int i = 0; i < num_of_keys; i++) { */
	/* 	char* key = keys[i]; */
	/* 	char* value = (char*)dict_get_value(dictionary, key); */

	/* 	values_list[i] = malloc(100); */
	/* 	handle_escape_characters(values_list[i], value); */
	/* 	printf("new_value: %s\n", values_list[i]); */

	/* 	dict_update_value(dictionary, key, (void*)values_list[i]); */
	/* } */
	
	/* free(values_list); */

	/* dict_print_dict(dictionary, "char*"); */

	/* dict_delete_dict(dictionary); */

	return 0;
}
