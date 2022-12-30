#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

void chop_string(char* dest, char* string, int start_index, int end_index) {
	end_index -= start_index;
	string += start_index;
	strncpy(dest, string, end_index);
}

void chop_string_from_array(char** dest, char* string, int start_index, int end_index, int dest_index) {
	end_index -= start_index;
	string += start_index;
	dest[dest_index] = NULL;
	dest[dest_index] = calloc(200, sizeof(char));
	strncpy(dest[dest_index], string, end_index);
}

void handle_escape_characters(char* dest, char* uri_data) {
	// Escape Characters:
	// = %61
	// & %38
	// % %37
	// + %43

	void copy_character(int* prev, int* i, char character) {
		int cur_i = *i;
		if (*prev == 0) {
			memmove((void*)dest, (void*)uri_data, cur_i+1);
			dest[cur_i] = character;
			*prev = cur_i;
		}
		else {
			uri_data[cur_i] = character;
			char* volatile uri_data_copy = malloc(strlen(uri_data));
			chop_string(uri_data_copy, uri_data, (*prev)+3, cur_i+1);
			if (cur_i == (strlen(uri_data) - 3)) {
				uri_data_copy[1] = '\000';
			}
			strcat(dest, uri_data_copy);
			free(uri_data_copy);
			uri_data[cur_i] = '%';
			*prev = cur_i;
		}
	}

	for (int i = 0, prev = 0; i < strlen(uri_data); i++) {
		if (uri_data[i] == '%') {
			char new_string[3];
			chop_string(new_string, uri_data, i, i+3);

			if (strncmp(new_string, "%61", 3) == 0) {
				copy_character(&prev, &i, '=');
			}
			else if (strncmp(new_string, "%38", 3) == 0) {
				copy_character(&prev, &i, '&');
			}
			else if (strncmp(new_string, "%37", 3) == 0) {
				copy_character(&prev, &i, '%');
			}
			else if (strncmp(new_string, "%43", 3) == 0) {
				copy_character(&prev, &i, '+');
			}
		}
		else if ((i == (strlen(uri_data)-1)) && (prev != 0)) {
			if (i-2 == '%') {
				char* uri_data_copy = malloc(strlen(uri_data));
				chop_string(uri_data_copy, uri_data, prev+3, i+1);
				strcat(dest, uri_data_copy);
				free(uri_data_copy);
				dest[strlen(dest)-1] = '\0';
			}	
		}
		else if ((i == (strlen(uri_data)-1)) && (prev == 0)) {
			strcpy(dest, uri_data);
		}
	}
}

dict* parse_uri_into_dict(char* uri) {
	dict* dictionary = NULL;
	int uri_len = strlen(uri);

	int prev = 0;
	char key_val[500];
	char** value = malloc(1000); 
	char** new_value = malloc(1000);
	int value_i = 0;
	int new_value_i = 0;

	for (int i = 0; i < uri_len; i++) {
		if ((prev == 0) && (uri[i] == '=')) {
			memset(key_val, 0, 500);
			chop_string(key_val, uri, 0, i);
			prev = i;
		}
		else if ((prev != 0) && (uri[i] == '=')) {
			memset(key_val, 0, 500);
			chop_string(key_val, uri, prev+1, i);
			prev = i;
		}
		else if ((prev != 0) && ((uri[i] == '&') || (i == (uri_len-1)))) {
			if (i == (uri_len-1)) {
				i += 1;
			}
			chop_string_from_array(value, uri, prev+1, i, value_i);
			prev = i;

			new_value[new_value_i] = malloc(100);
			handle_escape_characters(new_value[new_value_i], value[value_i]);

			if (dictionary == NULL) {
				dictionary = create_dict(key_val, (void*)new_value[new_value_i]);
			}
			else {
				dict_add_element(dictionary, key_val, (void*)new_value[new_value_i]);
			}

			++value_i;
			++new_value_i;
		}
	}

	free(new_value);
	free(value);

	return dictionary;
}
