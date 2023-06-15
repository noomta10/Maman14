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
#include "second_pass.h"

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
	boolean error_flag = FALSE;

	long IC = IC_START_ADDRESS;
	long DC = 0;
	symbols_table_entry* symbol_table_head = NULL;
    data_table_entry* data_table_head = NULL;
    entry_entry* ent_head = NULL;
    extern_entry* ext_head = NULL;
	code_table_entry* code_table_head = NULL;
	uninitialized_symbols_table_entry* uninitialized_symbol_head = NULL;

	/* Concatenate '.as' postfix to file name */
	char* full_file_name = add_file_postfix(file_name, ".as");
	/* Concatenate '.am' postfix to file name */
	char* full_am_name = add_file_postfix(file_name, ".am");

	unsigned int test = 0;
	unsigned int test2 = -3;

	LOG_DEBUG("process_file start");


	/* Check if file opened successfully */
	file_pointer = fopen(full_file_name, "r");
	if (file_pointer == NULL) {
		printf("Error: The file %s couldn't be opened\n", full_file_name);
		free(full_file_name);
		return;
	}

	/* Call pre_assembler to create .am file */
	pre_assembler(file_pointer, file_name);

	/* Close file and free data */
	fclose(file_pointer);
	free(full_file_name);

	/* Check if .am file opened successfully */
	file_pointer = fopen(full_am_name, "r");
	if (file_pointer == NULL) {
		printf("Error: The file %s couldn't be opened\n", full_am_name);
		free(full_am_name);
		return;
	}

	printf("first_pass started\n");
	/*call first_pass*/
	error_flag = first_pass(file_pointer, &symbol_table_head, &data_table_head, &ent_head, &ext_head, &code_table_head, &uninitialized_symbol_head, &IC, &DC);


	/*printing data_tables for debugging*/
	printf("IC = %ld\n", IC);
    printf("DC = %ld\n", DC);
    printf("\n");
    print_data_table(data_table_head);
    print_symbol_table(symbol_table_head);
    print_entry_table(ent_head);
    print_extern_table(ext_head);
	print_code_word(code_table_head);
	print_uninitialized_symbols_table(uninitialized_symbol_head);

	second_pass(uninitialized_symbol_head);

	/* Free memory */
	free_data_table(data_table_head);
	free_entry_table(ent_head);
	free_extern_table(ext_head);
	free_symbols_table(symbol_table_head);
	//free_code_table(code_table_head);
	
	/* Close file */
	fclose(file_pointer);

	/* Check encoding */
	encode_base64('c');
}