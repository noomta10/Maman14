#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 80
#define IC_START_ADDRESS 100 
#define MAX_LABEL_LENGTH 31



typedef struct {
	long line_number;//unused
	char *file_name;//unused
	char *content;//unused

    char *label;
    char *instruction;
    char *opcode;
    char *source_operand;
    char *target_operand;
    char *instruction_data;
	boolean comma;
    boolean is_label;
    boolean is_data;
    boolean is_instruction;
     
} line_info;




void reset_line_info(line_info *line);
void reset_str(char *string);


boolean is_label(char *);
boolean is_instruction(char *);
boolean check_comma(char **string);
char *get_operand(char **string);
char *get_opcode(char **string);
char *get_instruction(char **string);
char *get_label(char **string);
char *get_word(char **string);
boolean first_pass(char *am_file_name, symbols_table_entry *symbol_table, data_table_entry *data_table,
entry_entry *ent, extern_entry *ext, long *IC, long *DC);
boolean extract_command_info(char *line_content, line_info *line);
boolean validate_line(line_info *line);
void process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry* symbol_table, data_table_entry* data_table, entry_entry* ent, extern_entry* ext, boolean* error_in_code);
boolean ignore_line(char* line);
boolean bad_label(char *label);
boolean check_instruction(char *instruction);
boolean end_of_string(char *string);
void skip_white_spaces(char **string);

