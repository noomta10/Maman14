#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "assembler.h"

char* add_file_postfix(char* file_name, char* postfix) {
	char* full_file_name = (char*)malloc_with_check(strlen(file_name) + strlen(postfix) + 1);
	strcpy(full_file_name, file_name);
	strcat(full_file_name, postfix);
	return full_file_name;
}


void* malloc_with_check(size_t length) {
	void* pointer = malloc(length);
	printf ("debug malloc %p\n",pointer);
	if (pointer == NULL) {
		printf("Error: memory allocation failed\n");
		exit(-1);
	}
	return pointer;
}


void* realloc_with_check(char* ptr, size_t length) {
	printf("debug realloc before %p\n", ptr);

	void* pointer = realloc(ptr, length);
	printf("debug realloc after %p\n", pointer);
	if (pointer == NULL) {
		printf("Error: memory reallocation failed\n");
		exit(-1);
	}
	return pointer;
}


char* get_next_element(char* line) {
	int start_word_index;
	int end_word_index;
	int word_length;
	int i = 0;
	char element[MAX_LINE_LENGTH];
	char* word = NULL;

	/* Skip spaces and tabs */
	while (line[i] == ' ' || line[i] == '\t') {
		i++;
	}

	start_word_index = i;

	/* Increase the index as long as no tab or space was found */
	while (line[i] != ' ' && line[i] != '\t') {
		i++;
	}

	end_word_index = i - 1;
	word_length = end_word_index - start_word_index + 1;
	word = malloc_with_check(word_length + 1);
	strncpy(word, line + start_word_index, word_length);
	word[word_length] = '\0';

	return word;
}