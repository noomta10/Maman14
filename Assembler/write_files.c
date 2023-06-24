#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "utils.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"
#include "write_files.h"
#include "encoding.h"


/* Write entry file */
void write_entry_file(char* file_name, entry_entry* entry_table_head) {
	char* entry_file_name;
	FILE* entry_file;
	entry_entry* current_entry_entry;

	/* If there are no entries in the table, do not create entry file */
	if (!entry_table_head) {
		LOG_DEBUG("No entries in file");
		return;
	}

	entry_file_name = add_file_postfix(file_name, ".ent");
	entry_file = fopen(entry_file_name, "w");

	if (!entry_file) {
		fprintf(stderr, "Error: couldn't open file %s\n", entry_file_name);
		return;
	}

	current_entry_entry = entry_table_head;
	
	while (current_entry_entry) {
		fprintf(entry_file, "%s %5d\n", current_entry_entry->name, current_entry_entry->address);
		current_entry_entry = current_entry_entry->next;
	}

	/* Close file, and free file name allocated memory */
	fclose(entry_file);
	free(entry_file_name);
}


/* Write extern file */
void write_extern_file(char* file_name, extern_entry* extern_table_head, uninitialized_symbols_table_entry* uninitialized_entry_head) {
	char* extern_file_name;
	FILE* extern_file;
	extern_entry* current_extern_entry;
	uninitialized_symbols_table_entry* current_uninitialized_entry = uninitialized_entry_head;

	/* If there are no entries in the table, do not create entry file */
	if (!extern_table_head) {
		LOG_DEBUG("No externs in file");
		return;
	}

	extern_file_name = add_file_postfix(file_name, ".ext");
	extern_file = fopen(extern_file_name, "w");

	if (!extern_file) {
		fprintf(stderr, "Error: couldn't open file %s\n", extern_file_name);
		return;
	}

	current_extern_entry = extern_table_head;

	while (current_extern_entry) {
		current_uninitialized_entry = uninitialized_entry_head;

		while (current_uninitialized_entry) {
			if (strcmp(current_extern_entry->name, current_uninitialized_entry->name) == 0) {
				fprintf(extern_file, "%s %5d\n", current_extern_entry->name, current_uninitialized_entry->address);
				current_uninitialized_entry = current_uninitialized_entry->next;
				continue;
			}
			current_uninitialized_entry = current_uninitialized_entry->next;
		}
		current_extern_entry = current_extern_entry->next;
	}

	/* Close file, and free file name allocated memory */
	fclose(extern_file);
	free(extern_file_name);
}


/* Write object file */
void write_object_file(char* file_name, long IC, long DC, code_table_entry* code_entry_head, data_table_entry* data_entry_head) {
	FILE* object_file;
	data_table_entry* current_data_entry;
	code_table_entry* current_code_entry;
	unsigned int final_bits_word;

	char* object_file_name = add_file_postfix(file_name, ".ob");
	object_file = fopen(object_file_name, "w");

	if (!object_file) {
		fprintf(stderr, "Error: couldn't open file %s\n", object_file_name);
		return;
	}

	/* Print first line */
	fprintf(object_file, "%d %d\n", IC, DC);
	current_code_entry = code_entry_head;

	/* Print all encoded code words to the object file */
	while (current_code_entry) {
		final_bits_word = get_bits_word(current_code_entry);
		fprintf(object_file, "%s\n", encode_base64(final_bits_word));
		current_code_entry = current_code_entry->next;
	}

	current_data_entry = data_entry_head;

	/* Print all encoded data words to the object file */
	while (current_data_entry) {
		fprintf(object_file, "%s\n", encode_base64(current_data_entry->data.character));
		current_data_entry = current_data_entry->next;
	}

	/* Close file and free memory */
	fclose(object_file);
	free(object_file_name);
}


/* Convert the bits in the code_word_entry structure to a number */
static unsigned int get_bits_word(code_table_entry* current_code_entry){
	unsigned int final_bits_word = 0;
	if (current_code_entry->type == TYPE_CODE_WORD) {
		final_bits_word = final_bits_word | current_code_entry->value.code_word_value.ARE;
		final_bits_word = final_bits_word | current_code_entry->value.code_word_value.target_addressing << 2;
		final_bits_word = final_bits_word | current_code_entry->value.code_word_value.opcode << 5;
		final_bits_word = final_bits_word | current_code_entry->value.code_word_value.source_addressing << 9;
	}
	else if (current_code_entry->type == TYPE_EXTRA_CODE_WORD) {
		final_bits_word = final_bits_word | current_code_entry->value.extra_code_word_value.ARE;
		final_bits_word = final_bits_word | current_code_entry->value.extra_code_word_value.data << 2;
	}
	else if (current_code_entry->type == TYPE_REGISTER_WORD) {
		final_bits_word = final_bits_word | current_code_entry->value.register_word_value.ARE;
		final_bits_word = final_bits_word | current_code_entry->value.register_word_value.target_register << 2;
		final_bits_word = final_bits_word | current_code_entry->value.register_word_value.source_register << 7;
	}

	return final_bits_word;
}