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


boolean is_directive(char* str)
{
    if (*str == '.')
        return TRUE;
    return FALSE;
}

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

boolean bad_label(char* file_name, char* label, char* line_content, long line_number)
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
        PRINT_ERROR(file_name, line_number, line_content, "label is a reserved word");
        //printf("Error: in line %ld: %s\nThe label '%s' is a reserved word\n", line_number, line_content, label);
            return TRUE;
    }

    /* Check if label is too long or too short */
    if (strlen(label) > MAX_LABEL_LENGTH)
    {
        PRINT_ERROR(file_name, line_number, line_content, "label is too long");
        //printf("Error: in line %ld: %s\nThe label '%s' is too long\n", line_number, line_content, label);
        return TRUE;
    }
    if (strlen(label) == 0)
    {
        PRINT_ERROR(file_name, line_number, line_content, "empty label");
        //printf("Error: in line %ld: %s\nThe label '%s' is too short\n", line_number, line_content, label);
        return TRUE;
    }

    /* Check if label starts with a letter */
    if (!isalpha(label[i])) {
        PRINT_ERROR(file_name, line_number, line_content, "label must start with a letter");
        //printf("Error: in line %ld: %s\nThe label '%s' must start with a letter\n", line_number, line_content, label);
        return TRUE;
    }

    /* Check if label has invalid characters */
    for (i = 1; i < strlen(label); i++)
    {
        if (!isalnum(label[i]))
        {
            PRINT_ERROR(file_name, line_number, line_content, "label has invalid characters");
            //printf("Error: in line %ld: %s\nThe label '%s' has invalid characters\n",line_number, line_content, label);
            return TRUE;
        }
    }

    return FALSE;
}

boolean check_comma(char** str)
{
    if (**str == ',')
    {
        (*str)++;
        return TRUE;
    }
    return FALSE;
}

boolean is_number(char* string)
{
    int i = 0;
    if (string == NULL)
        return FALSE;
    if (string[0] == '-' || string[0] == '+')
        i++;
    for (; i < strlen(string); i++)
    {
        if (!isdigit(string[i]))
            return FALSE;
    }
    return TRUE;
}

boolean ignore_line(char* line)
{
    /* Epmty line or comment line */
    if (end_of_string(line) || *line == ';')
    {
        return TRUE;
    }

    return FALSE;
}

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

boolean valid_directive_line(line_info* line)
{
    if(strcmp(line->directive_command, "data") == 0)
    {
        if (!valid_data_command(line))
            return FALSE;
    }
    else if(strcmp(line->directive_command, "string") == 0)
    {
        if(!valid_string_command(line))
            return FALSE;
    }
    else if(strcmp(line->directive_command, "entry") == 0)
    {
        if (!valid_entry_command(line))
            return FALSE;
    }
    else if(strcmp(line->directive_command, "extern") == 0)
    {
        if (!valid_extern_command(line))
            return FALSE;
    }
    else {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid directive command");
        //printf("Error: in line %ld: %sInvalid directive command\n", line->line_number, line->line_content);
        return FALSE;
    }

    return TRUE;
}

boolean valid_data_command(line_info* line)
{
    if (string_is_empty(line->directive_data))
    {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "missing data");
        //printf("Error: in line %ld: %s issing data\n", line->line_number, line->line_content);
        return FALSE;
    }
    return TRUE;
}

boolean valid_string_command(line_info* line)
{
    char* directive_data = line->directive_data;
    /* set i to the end of the line and skip the null character */
    int i = strlen(directive_data) -1 ;
    /* check if string is empty */
    if(end_of_string(directive_data))
    {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "missing data");
        //printf("Error: in line %ld: %s\nMissing data\n", line->line_number, line->line_content);
        return FALSE;
    }
    /* skip white spaced and update the length i */
    while (isspace(*directive_data) && i >= 0) {
        i--;
        directive_data++;
    }
    /* check if string starts with quotes */
    if(*directive_data != '"')
    {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "string must start with quotes");
        //printf("Error: in line %ld: %s\nString must start with quotes\n", line->line_number, line->line_content);
        return FALSE;
    }
    /* check if string ends with quotes */
    while(isspace(directive_data[i]))
        i--;
    if(directive_data[i] != '"')
    {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "string must end with quotes");
        //printf("Error: in line %ld: %s\nString must end with quotes\n", line->line_number, line->line_content);
        return FALSE;
    }

    return TRUE;
}


boolean valid_entry_command(line_info* line)
{
    if (string_is_empty(line->directive_data))
    { 
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "no entry labels given");
        //printf("Error: in line %ld: %s\nNo entry labels given\n", line->line_number, line->line_content);
        return FALSE;
    }
    return TRUE;
}

boolean valid_extern_command(line_info* line)
{
    if(strcmp(line->directive_data, "") == 0){ 
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "no extern labels given");
        //printf("Error: in line: %ld %s no extern labels given\n", line->line_number, line->line_content);
        return FALSE;
    }
    return TRUE;
}

boolean check_extra_or_missing_operands(line_info* line)
{
 /*checking sub, mov, add, lea commands*/
    if (strcmp(line->opcode, "sub") == 0 || 
        strcmp(line->opcode, "mov") == 0 ||
        strcmp(line->opcode, "add") == 0 ||
        strcmp(line->opcode, "cmp") == 0 ||
        strcmp(line->opcode, "lea") == 0)
    {
        /*checking for missing operand*/
        if (line->source_operand == NULL || line->target_operand == NULL)
        {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "missing operand");
            //printf("Error: in line %ld %s missing operand\n", line->line_number, line->line_content);
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
        strcmp(line->opcode, "jsr") == 0)
    {
            /*checking for missing operand*/
        if (line->source_operand == NULL)
        {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "missing operand");
            //printf("Error: in line %ld %s missing operand\n", line->line_number, line->line_content);
            return FALSE;
        }
        /*checking for extra operand*/
        else if (line->target_operand != NULL && strcmp(line->target_operand, "") != 0)
        { 
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "extra operand");
            //printf("Error: in line %ld %sextra operand\n", line->line_number, line->line_content);
            return FALSE;
        }
        line->target_operand = line->source_operand; /*moving source operand to target operand*/
        line->source_operand = NULL;
    }
    /*checking rts, stop commands*/
    else if (strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0)
    {
            /*checking for extra operand*/
        if ((line->source_operand != NULL && strcmp(line->source_operand, "")) ||
            (line->target_operand != NULL && strcmp(line->target_operand, "")))
        {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "extra operand");
            //printf("Error: in line %ld %s extra operand\n", line->line_number, line->line_content);
            return FALSE;
        }
    }
    /*invalid opcode*/
    else
    {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid opcode");
        //printf("Error: in line %ld: %s\nInvalid opcode\n", line->line_number, line->line_content);
        return FALSE;
    }

    return TRUE;
}

boolean check_operands(line_info* line)
{
    /* checking if source operand is valid */
    if(!valid_source_operand(line))
        return FALSE;
    
    /* checking target operand */
    if(!valid_target_operand(line))
        return FALSE;
    return TRUE;
}

boolean valid_source_operand(line_info* line)
{
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
                //printf("Error: in line %ld %s invalid source operand\n", line->line_number, line->line_content);
                return FALSE;
            }
    }

    /* if source operand is a label, check it */
    if(get_addressing_type(line->source_operand) == DIRECT_ADDRESSING && bad_label(line->file_name, line->source_operand, line->line_content, line->line_number))
            return FALSE;

    if(get_addressing_type(line->source_operand) == IMMEDIATE_ADDRESSING && data_number_too_big(line->source_operand)){
        printf("Error: in file `%s` line %ld \"%s\" the number `%s` must be between %d - %d\n",line->file_name, line->line_number, line->line_content,
                line->source_operand, MIN_DATA_NUMBER_VALUE, MAX_DATA_NUMBER_VALUE);
            return FALSE;
    }
    return TRUE;
}

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
    if(get_addressing_type(line->target_operand) == DIRECT_ADDRESSING && bad_label(line->file_name, line->target_operand, line->line_content, line->line_number))
            return FALSE;

    if(get_addressing_type(line->target_operand) == IMMEDIATE_ADDRESSING && data_number_too_big(line->target_operand)){
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "invalid target operand");
            //printf("Error: in line %ld %s the number `%s` must be between %d - %d\n", line->line_number, line->line_content,
                //line->target_operand, MIN_INT_VALUE, MAX_INT_VALUE);
            return FALSE;
    }
    return TRUE;
}

boolean exists_in_symbol_table(char* symbol, symbols_table_entry* symbol_table) {
    while (symbol_table != NULL) {
        if (strcmp(symbol_table->name, symbol) == 0)
            return TRUE;
        symbol_table = symbol_table->next;
    }
    return FALSE;
}

boolean exists_in_extern_table(char* symbol, extern_entry* external_table) {
    while (external_table != NULL) {
        if (strcmp(external_table->name, symbol) == 0)
            return TRUE;
        external_table = external_table->next;
    }
    return FALSE;
}

boolean exists_in_entry_table(char* symbol, entry_entry* entry_table) {
    while (entry_table != NULL) {
        if (strcmp(entry_table->name, symbol) == 0)
            return TRUE;
        entry_table = entry_table->next;
    }
    return FALSE;
}

boolean program_too_big(char* file_name, long IC, long DC){
    char* as_file_name;

    if (IC + DC >= MAX_PROGRAM_LENGTH) {
        as_file_name = add_file_postfix(file_name, ".as");
        printf("Error: The file '%s' you provided is too large for the imaginary computer.\n", as_file_name);
        printf("Debug: IC + DC = %ld\n", IC + DC);
        free(as_file_name);
        return TRUE;
    }
    return FALSE;
}

boolean is_register(char* operand) {
	if (operand[0] == '@' && operand[1] == 'r' && operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0')
		return TRUE;
	return FALSE;
}

boolean data_number_too_big(char* string_number) {
    int number = atoi(string_number);
    if (number > MAX_DATA_NUMBER_VALUE)
        return TRUE;
    return FALSE;
}

boolean data_number_too_small(char* string_number) {
    int number = atoi(string_number);
    if (number < MIN_DATA_NUMBER_VALUE)
        return TRUE;
    return FALSE;
}

boolean string_is_empty(char* string) {
    int i;
    for (i = 0; i < strlen(string); i++) {
        if (!isspace(string[i])) {
            return FALSE;
        }
    }

    return TRUE;
}