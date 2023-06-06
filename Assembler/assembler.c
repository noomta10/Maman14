#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h" 
#include "utils.h" 
#include "pre_assembler.h"
#include "tables.h"
#include "debuging.h"
#include "first_pass.h"







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

	/*for Noam, do we need to set IC to 0 and start adding information from the address 100? */
	long IC = IC_START_ADDRESS;
	long DC = 0;
	symbols_table_entry* symbol_table = (symbols_table_entry*)malloc_with_check(sizeof(symbol_table));
    data_table_entry* data_table = (data_table_entry*)malloc_with_check(sizeof(data_table));
    entry_entry* ent = (entry_entry*)malloc_with_check(sizeof(ent));
    extern_entry* ext = (extern_entry*)malloc_with_check(sizeof(ext));

	/* Concatenate '.as' postfix to file name */
	char* full_file_name = add_file_postfix(file_name, ".as");
	/* Concatenate '.am' postfix to file name */
	char* full_am_name = add_file_postfix(file_name, ".am");

	/* Reset tables */
    reset_entry(ent);
    reset_extern(ext);
    reset_symbol(symbol_table);
    reset_data(data_table);

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

	/*close file and freeing data*/
	fclose(file_pointer);
	free(full_file_name);

	/* Check if .am file opened successfully */
	file_pointer = fopen(full_am_name, "r");
	if (file_pointer == NULL) {
		printf("Error: The file %s couldn't be opened\n", full_am_name);
		free(full_am_name);
		return;
	}

	/*call first_pass*/
	error_flag = first_pass(file_pointer, symbol_table, data_table, ent, ext, &IC, &DC);


	/*printing data_tables for debugging*/
	printf("IC = %ld\n", IC);
    printf("DC = %ld\n", DC);
    printf("\n");
    print_data_table(data_table);
    print_symbol_table(symbol_table);
    print_entry_table(ent);
    print_extern_table(ext);



	/* Free memory */

	/* Close file */
	fclose(file_pointer);
	
}