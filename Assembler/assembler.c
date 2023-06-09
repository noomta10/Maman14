#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h" 
#include "line_info.h"
#include "utils.h" 
#include "pre_assembler.h"
#include "tables.h"
#include "first_pass.h"
#include "encoding.h"
#include "info_check.h"
#include "second_pass.h"

/* Prosseses a file by calling pre-prosesser, first and second pass. 
Return TRUE if the file was processed successfuly, or FALSE if one of the steps failed. */
void process_file(char* file_name) {
	FILE* file_pointer = NULL;
	long IC = 0;
	long DC = 0;
	symbols_table_entry* symbol_entry_head = NULL;
	data_table_entry* data_entry_head = NULL;
	entry_entry* entry_entry_head = NULL;
	extern_entry* extern_entry_head = NULL;
	code_table_entry* code_entry_head = NULL;
	uninitialized_symbols_table_entry* uninitialized_symbol_entry_head = NULL;

	/* Concatenate '.as' postfix to file name */
	char* full_file_name = add_file_postfix(file_name, ".as");
	/* Concatenate '.am' postfix to file name */
	char* full_am_name = add_file_postfix(file_name, ".am");

	/* Check if file opened successfully */
	file_pointer = fopen(full_file_name, "r");
	if (file_pointer == NULL) {
		printf("File: '%s'\n", full_file_name);
		perror("Error: Could not open file");
		free(full_file_name);
		free(full_am_name);
		return;
	}

	/* Pre_assembler, create .am file, return FALSE if falied */
	if (!pre_assembler(file_pointer, file_name)) {
		fclose(file_pointer);
		free(full_file_name);
		free(full_am_name);
		return;
	}

	/* Close file */
	fclose(file_pointer);

	/* Check if .am file opened successfully */
	file_pointer = fopen(full_am_name, "r");
	if (file_pointer == NULL) {
		printf("File: '%s'\n", full_am_name);
		perror("Error: Could not open file");
		free(full_file_name);
		free(full_am_name);
		return;
	}

	/* First_pass, return FALSE if falied */
	if (first_pass(file_name, file_pointer, &symbol_entry_head, &data_entry_head, &entry_entry_head, &extern_entry_head, &code_entry_head, &uninitialized_symbol_entry_head, &IC, &DC)) {
		second_pass(uninitialized_symbol_entry_head, symbol_entry_head, extern_entry_head, entry_entry_head, full_file_name, file_name, IC, DC, code_entry_head, data_entry_head);
	}


	/* Free memory */
	free(full_file_name);
	free(full_am_name);
	free_data_table(data_entry_head);
	free_entry_table(entry_entry_head);
	free_extern_table(extern_entry_head);
	free_symbols_table(symbol_entry_head);
	free_code_table(code_entry_head);

	/* Close file */
	fclose(file_pointer);
}


/* Main function handle all files that were given as command line arguments one by one */
int main(int argc, char* argv[]) {
	int file_number;

	/* If no file name was given, print an error message and exit */
	if(argc == 1) {
		fprintf(stderr, "Error: No files were given\n\n");
		return NO_FILES_GIVEN_ERROR;
	}

	/* Process each file by arguments */
	for (file_number = 1; file_number < argc; ++file_number) {
		process_file(argv[file_number]);
		printf("\n\n");
	}

	return NO_ERRORS;
}
