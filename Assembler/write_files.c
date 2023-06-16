#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "assembler.h"
#include "utils.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"
#include "write_files.h"

/* Write entry file */
void write_entry_file(char* file_name, entry_entry* entry_table_head) {
	char* entry_file_name;
	FILE* entry_file;
	entry_entry* current_entry_entry;

	/* If there are no entries in the table, do not create entry file */
	if (!entry_table_head) 
	{
		LOG_DEBUG("No entries in file");
		return;
	}

	entry_file_name = add_file_postfix(file_name, ".ent");
	entry_file = fopen(entry_file_name, "w");

	if (!entry_file) {
		printf("Error: couldn't open file %s\n", entry_file_name);
		return;
	}

	current_entry_entry = entry_table_head;
	
	while (current_entry_entry)
	{
		fprintf(entry_file, "%s %5d\n", current_entry_entry->name, current_entry_entry->address);
		current_entry_entry = current_entry_entry->next;
	}

	/* Close file, and free file name allocated memory */
	fclose(entry_file);
	free(entry_file_name);
}


/* Write extern file */
void write_extern_file(char* file_name, extern_entry* extern_table_head) {
	char* extern_file_name;
	FILE* extern_file;
	extern_entry* current_extern_entry;

	/* If there are no entries in the table, do not create entry file */
	if (!extern_table_head)
	{
		LOG_DEBUG("No externs in file");
		return;
	}

	extern_file_name = add_file_postfix(file_name, ".ext");
	extern_file = fopen(extern_file_name, "w");

	if (!extern_file) {
		printf("Error: couldn't open file %s\n", extern_file_name);
		return;
	}

	current_extern_entry = extern_table_head;

	while (current_extern_entry)
	{
		fprintf(extern_file, "%s %5d\n", current_extern_entry->name, current_extern_entry->address);
		current_extern_entry = current_extern_entry->next;
	}

	/* Close file, and free file name allocated memory */
	fclose(extern_file);
	free(extern_file_name);
}


void write_object_file(char* file_name, long IC, long DC, code_table_entry* code_entry_head, data_table_entry* data_entry_head) {
	FILE* object_file;
	data_table_entry* current_data_entry;
	char* object_file_name = add_file_postfix(file_name, ".ob");
	object_file = fopen(object_file_name, "w");

	if (!object_file) {
		printf("Error: couldn't open file %s\n", object_file_name);
		return;
	}

	/* Print first line */
	fprintf(object_file, "%d %d\n", IC, DC);

	current_data_entry = data_entry_head;
	while (current_data_entry)
	{
		fprintf(object_file, "%c\n", current_data_entry->data);
		current_data_entry = current_data_entry->next;
	}


	fclose(object_file);
	free(object_file_name);
}