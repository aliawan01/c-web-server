#include <stdio.h>
#include <stdlib.h>
#include "http_server.c"

char* show_first_page(dict* data) {
	return get_html_from_file("static/first_page.html");
}

char* show_second_page(dict* data) {
	return get_html_from_file("static/second_page.html");
}

char* first_page_response(dict* data) {
	// This will print the first name and last name we give into the form.
	if (data != NULL) {
		dict* dictionary = data;
		char** keys = dict_get_keys(dictionary);
		int num_of_keys = dict_get_num_of_keys(dictionary);
		char** values = malloc(sizeof(char*)*num_of_keys);

		char* names = malloc(600);
		for (int i = 0; i < num_of_keys; i++) {
			values[i] = malloc(100);
			strcpy(values[i], (char*)dict_get_value(dictionary, keys[i]));
			printf("values[i]: %s\n", values[i]);
			strcat(names, values[i]);

			if (i != (num_of_keys - 1)) {
				sprintf(names, "%s ", names);
			}		
		}

		free(values);
		return names;
	}
	else {
		return "Could not load data sent by server";
	}
}

int main(void) {
	Http_Server server = {0};
	http_server_create(&server, "localhost", 8080);

	http_route_handle(&server, "GET", "/", &show_first_page);
	http_route_handle(&server, "POST", "/", &first_page_response);
	http_route_handle(&server, "GET", "/second", &show_second_page);

	http_server_run(&server);

	return 0;
}


int thing(void) {
	/* char uri_data[] = "fname=1%431%38d&lname=morningb1&something=print hello world"; */

	/* dict* dictionary = parse_uri_into_dict(uri_data); */

	/* dict_print_dict(dictionary, "char*"); */
	/* dict_delete_dict(dictionary); */

	return 0;
}
