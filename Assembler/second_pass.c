#include <stdio.h>
#include <string.h>

#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"
#include "write_files.h"

void add_entry_table_address(entry_entry* head_entry_entry, symbols_table_entry* head_symbol_table, long IC) {
	entry_entry* current_entry_entry = head_entry_entry;
	symbols_table_entry* current_symbol_entry;
	
	/* As long as there is an entry entry in the table, add it's address from symbols table */
	while (current_entry_entry) {
		current_symbol_entry = head_symbol_table;

		/* Loop over the symbols table and find entry entry address */
		while (current_symbol_entry) {
			if (strcmp(current_entry_entry->name, current_symbol_entry->name) == 0) {
				if (current_symbol_entry->address_type == DIRECTIVE) {
					current_entry_entry->address = current_symbol_entry->address + 100 + IC;
				}
				else {
					current_entry_entry->address = current_symbol_entry->address + 100;
				}
				break;
			}
			current_symbol_entry = current_symbol_entry->next;
		}
		current_entry_entry = current_entry_entry->next;
	}

}


void second_pass(uninitialized_symbols_table_entry* head_uninitialized_symbols_entry, symbols_table_entry* head_symbols_entry, extern_entry* head_extern_entry,
	entry_entry* head_entry_entry,char* file_name, long IC, long DC, code_table_entry* code_entry_head, data_table_entry* data_entry_head) {
	uninitialized_symbols_table_entry* current_uninitialized_symbols_entry = head_uninitialized_symbols_entry;
	symbols_table_entry* current_symbol_entry;

	add_entry_table_address(head_entry_entry, head_symbols_entry, IC);

	/* As long as there are uninitialized symbols in the table, add them to the code image */
	while (current_uninitialized_symbols_entry) {
		/* Reset symbols table */
		current_symbol_entry = head_symbols_entry;

		/* Find current uninitialized symbol address in symbols table */
		while (current_symbol_entry)
		{
			/* Check if it is an external symbol and set ARE accordingly */
			if (is_external(current_uninitialized_symbols_entry->name, head_extern_entry)) {
				current_uninitialized_symbols_entry->extra_code_word_value->ARE = EXTERNAL;
			}
			else {
				current_uninitialized_symbols_entry->extra_code_word_value->ARE = RELOCATABLE;
			}

			if (strcmp(current_uninitialized_symbols_entry->name, current_symbol_entry->name) == 0) {
				/* Check if it is a directive or instruction symbol and set data accordingly */
				if (current_symbol_entry->address_type == DIRECTIVE) {
					current_uninitialized_symbols_entry->extra_code_word_value->data = current_symbol_entry->address + 100 + IC;
				}
				else {
					current_uninitialized_symbols_entry->extra_code_word_value->data = current_symbol_entry->address + 100;
				}
				break;
			}

			current_symbol_entry = current_symbol_entry->next;
		}

		current_uninitialized_symbols_entry = current_uninitialized_symbols_entry->next;
	}

	/* Write files */
	write_extern_file(file_name, head_extern_entry);
	write_entry_file(file_name, head_entry_entry);
	write_object_file(file_name, IC, DC, code_entry_head, data_entry_head);

}


/* Check if a symbol is in the externals table */
boolean is_external(char* current_symbol, extern_entry* head_extern_entry) {
	extern_entry* current_extern_entry = head_extern_entry;

	while (current_extern_entry) {
		if (strcmp(current_symbol, current_extern_entry->name) == 0) {
			return TRUE;
		}
		current_extern_entry = current_extern_entry->next;
	}

	return FALSE;
}


