#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 80
#define IC_INITIAL_VALUE 100 
#define MAX_LABEL_LENGTH 31



typedef enum{
	EXTERNAL,
	RELOCATEABLE
} symbol_type;


typedef enum {
	FALSE = 0, TRUE = 1
} boolean;

typedef enum {
	TYPE_STRING,
	TYPE_NUMBER,
	TYPE_ENTRY,//?
	TYPE_EXTERN//?
} data_types, symbol_data_types;

typedef union{
	char character;
	long number;
} data_value;

typedef struct symbols_table_entry{/*struct for the symbols table*/
	struct symbols_table_entry *next;/*pointer to the next entry*/
	long address;/*address of IC or DC*/
	long L;/*if data type, length of data*/
	char * name;
	symbol_data_types data_type;
	data_types type;
	boolean is_data;
} symbols_table_entry;

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

typedef struct data_table_entry{
	struct data_table_entry *next;
	long address; /*address of DC*/
	data_value data;
	data_types type;	
} data_table_entry;


typedef struct extern_entry{
	struct extern_entry *next;
	long address;
	char *name;
} extern_entry;

typedef struct entry_entry{
	struct entry_entry *next;
	long address;
	char *name;
} entry_entry;


void print_line_info(line_info *line);
void print_symbol_table(symbols_table_entry *symbol_table);
void print_data_table(data_table_entry *data_table);
void print_extern_table(extern_entry *ext);
void print_entry_table(entry_entry *ent);

void reset_line_info(line_info *line);
void reset_str(char *string);
void reset_data(data_table_entry* data);
void reset_extern(extern_entry *ext);
void reset_entry(entry_entry *ent);
void reset_symbol(symbols_table_entry* symbol);

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
void *malloc_or_exit(size_t size);

boolean add_data_to_table(line_info* line, symbols_table_entry* symbol_table, data_table_entry* data_table, extern_entry* ext, entry_entry* ent, long* DC, boolean* error_in_code);
boolean add_symbol_to_table(line_info* line, symbols_table_entry* symbol_table, symbol_data_types data_type, long* DC, long L);

