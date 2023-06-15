#include <stdio.h>
#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"

void second_pass(uninitialized_symbols_table_entry* head_uninitialized_symbols_entry) {
	char* current_symbol_name;
	uninitialized_symbols_table_entry* current_uninitialized_symbols_entry = head_uninitialized_symbols_entry;
	
	/* As long as there are uninitialized_symbols in the table, add them to the code image */
	while (current_uninitialized_symbols_entry) {
		current_symbol_name = current_uninitialized_symbols_entry->name;
		LOG_DEBUG(current_symbol_name);
		current_uninitialized_symbols_entry = current_uninitialized_symbols_entry->next;
	}


}