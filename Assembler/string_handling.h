void reset_str(char *string);
void skip_white_spaces(char **string);
char *copy_string(char *string);
char *copy_next_word(char *string);
char *get_opcode(char **string);
char *get_operand(char **string);
char *get_directive(char **string);
char *get_label(char **string);
boolean end_of_string(char *string);
opcode_type get_opcode_bits(char* opcode);
register_type get_register_number(char* register_name);
void remove_new_line_character(char* string);


