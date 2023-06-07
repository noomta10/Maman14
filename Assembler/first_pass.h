#define _CRT_SECURE_NO_WARNINGS









boolean is_label(char *);
boolean is_instruction(char *);
boolean check_comma(char **string);
boolean first_pass(FILE *am_file, symbols_table_entry **symbol_table, data_table_entry **data_table,
        entry_entry **ent, extern_entry **ext, long *IC, long *DC);
void extract_command_info(char *line_content, line_info *line);
boolean validate_line(line_info *line);
void process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry** symbol_table, data_table_entry** data_table, entry_entry** ent, extern_entry** ext, boolean* error_in_code);
boolean bad_label(char *label);
boolean check_instruction(char *instruction);


