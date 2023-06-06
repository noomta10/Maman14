#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"
#include "tables.h"



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







boolean is_label(char *);
boolean is_instruction(char *);
boolean check_comma(char **string);
boolean first_pass(char *am_file_name, symbols_table_entry *symbol_table, data_table_entry *data_table,
entry_entry *ent, extern_entry *ext, long *IC, long *DC);
boolean extract_command_info(char *line_content, line_info *line);
boolean validate_line(line_info *line);
void process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry* symbol_table, data_table_entry* data_table, entry_entry* ent, extern_entry* ext, boolean* error_in_code);
boolean bad_label(char *label);
boolean check_instruction(char *instruction);

