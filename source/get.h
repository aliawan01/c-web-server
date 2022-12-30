#pragma once

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

