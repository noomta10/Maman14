#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "first_pass.h"
#include "utils.h"
#include "string_handling.h"
#include "debuging.h"
#include "info_check.h"

/* gets a string and returns true if the string starts with a piread (full stop) */
boolean is_directive(char* str)
{
    if (*str == '.')
        return TRUE;
    return FALSE;
}

/* checks if the string that was reseved as argument is a label */
boolean is_label(char* str)
{
    while (!isspace(*str) && !end_of_string(str))
    {
        if (*str == ':')
            return TRUE;
        str++;
    }
    return FALSE;
}

/* checks if the line that was read - line_content, is the whole line. if it is, the function will return true.
 * if its not, will return false and read the read of the line */
boolean line_too_long(FILE* am_file, char* line_content)
{
    int c;

    /* checks if if full line was read */
    while (*line_content != '\0')
    {
        if (*line_content == '\n')
            return FALSE;
        line_content++;
    }
    if ((c = getc(am_file)) == EOF || c == '\n')
        return FALSE;

    /* reads rest of line */
    while ((c = getc(am_file)) != '\n' && c != EOF);

    return TRUE;
}

/* gets a label and the location of the label and returns true if the label if valid. 
 * else retruns false and prints a error */
boolean invalid_label(char* file_name, char* label, char* line_content, long line_number)
{
    int i = 0;

    /*debug*/
    printf("Debug: %s\n", label);

    /* Check if label is a reserved word */
    if (strcmp(label, "mov") == 0 || strcmp(label, "cmp") == 0 ||
        strcmp(label, "add") == 0 || strcmp(label, "sub") == 0 ||
        strcmp(label, "not") == 0 || strcmp(label, "clr") == 0 ||
        strcmp(label, "lea") == 0 || strcmp(label, "inc") == 0 ||
        strcmp(label, "dec") == 0 || strcmp(label, "jmp") == 0 ||
        strcmp(label, "bne") == 0 || strcmp(label, "red") == 0 ||
        strcmp(label, "prn") == 0 || strcmp(label, "jsr") == 0 ||
        strcmp(label, "rts") == 0 || strcmp(label, "stop") == 0 ||
        strcmp(label, "data") == 0 || strcmp(label, "string") == 0 ||
        strcmp(label, "entry") == 0 || strcmp(label, "extern") == 0 ||
        strcmp(label, "@r0") == 0 || strcmp(label, "@r1") == 0 ||
        strcmp(label, "@r2") == 0 || strcmp(label, "@r3") == 0 ||
        strcmp(label, "@r4") == 0 || strcmp(label, "@r5") == 0 ||
        strcmp(label, "@r6") == 0 || strcmp(label, "@r7") == 0)
    {
        PRINT_ERROR(file_name, line_number, line_content, "The label is a reserved word.");
            return TRUE;
    }

    /* Check if label is too long */
    if (strlen(label) > MAX_LABEL_LENGTH) {
        PRINT_ERROR(file_name, line_number, line_content, "The label is too long.");
        return TRUE;
    }
    /* checks if the label is empty */
    if (strlen(label) == 0) {
        PRINT_ERROR(file_name, line_number, line_content, "The label is empty.");
        return TRUE;
    }

    /* Check if label starts with a letter */
    if (!isalpha(label[i])) {
        PRINT_ERROR(file_name, line_number, line_content, "The label must start with a letter.");
        return TRUE;
    }

    /* Check if label has invalid characters */
    for (i = 1; i < strlen(label); i++) {
        if (!isalnum(label[i])) {
            PRINT_ERROR(file_name, line_number, line_content, "The label has invalid characters.");
            return TRUE;
        }
    }

    return FALSE;
}

/* returns true if the string starts with a comma and skips it. else, returns false */
boolean check_comma(char** str) {
    if (**str == ',') {
        (*str)++;
        return TRUE;
    }
    return FALSE;
}

/* returns true if the string is a number */
boolean is_number(char* string) {
    int i = 0;
    if (string == NULL || strlen(string) == 0)
        return FALSE;
    if (string[0] == '-' || string[0] == '+')
        i++;
    for (; i < strlen(string); i++) {
        if (!isdigit(string[i]))
            return FALSE;
    }
    return TRUE;
}

/* returns true if the line is a command or empty line */
boolean ignore_line(char* line) {
    /* Epmty line or comment line */
    if (end_of_string(line) || *line == ';') {
        return TRUE;
    }

    return FALSE;
}

/* calls function to check if the instruction line is valid */
boolean valid_instruction_line(line_info* line)
{
             
    /* check if theres more or less operands then nedded */
    if(!check_extra_or_missing_operands(line))
        return FALSE;

    /* check if operands are valid */
    if(!check_operands(line))
        return FALSE;
    return TRUE;
}

/* calls the right functions to check the directive line */
boolean valid_directive_line(line_info* line) {
    if(strcmp(line->directive_command, "data") == 0) {
        if (!valid_data_command(line))
            return FALSE;
    }
    else if(strcmp(line->directive_command, "string") == 0) {
        if(!valid_string_command(line))
            return FALSE;
    }
    else if(strcmp(line->directive_command, "entry") == 0) {
        if (!valid_entry_command(line))
            return FALSE;
    }
    else if(strcmp(line->directive_command, "extern") == 0) {
        if (!valid_extern_command(line))
            return FALSE;
    }
    else {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid directive command.");
        //printf("Error: in line %ld: %sInvalid directive command\n", line->line_number, line->line_content);
        return FALSE;
    }

    return TRUE;
}

/* checks if the line is a valid data command */
boolean valid_data_command(line_info* line) {
    if (string_is_empty(line->directive_data)) {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Missing data.");
        return FALSE;
    }
    return TRUE;
}

/* checks if the line is a valid string command */
boolean valid_string_command(line_info* line) {
    char* directive_data = line->directive_data;
    /* set i to the end of the line and skip the null character */
    int i = strlen(directive_data) -1 ;
    /* check if string is empty */
    if(end_of_string(directive_data)) {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Missing data.");
        return FALSE;
    }
    /* skip white spaced and update the length i */
    while (isspace(*directive_data) && i >= 0) {
        i--;
        directive_data++;
    }
    /* check if string starts with quotes */
    if(*directive_data != '"') {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "String must start with quotes.");
        return FALSE;
    }
    /* check if string ends with quotes */
    while(isspace(directive_data[i]))
        i--;
    if(directive_data[i] != '"' || i == 0) {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "String must end with quotes.");
        return FALSE;
    }

    return TRUE;
}

/* checks if the line is a valid entry command */
boolean valid_entry_command(line_info* line) {
    if (string_is_empty(line->directive_data)) { 
        /* no entry labels were given */
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "No entry labels were given.");
        return FALSE;
    }
    /* ilegal comma at the end of the line */
    if(extra_comma(line->line_content)){
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Extra comma at end of line.");
        return FALSE;
    }
    return TRUE;
}

/* checks if the line is a valid extern command */
boolean valid_extern_command(line_info* line) {
    /* no extern labels were given */
    if(string_is_empty(line->directive_data)) { 
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "No extern labels were given.");
        return FALSE;
    }
    /* ilegal comma at the end of the line */
    if(extra_comma(line->line_content)){
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Extra comma at end of line.");
        return FALSE;
    }
    return TRUE;
}

/* checks if there is the right amount of operands */
boolean check_extra_or_missing_operands(line_info* line) {
 /*checking sub, mov, add, lea commands*/
    if (strcmp(line->opcode, "sub") == 0 || 
        strcmp(line->opcode, "mov") == 0 ||
        strcmp(line->opcode, "add") == 0 ||
        strcmp(line->opcode, "cmp") == 0 ||
        strcmp(line->opcode, "lea") == 0) {
        /*checking for missing operand*/
        if (string_is_empty(line->source_operand) || string_is_empty(line->target_operand)) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "missing operand.");
            return FALSE;
        }
    }
    /*checking not, clr, inc, dec, jmp, bne, red, prn, jsr commands*/
    else if (strcmp(line->opcode, "not") == 0 ||
        strcmp(line->opcode, "clr") == 0 ||
        strcmp(line->opcode, "inc") == 0 ||
        strcmp(line->opcode, "dec") == 0 ||
        strcmp(line->opcode, "jmp") == 0 ||
        strcmp(line->opcode, "bne") == 0 ||
        strcmp(line->opcode, "red") == 0 ||
        strcmp(line->opcode, "prn") == 0 ||
        strcmp(line->opcode, "jsr") == 0) {
            /*checking for missing operand*/
        if (string_is_empty(line->source_operand)) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "missing operand");
            return FALSE;
        }
        /*checking for extra operand*/
        if(!string_is_empty(line->target_operand)){
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "extra operand");
            return FALSE;
        }
        line->target_operand = line->source_operand; /*moving source operand to target operand*/
        line->source_operand = NULL;
    }
    /*checking rts, stop commands*/
    else if (strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0) {
            /*checking for extra operand*/
        if (!string_is_empty(line->source_operand) || !string_is_empty(line->target_operand)) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "extra operand");
            return FALSE;
        }
    }
    /*invalid opcode*/
    else {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid opcode");
        return FALSE;
    }

    return TRUE;
}

/* checks if the operands are valid */
boolean check_operands(line_info* line) {
    /* checking if source operand is valid */
    if(!valid_source_operand(line))
        return FALSE;
    
    /* checking target operand */
    if(!valid_target_operand(line))
        return FALSE;
    return TRUE;
}

/* checks if the source operand is valid */
boolean valid_source_operand(line_info* line) {
    if (strcmp(line->opcode, "mov") == 0 ||
            strcmp(line->opcode, "cmp") == 0 ||
            strcmp(line->opcode, "add") == 0 ||
            strcmp(line->opcode, "sub") == 0 ) {
        if( get_addressing_type(line->source_operand) != IMMEDIATE_ADDRESSING &&
            get_addressing_type(line->source_operand) != DIRECT_ADDRESSING &&
            get_addressing_type(line->source_operand) != REGISTER_ADDRESSING) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid source operand");
                //printf("Error: in line %ld %s invalid source operand\n", line->line_number, line->line_content);
                return FALSE;
            }
    }
    else if(strcmp(line->opcode, "lea") == 0) {
        return get_addressing_type(line->source_operand) == DIRECT_ADDRESSING;
    }
    else if(strcmp(line->opcode, "not") == 0 ||
        strcmp(line->opcode, "clr") == 0 ||
        strcmp(line->opcode, "inc") == 0 ||
        strcmp(line->opcode, "dec") == 0 ||
        strcmp(line->opcode, "jmp") == 0 ||
        strcmp(line->opcode, "bne") == 0 ||
        strcmp(line->opcode, "red") == 0 ||
        strcmp(line->opcode, "prn") == 0 ||
        strcmp(line->opcode, "jsr") == 0 ||
        strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0) {
        if( get_addressing_type(line->source_operand) != NO_OPERAND) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid source operand");
                return FALSE;
            }
    }

    /* if source operand is a label, check it */
    if(get_addressing_type(line->source_operand) == DIRECT_ADDRESSING && invalid_label(line->file_name, line->source_operand, line->line_content, line->line_number))
            return FALSE;

    if(get_addressing_type(line->source_operand) == IMMEDIATE_ADDRESSING &&( instruction_number_too_big(line->source_operand) || instruction_number_too_small(line->source_operand))){
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "operand numbers must be between -512 and 511");
            return FALSE;
    }
    return TRUE;
}

/* checks if the target operand is valid */
boolean valid_target_operand(line_info* line) {
    if (strcmp(line->opcode, "cmp") == 0 ||
        strcmp(line->opcode, "prn") == 0) {
        if( get_addressing_type(line->target_operand) != IMMEDIATE_ADDRESSING &&
            get_addressing_type(line->target_operand) != DIRECT_ADDRESSING &&
            get_addressing_type(line->target_operand) != REGISTER_ADDRESSING) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid target operand");
                //printf("Error: in line %ld %s invalid target operand\n", line->line_number, line->line_content);
                return FALSE;
            }
    }
    else if(strcmp(line->opcode, "mov") == 0 ||
        strcmp(line->opcode, "add") == 0 ||
        strcmp(line->opcode, "sub") == 0 ||
        strcmp(line->opcode, "not") == 0 ||
        strcmp(line->opcode, "clr") == 0 ||
        strcmp(line->opcode, "lea") == 0 ||
        strcmp(line->opcode, "inc") == 0 ||
        strcmp(line->opcode, "dec") == 0 ||
        strcmp(line->opcode, "jmp") == 0 ||
        strcmp(line->opcode, "bne") == 0 ||
        strcmp(line->opcode, "red") == 0 ||
        strcmp(line->opcode, "jsr") == 0) {
        if( get_addressing_type(line->target_operand) != DIRECT_ADDRESSING &&
            get_addressing_type(line->target_operand) != REGISTER_ADDRESSING) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid target operand");
                //printf("Error: in line %ld %s invalid target operand\n", line->line_number, line->line_content);
                return FALSE;
            }
    }
    else if(strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0) {
            if( get_addressing_type(line->target_operand) != NO_OPERAND) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid target operand");
                //printf("Error: in line %ld %s invalid target operand\n", line->line_number, line->line_content);
                return FALSE;
            }
    }

    /* if target address is an lable, check it */
    if(get_addressing_type(line->target_operand) == DIRECT_ADDRESSING && invalid_label(line->file_name, line->target_operand, line->line_content, line->line_number))
            return FALSE;

    if(get_addressing_type(line->target_operand) == IMMEDIATE_ADDRESSING && (instruction_number_too_big(line->target_operand) || instruction_number_too_small(line->target_operand))){
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "operand numbers must be between -512 and 511");
            return FALSE;
    }
    return TRUE;
}

/* check if the symbol is already defined in symbol_table */
boolean exists_in_symbol_table(char* symbol, symbols_table_entry* symbol_table) {
    while (symbol_table != NULL) {
        if (strcmp(symbol_table->name, symbol) == 0)
            return TRUE;
        symbol_table = symbol_table->next;
    }
    return FALSE;
}

/* checks if symbol already exists in extern table */
boolean exists_in_extern_table(char* symbol, extern_entry* external_table) {
    while (external_table != NULL) {
        if (strcmp(external_table->name, symbol) == 0)
            return TRUE;
        external_table = external_table->next;
    }
    return FALSE;
}

/* checks if symbol already exists in entry table */
boolean exists_in_entry_table(char* symbol, entry_entry* entry_table) {
    while (entry_table != NULL) {
        if (strcmp(entry_table->name, symbol) == 0)
            return TRUE;
        entry_table = entry_table->next;
    }
    return FALSE;
}

/* checks if the program size (IC + DC) is bigger than what the imagenary PC can hold */
boolean program_too_big(char* file_name, long IC, long DC) {
    char* as_file_name;

    /* if the program is too big, print error and return TRUE */
    if (IC + DC >= MAX_PROGRAM_LENGTH) {
        as_file_name = add_file_postfix(file_name, ".as");
        printf("Error: The file '%s' you provided is too large for the imaginary computer.\n", as_file_name);
        printf("Debug: IC + DC = %ld\n", IC + DC);
        free(as_file_name);
        return TRUE;
    }
    return FALSE;
}

/* checks if the operand is a regiser */
boolean is_register(char* operand) {
	if (operand[0] == '@' && operand[1] == 'r' && operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0')
		return TRUE;
	return FALSE;
}

/* checks if the number is too big to code in to the memory word */
boolean data_number_too_big(char* string_number) {
    int number = atoi(string_number);
    if (number > MAX_DATA_NUMBER_VALUE)
        return TRUE;
    return FALSE;
}

boolean instruction_number_too_big(char* string_number) {
    int number = atoi(string_number);
    if (number > MAX_INSTRUCTION_NUMBER_VALUE)
        return TRUE;
    return FALSE;
}

boolean instruction_number_too_small(char* string_number) {
    int number = atoi(string_number);
    if (number < MIN_INSTRUCTION_NUMBER_VALUE)
        return TRUE;
    return FALSE;
}
/* checks if the number is too small to code in to the memory word */
boolean data_number_too_small(char* string_number) {
    int number = atoi(string_number);
    if (number < MIN_DATA_NUMBER_VALUE)
        return TRUE;
    return FALSE;
}

/* checks if the string is empty (white spaces) */
boolean string_is_empty(char* string) {
    int i;
    if (string == NULL)
        return TRUE;
    for (i = 0; i < strlen(string); i++) {
        if (!isspace(string[i])) {
            return FALSE;
        }
    }

    return TRUE;
}

/* checks for extra command in the end of the line */
boolean extra_comma(char* line) {
    int i = 0;

    /* Go to '\0' position */
    while (line[i] != '\0') {
        i++;
    }

    do{
        /* One strp back */
        i--;

        if (line[i] == ',') {
            return TRUE;
        }
    } while (isspace(line[i]));

    return FALSE;
}