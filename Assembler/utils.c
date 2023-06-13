#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "utils.h"
#include "line_info.h"
#include "tables.h"

char* add_file_postfix(char* file_name, char* postfix) {
	char* full_file_name = (char*)malloc_with_check(strlen(file_name) + strlen(postfix) + 1);
	strcpy(full_file_name, file_name);
	strcat(full_file_name, postfix);
	return full_file_name;
}


void* malloc_with_check(size_t length) {
	void* pointer = malloc(length);
	if (pointer == NULL) {
		printf("Error: memory allocation failed\n");
		exit(-1);
	}
	return pointer;
}


void* realloc_with_check(char* ptr, size_t length) {
	void* pointer = realloc(ptr, length);

	printf("debug realloc before %p\n", ptr);

	if (pointer == NULL) {
		printf("Error: memory reallocation failed\n");
		exit(-1);
	}
	return pointer;
}


boolean is_register(char* operand) {
	if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0')
		return TRUE;
	return FALSE;
}

