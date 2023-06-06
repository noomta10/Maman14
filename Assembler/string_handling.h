#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


void reset_str(char *string);
void skip_white_spaces(char **string);

char *get_word(char **string);
char *get_opcode(char **string);
char *get_operand(char **string);
char *get_instruction(char **string);
char *get_label(char **string);

boolean end_of_string(char *string);
boolean ignore_line(char* line);

