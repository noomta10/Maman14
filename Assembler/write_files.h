/* Write '.ent' file for entries */
void write_entry_file(char* file_name, entry_entry* entry_table_head);

/* Write '.ent' file for externs */
void write_extern_file(char* file_name, extern_entry* extern_table_head, uninitialized_symbols_table_entry* uninitialized_entry_head);

/* Write '.ob' file for final machine code */
void write_object_file(char* file_name, long IC, long DC, code_table_entry* code_entry_head, data_table_entry* data_entry_head);