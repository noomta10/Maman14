#define _CRT_SECURE_NO_WARNINGS

boolean first_pass(char* file_name, FILE *am_file, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head,
                   entry_entry** ent_head, extern_entry** ext_head, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbols_table_entry, long* IC, long* DC);
void extract_command_info(char *line_content, line_info *line);
boolean validate_line(line_info *line);
boolean process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry** symbol_table_head, data_table_entry** data_table,
    entry_entry** ent, extern_entry** ext, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbol_head);
addressing_type get_addressing_type(char* operand);


