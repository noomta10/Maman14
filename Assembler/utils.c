#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "utils.h"
#include "line_info.h"
#include "tables.h"


/* Adds postfix to the file name */
char* add_file_postfix(char* file_name, char* postfix) {
	char* full_file_name = (char*)malloc_with_check(strlen(file_name) + strlen(postfix) + 1);
	strcpy(full_file_name, file_name);
	strcat(full_file_name, postfix);
	return full_file_name;
}


/* Allocates memory and checks for failure */
void* malloc_with_check(size_t length) {
	void* pointer = malloc(length);

	/* If allocation failed, print an error and exit */
	if (pointer == NULL) {
		fprintf(stderr, "Error: memory allocation failed\n");
		exit(MEMORY_ALLOCATION_ERROR);
	}
	return pointer;
}


/* Reallocs memory and checks for failer */
void* realloc_with_check(char* ptr, size_t length) {
	void* pointer = realloc(ptr, length);

	/* If reallocation failed, print an error and exit */
	if (pointer == NULL) {
		fprintf(stderr, "Error: memory reallocation failed\n");
		exit(MEMORY_REALLOCATION_ERROR);
	}
	return pointer;
}


/* Checks if the name if a reseved word */
boolean is_reserved_name(char* name) {
	if (strcmp(name, "mov") == 0 || strcmp(name, "cmp") == 0 ||
		strcmp(name, "add") == 0 || strcmp(name, "sub") == 0 ||
		strcmp(name, "not") == 0 || strcmp(name, "clr") == 0 ||
		strcmp(name, "lea") == 0 || strcmp(name, "inc") == 0 ||
		strcmp(name, "dec") == 0 || strcmp(name, "jmp") == 0 ||
		strcmp(name, "bne") == 0 || strcmp(name, "red") == 0 ||
		strcmp(name, "prn") == 0 || strcmp(name, "jsr") == 0 ||
		strcmp(name, "rts") == 0 || strcmp(name, "stop") == 0 ||
		strcmp(name, "string") == 0 || strcmp(name, "data") == 0 ||
		strcmp(name, "entry") == 0 || strcmp(name, "extern") == 0 ||
		strcmp(name, "@r0") == 0 || strcmp(name, "@r1") == 0 ||
		strcmp(name, "@r2") == 0 || strcmp(name, "@r3") == 0 ||
		strcmp(name, "@r4") == 0 || strcmp(name, "@r5") == 0 ||
		strcmp(name, "@r6") == 0 || strcmp(name, "@r7") == 0)
	{
		return TRUE;
	}
	return FALSE;
}
