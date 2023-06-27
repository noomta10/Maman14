#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h" 
#include "line_info.h"
#include "utils.h" 
#include "pre_assembler.h"
#include "tables.h"
#include "debuging.h"
#include "first_pass.h"
#include "encoding.h"
#include "info_check.h"
#include "second_pass.h"


boolean process_file(char* file_name) {
	FILE* file_pointer = NULL;
	boolean error_flag = FALSE;
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
		fprintf(stderr, "Error: The file %s couldn't be opened\n", full_file_name);
		free(full_file_name);

		LOG_DEBUG("Openning file failed");
		return FALSE;
	}

	/* Pre_assembler, create .am file */
	if (!pre_assembler(file_pointer, file_name)) {
		LOG_DEBUG("Pre assembler failed");
		return FALSE;
	}

	/* Close file and free data */
	fclose(file_pointer);

	/* Check if .am file opened successfully */
	file_pointer = fopen(full_am_name, "r");
	if (file_pointer == NULL) {
		fprintf(stderr, "Error: The file %s couldn't be opened\n", full_am_name);
		free(full_am_name);

		LOG_DEBUG("Openning file failed");
		return FALSE;
	}

	/* First_pass */
	error_flag = first_pass(file_name, file_pointer, &symbol_entry_head, &data_entry_head, &entry_entry_head, &extern_entry_head, &code_entry_head, &uninitialized_symbol_entry_head, &IC, &DC);
	if (error_flag) {
		free_data_table(data_entry_head);
		free_entry_table(entry_entry_head);
		free_extern_table(extern_entry_head);
		free_symbols_table(symbol_entry_head);
		fclose(file_pointer);

		LOG_DEBUG("First pass failed");
		return FALSE;
	}

	/* DEBUG- printing data_tables */
	printf("IC = %ld\n", IC);
	printf("DC = %ld\n\n", DC);
	if(data_entry_head) print_data_table(data_entry_head);
	if(symbol_entry_head) print_symbol_table(symbol_entry_head);
	if(entry_entry_head) print_entry_table(entry_entry_head);
	if (extern_entry_head) print_extern_table(extern_entry_head);
	if (code_entry_head) print_code_word(code_entry_head);
	if (uninitialized_symbol_entry_head) print_uninitialized_symbols_table(uninitialized_symbol_entry_head);

	/* Second pass, exit if errors were found */
	if (!second_pass(uninitialized_symbol_entry_head, symbol_entry_head, extern_entry_head, entry_entry_head, full_file_name, file_name, IC, DC, code_entry_head, data_entry_head)) {
		free_data_table(data_entry_head);
		free_entry_table(entry_entry_head);
		free_extern_table(extern_entry_head);
		free_symbols_table(symbol_entry_head);
		fclose(file_pointer);

		LOG_DEBUG("Second pass failed");
		return FALSE;
	}

	/* DEBUG */
	LOG_DEBUG("After second pass:\n");
	print_uninitialized_symbols_table(uninitialized_symbol_entry_head);
	print_code_table_in_binary(code_entry_head);
	printf("IC: %ld\n", IC);
	printf("DC: %ld\n", DC);

	/* Free memory */
	free(full_file_name);
	free_data_table(data_entry_head);
	free_entry_table(entry_entry_head);
	free_extern_table(extern_entry_head);
	free_symbols_table(symbol_entry_head);
	free_code_table(code_entry_head);

	/* Close file */
	fclose(file_pointer);

	LOG_DEBUG("File was processed successfully");
	return TRUE;
}


int main(int argc, char* argv[]) {
	int file_index;

	/* Process each file by arguments */
	for (file_index = 1; file_index < argc; ++file_index) {
		printf("\n\n\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~ PROCESSING FILE \"%s.as\" ~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", argv[file_index]);
		process_file(argv[file_index]);
	}

	return 0;
}
