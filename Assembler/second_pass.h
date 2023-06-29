/* Add the uninitialized symbols adresses to the code image table.
* If errors were found, second pass failed, return FALSE.
Else, second pass succeeded, write the '.ob', '.ext', '.ent' files and return TRUE */
boolean second_pass(uninitialized_symbols_table_entry* head_uninitialized_symbols_entry, symbols_table_entry* head_symbols_entry, extern_entry* head_extern_entry,
	entry_entry* head_entry_entry, char* as_file_name, char* file_name, long IC, long DC, code_table_entry* code_entry_head, data_table_entry* data_entry_head);
