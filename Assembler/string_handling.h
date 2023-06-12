#pragma once



void reset_str(char *string);
void skip_white_spaces(char **string);

char *copy_string(char *string);
char *copy_next_word(char *string);
char *get_opcode(char **string);
char *get_operand(char **string);
char *get_directive(char **string);
char *get_label(char **string);

boolean end_of_string(char *string);
boolean ignore_line(char* line);

