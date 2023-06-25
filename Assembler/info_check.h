boolean is_directive(char*);
boolean is_label(char*);
boolean is_number(char* string);
boolean ignore_line(char* line);
boolean line_too_long(FILE* am_file, char* line_content);
boolean bad_label(char* label, char* line_content, long line_number);
boolean check_comma(char** string);
boolean program_too_big(long IC, long DC);
boolean valid_directive_line(line_info* line);
boolean valid_instruction_line(line_info* line);
boolean valid_data_command(line_info* line);
boolean valid_string_command(line_info* line);
boolean valid_extern_command(line_info* line);
boolean valid_entry_command(line_info* line);
boolean check_extra_or_missing_operands(line_info* line);
boolean check_operands(line_info* line);
boolean valid_source_operand(line_info* line);
boolean valid_target_operand(line_info* line);
boolean is_register(char* operand);
boolean exists_in_symbol_table(char* symbol, symbols_table_entry* symbol_table);
boolean exists_in_extern_table(char* symbol, extern_entry* external_table);
boolean exists_in_entry_table(char* symbol, entry_entry* entry_table);
boolean number_too_big(char* string_number);
boolean string_is_empty(char* string);
