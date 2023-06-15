#include <stdio.h>
#include <string.h>

#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"

void second_pass(uninitialized_symbols_table_entry* head_uninitialized_symbols_entry, symbols_table_entry* head_symbols_entry) {
	uninitialized_symbols_table_entry* current_uninitialized_symbols_entry = head_uninitialized_symbols_entry;
	symbols_table_entry* current_symbol_entry;

	///* As long as there are uninitialized_symbols in the table, add them to the code image */
	while (current_uninitialized_symbols_entry) {
		current_symbol_entry = head_symbols_entry;

		/* Find current uninitialized symbol address in symbols table */
		while (current_symbol_entry)
		{
			LOG_DEBUG(current_symbol_entry->name);
			current_symbol_entry = current_symbol_entry->next;
		}

		LOG_DEBUG(current_uninitialized_symbols_entry->name);
		current_uninitialized_symbols_entry = current_uninitialized_symbols_entry->next;
	}
}