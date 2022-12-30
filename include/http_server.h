#pragma once
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

// dict.h
typedef struct dict_struct {
	char* key;
	void* value;
	struct dict_struct* next;
} dict;

dict* create_dict(char* key, void* value);
int dict_delete_element(dict* dictionary, char* key);
void dict_add_element(dict* dictionary, char* key, void* value);
void dict_print_dict(dict* dictionary, char* print_type);
int dict_get_num_of_keys(dict* dictionary);
char** dict_get_keys(dict* dictionary);
void* dict_get_value(dict* dictionary, char* key);
void dict_update_value(dict* dictionary, char* key, void* value);
void dict_delete_dict(dict* dictionary);

// parse_uri.h
void chop_string(char* dest, char* string, int start_index, int end_index);
void chop_string_from_array(char** dest, char* string, int start_index, int end_index, int dest_index);
void handle_escape_characters(char* dest, char* uri_data);
dict* parse_uri_into_dict(char* uri);

// get.h
char* get_image_from_file(const char* path);
int get_image_file_size(const char* path);

// http_server.c
typedef struct {
	int socket;
	struct sockaddr_in address;
	dict* get_routes;
	dict* post_routes;
} Http_Server;

void http_server_create(Http_Server* server, char* ip_address, int port);
void http_route_handle(Http_Server* server, char* method, char* route, char* (*response)(dict*));
void http_server_run(Http_Server* server);
