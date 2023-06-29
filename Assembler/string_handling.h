/* Clears the string */
void reset_str(char *string);

/* Skips whith spaces in the string */
void skip_white_spaces(char **string);

/* Returns a copy of the string */
char *copy_string(char *string);

/* Returns a copy of the next word in the string */
char *copy_next_word(char *string);

/* Returns a copy of the opcode in the string */
char *get_opcode(char **string);

/* Returns a copy of the operand in the string */
char *get_operand(char **string);

/* Returns a copy of the directive commad in the string */
char *get_directive(char **string);

/* Returns a copy of the label in the string */
char *get_label(char **string);

/* Returns TRUE if the string is empty */
boolean end_of_string(char *string);

/* Returns the opcode code */
opcode_type get_opcode_bits(char* opcode);

/* Returns the register number */
register_type get_register_number(char* register_name);

/* Deletes the new line character in the end of the string */
void remove_new_line_character(char* string);
