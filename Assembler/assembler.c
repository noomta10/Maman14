#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debuging.h"
#include "assembler.h" 
#include "utils.h" 
#include "pre_assembler.h"





static void process_file(char* file_name);

int main(int argc, char* argv[]) {
	int i;
	
	LOG_DEBUG("starting");

	/* Process each file by arguments */
	for (i = 1; i < argc; ++i) {
		process_file(argv[i]);
	}
	return 0;
}


void process_file(char* file_name) {
	FILE* file_pointer = NULL;


	/* Concatenate '.as' postfix to file name */
	char* full_file_name = add_file_postfix(file_name, ".as");

	LOG_DEBUG("process_file start");


	/* Check if file opened successfully */
	file_pointer = fopen(full_file_name, "r");
	if (file_pointer == NULL) {
		printf("Error: The file %s couldn't be opened\n", full_file_name);
		free(full_file_name);
		return;
	}

	/* Call pre-assembler */
	pre_assembler(file_pointer, file_name);

	/* Free memory */
	free(full_file_name);
}