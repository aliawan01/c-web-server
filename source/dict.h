#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dict_struct {
	char* key;
	void* value;
	struct dict_struct* next;
} dict;


dict* alloc_dict() {
	return malloc(sizeof(struct dict_struct));
}


dict* create_dict(char* key, void* value) {
	dict* new_dict = alloc_dict();
	new_dict->key = (char*)malloc(strlen(key) + 1);
	strcpy(new_dict->key, key);
	new_dict->value = value;
	return new_dict;
}

int dict_delete_element(dict* dictionary, char* key) {
	if (dictionary->next == NULL) {
		printf("DEL_ELEMENT: Only one key-value pair in dict, therefore not pairs can be deleted.\n");
		return -1;
	}

	struct dict_struct* cur_dict = dictionary;
	struct dict_struct* prev_dict = NULL;
	for (; cur_dict != NULL; prev_dict = cur_dict, cur_dict = cur_dict->next) {
		if (strcmp(cur_dict->key, key) == 0) {
			printf("DICT_DEL_ELEMENT: Successfully deleted key-value pair with key: '%s'\n", cur_dict->key);
			if (prev_dict == NULL) {
				dictionary = cur_dict->next;
			}
			else if (cur_dict->next == NULL) {
				prev_dict->next = NULL;
			} 
			else {
				prev_dict->next = cur_dict->next;
			}

			free(cur_dict->key);
			free(cur_dict);
			cur_dict = NULL;
			return 0;
		}
	}

	return -1;
}

void dict_add_element(dict* dictionary, char* key, void* value) {
	struct dict_struct* cur_dict = dictionary;

	if ((cur_dict->next == NULL) && (strcmp(cur_dict->key, key) == 0)) {
		printf("DICT_ADD_ELEMENT: key: '%s' already exists in the dictionary and it cannot be deleted.\n", cur_dict->key);
		return;
	}
	else if (cur_dict->next != NULL) {
		dict_delete_element(dictionary, key);
	}

	// making cur_dict point to the end of the dictionary
	for (; cur_dict->next != NULL; cur_dict = cur_dict->next) {}
	cur_dict->next = alloc_dict();
	cur_dict->next->key = (char*)malloc(strlen(key) + 1);
	strcpy(cur_dict->next->key, key);
	cur_dict->next->value = value;
}

void dict_print_dict(dict* dictionary, char* print_type) {
	struct dict_struct* cur_dict = dictionary;

	printf("RUNNING PRINT_DICT...\n");
	for (int index = 0; cur_dict != NULL; index++, cur_dict = cur_dict->next) {
		printf("Index: %d\n", index);
		printf("  Key: %s\n", cur_dict->key);
		if (strcmp(print_type, "char*") == 0) {
			printf("  Value: %s\n", (char*)cur_dict->value);
		}		
		else if (strcmp(print_type, "int") == 0) {
			printf("  Value: %d\n", (int)cur_dict->value);
		}
		else {
			printf("  Value: %p\n", cur_dict->value);
		}
	}
}

int dict_get_num_of_keys(dict* dictionary) {
	dict* cur_dict = dictionary;
	int length = 1;
	for (; cur_dict->next != NULL; cur_dict = cur_dict->next, length++);
	return length;
}

char** dict_get_keys(dict* dictionary) {
	int length = dict_get_num_of_keys(dictionary);

	char** result = malloc(sizeof(char*)*length);
	dict* cur_dict = dictionary;
	for (int i = 0; i < length; ++i, cur_dict = cur_dict->next) {
		result[i] = malloc(strlen(cur_dict->key)*sizeof(char));
		strcpy(result[i], cur_dict->key);
	}

	return result;
}

void* dict_get_value(dict* dictionary, char* key) {
	dict* cur_dict = dictionary;
	for (; cur_dict != NULL; cur_dict = cur_dict->next) {
		if (strcmp(cur_dict->key, key) == 0) {
			return cur_dict->value;
		}
	}

	return NULL;
}

void dict_update_value(dict* dictionary, char* key, void* value) {
	dict* cur_dict = dictionary;
	for (; cur_dict != NULL; cur_dict = cur_dict->next) {
		if (strcmp(cur_dict->key, key) == 0) {
			cur_dict->value = value;
			return;
		}
	}

	printf("WARNING UPDATE VALUE: Could not find any key called: '%s' so we can't update the value.\n", key);
}


void dict_delete_dict(dict* dictionary) {
	printf("\nDELETING ENTIRE DICTIONARY...\n");

	char** dict_keys = dict_get_keys(dictionary);
	int num_of_keys = dict_get_num_of_keys(dictionary) - 1;

	for (; num_of_keys > 0; num_of_keys--) {
		dict_delete_element(dictionary, dict_keys[num_of_keys]);
	}

	free(dictionary->key);
	free(dictionary);

	dictionary = NULL;
}
