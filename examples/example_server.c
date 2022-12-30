#include <stdio.h>
#include <stdlib.h>
#include "http_server.h"

char* show_first_page(dict* data) {
	return get_image_from_file("static/first_page.html");
}

char* first_page_response(dict* data) {
	// This shows how we can print out the first name and last names we received.
	if (data != NULL) {
		dict* dictionary = data;
		char** keys = dict_get_keys(dictionary);	
		int num_of_keys = dict_get_num_of_keys(dictionary);

		char* name = (char*)malloc(500);
		memset(name, '\0', 500);
		for (int i = 0; i < num_of_keys; i++) {
			char* key = keys[i];
			char* value = dict_get_value(dictionary, key);
			strcat(name, value);
			if (i != (num_of_keys - 1)) {
				name[strlen(name)] = ' ';
			}
			else {
				name[strlen(name)] = '\0';
			}
		}

		free(keys);
		return name;
	}
	else {
		return "Unable to get first name and last name";
	}
}

char* show_second_page(dict* data) {
	return get_image_from_file("static/second_page.html");
}

int main(void) {
	Http_Server server = {0};
	http_server_create(&server, "localhost", 8080);

	http_route_handle(&server, "GET", "/", &show_first_page);
	http_route_handle(&server, "POST", "/", &first_page_response);
	http_route_handle(&server, "GET", "/second_page", &show_second_page);

	http_server_run(&server);

	return 0;
}
