#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "write_files.h"

/* Add entries addresses from the symbols table to the entries table */
static void add_entry_table_address(entry_entry* head_entry_entry, symbols_table_entry* head_symbol_table, long IC) {
	entry_entry* current_entry_entry = head_entry_entry;
	symbols_table_entry* current_symbol_entry;
	
	/* As long as there is an entry entry in the table, add it's address from symbols table */
	while (current_entry_entry) {
		current_symbol_entry = head_symbol_table;

		/* Loop over the symbols table and find entry entry address */
		while (current_symbol_entry) {
			/* If entry name match symbol name */
			if (strcmp(current_entry_entry->name, current_symbol_entry->name) == 0) {
				current_entry_entry->address = current_symbol_entry->address;
				break;
			}
			current_symbol_entry = current_symbol_entry->next;
		}
		current_entry_entry = current_entry_entry->next;
	}
}


/* Check if a symbol is in the externals table */
static boolean is_external(char* current_symbol, extern_entry* head_extern_entry) {
	extern_entry* current_extern_entry = head_extern_entry;
	
	/* Loop as long as there is an extern entry in the table */
	while (current_extern_entry) {
		if (strcmp(current_symbol, current_extern_entry->name) == 0) {
			return TRUE;
		}
		current_extern_entry = current_extern_entry->next;
	}
	return FALSE;
}


/* Check if all entries are defined in the current file */
static boolean check_entry_defined_in_file(entry_entry* head_entry_entry, symbols_table_entry* head_symbols_entry, char* as_file_name) {
	symbols_table_entry* current_symbols_entry;
	entry_entry* current_entry_entry = head_entry_entry;
	boolean entry_has_definition;

	/* Loop as long as there is an entry entry in the table */
	while (current_entry_entry) {
		current_symbols_entry = head_symbols_entry;
		entry_has_definition = FALSE;

		/* Loops as long as there is a symbol entry in the table */
		while (current_symbols_entry) {
			if (strcmp(current_entry_entry->name, current_symbols_entry->name) == 0) {
				entry_has_definition = TRUE;
				break;
			}
			current_symbols_entry = current_symbols_entry->next;
		}

		/* If an entry does not have a definition in the file, print an error message and return FALSE */
		if (!entry_has_definition) {
			PRINT_ERROR(as_file_name, current_entry_entry->line_number, current_entry_entry->line_content, "Entry symbol is not defined in file.");
			return FALSE;
		}
		current_entry_entry = current_entry_entry->next;
	}
	return TRUE;
}


/* Check if all symbols that were used as operands are defined */
static boolean check_defined_symbol_operand(symbols_table_entry* head_symbols_entry, extern_entry* head_extern_entry, uninitialized_symbols_table_entry* head_uninitialized_entry) {
	uninitialized_symbols_table_entry* current_uninitialized_entry = head_uninitialized_entry;
	symbols_table_entry* current_symbols_entry;
	extern_entry* current_extern_entry;
	boolean symbol_operand_defined;
	
	/* Loop as long as there is an uninitialized symbol in the table */
	while (current_uninitialized_entry) {
		/* Reset fields */
		current_symbols_entry = head_symbols_entry;
		current_extern_entry = head_extern_entry;
		symbol_operand_defined = FALSE;

		/* Check if operand is an extern symbol */
		while (current_extern_entry) {
			if (strcmp(current_extern_entry->name, current_uninitialized_entry->name) == 0) {
				symbol_operand_defined = TRUE;
				break;
			}
			current_extern_entry = current_extern_entry->next;
		}

		/* Check if operand is defined in current file */
		while (current_symbols_entry) {
			if (strcmp(current_symbols_entry->name, current_uninitialized_entry->name) == 0) {
				symbol_operand_defined = TRUE;
				break;
			}
			current_symbols_entry = current_symbols_entry->next;
		}

		/* If the symbol is not defined as an entry, extern or in the current file, print an error message and return FALSE */
		if (!symbol_operand_defined) {
			printf("Error: operand symbol %s is not a known symbol.\n", current_uninitialized_entry->name);
			return FALSE;
		}
		current_uninitialized_entry = current_uninitialized_entry->next;
	}
	return TRUE;
}


/* Add the uninitialized symbols adresses to the code image table.
If errors were found, second pass failed, return FALSE.
Else, second pass succeeded, write the '.ob', '.ext', '.ent' files and return TRUE */
boolean second_pass(uninitialized_symbols_table_entry* head_uninitialized_symbols_entry, symbols_table_entry* head_symbols_entry, extern_entry* head_extern_entry,
	entry_entry* head_entry_entry,char* as_file_name, char* file_name, long IC, long DC, code_table_entry* code_entry_head, data_table_entry* data_entry_head) {
	uninitialized_symbols_table_entry* current_uninitialized_symbols_entry = head_uninitialized_symbols_entry;
	symbols_table_entry* current_symbol_entry;
	
	/* If an entry is not defined in the current file, print an error message and return FALSE */
	if (!check_entry_defined_in_file(head_entry_entry, head_symbols_entry, as_file_name)) {
		return FALSE;
	}

	/* If a symbol is not defined as an entry, extern or in the current file, print an error message and return FALSE */
	if (!check_defined_symbol_operand(head_symbols_entry, head_extern_entry, head_uninitialized_symbols_entry)) {
		return FALSE;
	}

	add_entry_table_address(head_entry_entry, head_symbols_entry, IC);

	/* As long as there are uninitialized symbols in the table, add them to the code image */
	while (current_uninitialized_symbols_entry) {
		/* Reset symbols table */
		current_symbol_entry = head_symbols_entry;

		/* Find current uninitialized symbol address in symbols table */
		while (current_symbol_entry) {
			/* Check if it is an external symbol and set ARE accordingly */
			if (is_external(current_uninitialized_symbols_entry->name, head_extern_entry)) {
				current_uninitialized_symbols_entry->extra_code_word_value->ARE = EXTERNAL;
			}
			else {
				current_uninitialized_symbols_entry->extra_code_word_value->ARE = RELOCATABLE;
			}

			if (strcmp(current_uninitialized_symbols_entry->name, current_symbol_entry->name) == 0) {
				/* Set symbol data */
				current_uninitialized_symbols_entry->extra_code_word_value->data = current_symbol_entry->address;
				break;
			}
			current_symbol_entry = current_symbol_entry->next;
		}
		current_uninitialized_symbols_entry = current_uninitialized_symbols_entry->next;
	}

	set_code_table_to_ic_initial_address(&code_entry_head);
	
	/* Write files */
	write_extern_file(file_name, head_extern_entry, head_uninitialized_symbols_entry);
	write_entry_file(file_name, head_entry_entry);
	write_object_file(file_name, IC, DC, code_entry_head, data_entry_head);
	
	return TRUE;
}
