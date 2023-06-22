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


boolean first_pass(FILE *am_file, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head,
                   entry_entry** ent_head, extern_entry** ext_head, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbols_table_entry, long* IC, long* DC) /*processes file*/
{
    char line_content[MAX_LINE_LENGTH];
    line_info* line = NULL;
    boolean* error_flag = (boolean *)malloc_with_check(sizeof(boolean));
    *DC = *IC = 0;/*end of page 48*/

    
    line = (line_info*)malloc_with_check(sizeof(line_info)); /*allocating memory for line*/
    reset_line_info(line);
    line->line_number = 1;

    /*reading .as file line by line entil the end*/
    while (fgets(line_content, MAX_LINE_LENGTH, am_file) != NULL) 
    {
        if (line_too_long(am_file, line_content))
        {
            *error_flag = TRUE;
            printf("Error: line %ld is too long.\n", line->line_number);
            continue;
        }
        line->line_content = copy_string(line_content);
        line->line_number++;

        /*checking if line is empty or command line*/
        if (ignore_line(line_content))
        { 
            printf("debug: empty or command line\n");
            continue;
        }

        /*processing line*/
        extract_command_info(line_content, line);
        process_line_first_pass(line, IC, DC, symbol_table_head, data_table_head, ent_head, ext_head, code_table_head, uninitialized_symbols_table_entry); /*processing line*/

        /*reseting variables*/
        reset_line_info(line);
        reset_str(line_content);
    }


    /*if a line was read, free it*/
    if(line)
        free(line);

    add_final_ic_to_dc_count(*symbol_table_head, *data_table_head, *IC, DC);

    program_too_big(*IC, *DC);

    return !error_flag; /*returning error flag*/
}


boolean process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry** symbol_table_head, data_table_entry** data_table,
    entry_entry** ent, extern_entry** ext, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbol_head)
{
    /*validating line*/
    if (!validate_line(line)) {
        printf("debug: invalid line\n");
        return FALSE;
    }

    /*if the line contans data, add it to the memory*/
    if (line->directive_flag) {
        if (!add_data_to_table(line, symbol_table_head, data_table, ext, ent, DC, IC))
            return FALSE;
    }
    /* if the line is instruction, add it to memory */
    if (line->instruction_flag){
        if (!add_instruction_to_table(line, symbol_table_head, ext, code_table_head, uninitialized_symbol_head, IC))
            return FALSE;
    }
    return TRUE;    
}


boolean validate_line(line_info* line)
{
    /*label checking*/
    if (line->label_flag)
    {
        if (bad_label(line->label, line->line_content, line->line_number))
            return FALSE;
    }

    /*checks for errors in directive lines*/
    if (line->directive_flag) 
    {
        if (!valid_directive_line(line))
            return FALSE;
    }

    /*checks for errors in directive lines*/
    if (line->instruction_flag)
    {
        if (!valid_instruction_line(line))
            return FALSE;
    }
    return TRUE;
}

void extract_command_info(char* content, line_info* line)
{
    char* token;
    
    line->line_content = (char*) malloc_with_check(sizeof(char)*(MAX_LINE_LENGTH+1));
    strcpy(line->line_content, content);

    /* if there's a label save it, else set label flag to false */
    skip_white_spaces(&content);
    if (is_label(content)) /* check for label */
    {
        line->label = get_label(&content);
        line->label_flag = TRUE;
    }
    else
    {
        line->label_flag = FALSE;
    }

    /*if there's an directive save it, else set directive flag to false*/
    skip_white_spaces(&content);
    if (is_directive(content))
    {
        line->directive_command = get_directive(&content);
        line->directive_flag = TRUE;
        line->directive_data = content;
        return;
    }
    else
    {
        line->directive_flag = FALSE;
        line->instruction_flag = TRUE;
    }

    /*gets the opcode*/
    skip_white_spaces(&content);
    token = get_opcode(&content); /*get opcode*/
    if (token != NULL)
        line->opcode = token;

    /*gets the fisrt operand*/
    skip_white_spaces(&content);
    token = get_operand(&content); /*get source operand*/
    if (token != NULL)
        line->source_operand = token;
    else
        return;

    /*checks for comma*/
    skip_white_spaces(&content);
    line->comma_flag = check_comma(&content); /*check for comma*/
    skip_white_spaces(&content);

    /*gets the second operand*/
    token = get_operand(&content); /*get target operand*/
    if (token != NULL)
        line->target_operand = token;
    else
        return;

    /*checking for extra characters after command*/
    skip_white_spaces(&content);
    if (!end_of_string(content))
        line->extra_chars_flag = TRUE;
    else
        line->extra_chars_flag = FALSE;
}


addressing_type get_addressing_type(char* operand)
{
    if (operand == NULL || strcmp(operand, "") == 0)
        return NO_OPERAND;
    else if (is_number(operand))
        return IMMEDIATE_ADDRESSING;
    else if (is_register(operand))
        return REGISTER_ADDRESSING;
    else
        return DIRECT_ADDRESSING;
}




