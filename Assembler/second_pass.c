#include <stdio.h>
#include <string.h>

#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"
#include "write_files.h"

void second_pass(uninitialized_symbols_table_entry* head_uninitialized_symbols_entry, symbols_table_entry* head_symbols_entry, extern_entry* head_extern_entry, entry_entry* head_entry_entry, char* file_name) {
	uninitialized_symbols_table_entry* current_uninitialized_symbols_entry = head_uninitialized_symbols_entry;
	symbols_table_entry* current_symbol_entry;

	/* As long as there are uninitialized_symbols in the table, add them to the code image */
	while (current_uninitialized_symbols_entry) {
		/* Reset symbols table */
		current_symbol_entry = head_symbols_entry;

		/* Find current uninitialized symbol address in symbols table */
		while (current_symbol_entry)
		{
			if (strcmp(current_uninitialized_symbols_entry->name, current_symbol_entry->name) == 0) 
			{
				current_uninitialized_symbols_entry->extra_code_word_value->data = current_symbol_entry->address;
				current_uninitialized_symbols_entry->extra_code_word_value->ARE = RELOCATABLE;
				break;
			}

			current_symbol_entry = current_symbol_entry->next;
		}

		current_uninitialized_symbols_entry = current_uninitialized_symbols_entry->next;
	}

	/* Write files */
	write_extern_file(file_name, head_extern_entry);
	write_entry_file(file_name, head_entry_entry);
}


