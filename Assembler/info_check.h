/* Checks if the line is a directive command */
boolean is_directive(char*);

/* Checks if the string is a label */
boolean is_label(char*);

/* Checks if the string is a number */
boolean is_number(char* string);

/* Checks if the line needs to be ignord (empty or starts with ;) */
boolean ignore_line(char* line);

/* Checks if the line is over 80 charaters long */
boolean line_too_long(FILE* am_file, char* line_content);

/* Checks if the label is valid */
boolean invalid_label(char* file_name, char* label, char* line_content, long line_number);

/* Checks if the next character in the string is a comma */
boolean check_comma(char** string);

/* Checks if the program needs more then PC can handle - 1024 words */
boolean program_too_big(char* file_name, long IC, long DC);

/* Checks for errors in directive line */
boolean valid_directive_line(line_info* line);

/* Checks for errors in instruction line */
boolean valid_instruction_line(line_info* line);

/* Checks for errors in .data lines */
boolean valid_data_command(line_info* line);

/* Checks for errors in .string lines */
boolean valid_string_command(line_info* line);

/* Checks for errors in .extern lines */
boolean valid_extern_command(line_info* line);

/* Checks for errors in .entry lines */
boolean valid_entry_command(line_info* line);

/* Looks for missing operands in instruction lines */
boolean check_extra_or_missing_operands(line_info* line);

/* Checks for worng operands types */
boolean check_operands(line_info* line);

/* Checks if the source operand is valid */
boolean valid_source_operand(line_info* line);

/* Checks if the target operand is valid */
boolean valid_target_operand(line_info* line);

/* Checks if the operand is a register */
boolean is_register(char* operand);

/* Checks if the symbol is defined in the symbol table alrady */
boolean exists_in_symbol_table(char* symbol, symbols_table_entry* symbol_table);

/* Checks if the symbol is defined in the extern table alrady */
boolean exists_in_extern_table(char* symbol, extern_entry* external_table);

/* Checks if the symbol is defined in the entry table alrady */
boolean exists_in_entry_table(char* symbol, entry_entry* entry_table);

/* Checks if the number in .data lines is bigger then 2047 */
boolean data_number_too_big(char* string_number);

/* Checks if numbers that are operands are bigger then 511 */
boolean instruction_number_too_big(char* string_number);

/* Checks if the number in .data lines is smaller then -2048 */
boolean data_number_too_small(char* string_number);

/* Checks if numbers that are operands are smaller then -512 */
boolean instruction_number_too_small(char* string_number);

/* Checks if the string is empty */
boolean string_is_empty(char* string);

/* Checks if there's a extra comma at the end of the line */
boolean extra_comma(char* line);
