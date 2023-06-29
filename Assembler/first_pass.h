/* Read the '.am' file line by line, extract and process it */
boolean first_pass(char* file_name, FILE *am_file, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head,
                   entry_entry** ent_head, extern_entry** ext_head, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbols_table_entry, long* IC, long* DC);

/* Gets an operand and return its addressing type: immediate, register or direct */
addressing_type get_addressing_type(char* operand);
