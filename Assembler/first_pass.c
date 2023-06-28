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


/* Gets a line and calls functions to add the data to the tables */
static boolean process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry** symbol_table_head, data_table_entry** data_table,
    entry_entry** ent, extern_entry** ext, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbol_head) {
    /* Checks and prints errors in the line */
    if (!validate_line(line)) {
        printf("debug: invalid line\n");
        return FALSE;
    }
    /* If the line contans data, add it to the memory */
    if (line->directive_flag) {
        if (!add_data_to_table(line, symbol_table_head, data_table, ext, ent, DC, IC))
            return FALSE;
    }
    /* If the line is instruction, add it to memory */
    if (line->instruction_flag){
        if (!add_instruction_to_table(line, symbol_table_head, ext, code_table_head, uninitialized_symbol_head, IC))
            return FALSE;
    }
    return TRUE;    
}

/* Calls functions to check errors based on the type of the line */
static boolean validate_line(line_info* line) {
    boolean valid_line_flag = TRUE;
    /* If there's a bad lable print the error and continue to check rest of line */
    if (line->label_flag){
        if (invalid_label(line->file_name, line->label, line->line_content, line->line_number))
            valid_line_flag = FALSE;
    }

    /* Checks for errors in directive lines */
    if (line->directive_flag) {
        if (!valid_directive_line(line))
            return FALSE;
    }

    /* Checks for errors in directive lines */
    if (line->instruction_flag) {
        if (!valid_instruction_line(line))
            return FALSE;
    }
    return valid_line_flag;
}

/* Gets a string that contains the line that was read and extact the infomaing by saving it in line_info */
static void extract_command_info(char* content, line_info* line) {
    char* token;
    
    line->line_content = (char*) malloc_with_check(sizeof(char)*(MAX_LINE_LENGTH+1));
    strcpy(line->line_content, content);

    /* If there's a label save it, else set label flag to false */
    skip_white_spaces(&content);
    if (is_label(content)) {
        line->label = get_label(&content);
        line->label_flag = TRUE;
    }
    else{
        line->label_flag = FALSE;
    }

    /* If there's a directive save it, else set directive flag to false */
    skip_white_spaces(&content);
    if (is_directive(content)){
        line->directive_command = get_directive(&content);
        line->directive_flag = TRUE;
        line->directive_data = content;
        return;
    }
    else {
        line->directive_flag = FALSE;
        line->instruction_flag = TRUE;
    }

    /* Gets the opcode */
    skip_white_spaces(&content);
    token = get_opcode(&content); /* Get opcode */
    if (token != NULL)
        line->opcode = token;

    /* Gets the fisrt operand */
    skip_white_spaces(&content);
    token = get_operand(&content); /* Get source operand */
    if (token != NULL)
        line->source_operand = token;
    else
        return;

    /*checks for comma*/
    skip_white_spaces(&content);
    line->comma_flag = check_comma(&content); /*check for comma*/
    skip_white_spaces(&content);

    /* Gets the second operand */
    token = get_operand(&content); /* Get target operand */
    if (token != NULL)
        line->target_operand = token;
    else
        return;

    /* Checking for extra characters after command */
    skip_white_spaces(&content);
    if (!end_of_string(content))
        line->extra_chars_flag = TRUE;
    else
        line->extra_chars_flag = FALSE;
}

/* Reades file and add infomation to tables */
boolean first_pass(char* file_name, FILE* am_file, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head,
    entry_entry** ent_head, extern_entry** ext_head, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbols_table_entry, long* IC, long* DC) {
    char line_content[MAX_LINE_LENGTH];
    line_info* line = NULL;
    boolean* error_flag = (boolean*)malloc_with_check(sizeof(boolean));
    *error_flag = FALSE;
    *DC = *IC = 0;


    line = (line_info*)malloc_with_check(sizeof(line_info)); /* Allocating memory for line */
    reset_line_info(line);
    line->line_number = 0;
    line->file_name = add_file_postfix(file_name, ".as");

    /* Reading .as file line by line entil the end */
    while (fgets(line_content, MAX_LINE_LENGTH, am_file) != NULL) {
        line->line_number++;
        printf("Debug: line %ld %s", line->line_number, line_content);
        /* Checks line size */
        if (line_too_long(am_file, line_content)){
            *error_flag = TRUE;
            PRINT_ERROR(line->file_name, line->line_number, line_content, "Line is too long.");
            continue;
        }

        remove_new_line_character(line_content);
        line->line_content = copy_string(line_content);

        /* Checking if line is empty or command line */
        if (ignore_line(line_content)){
            reset_line_info(line);
            continue;
        }

        /* Processing line */
        extract_command_info(line_content, line);
        if (!process_line_first_pass(line, IC, DC, symbol_table_head, data_table_head, ent_head, ext_head, code_table_head, uninitialized_symbols_table_entry)) {
            *error_flag = TRUE;
        }
        /* Reseting variables */
        reset_line_info(line);
        reset_str(line_content);
        printf("\n");
    }


    /* If a line was read, free it*/
    if (line) {
        free(line->file_name);
        free(line);
    }

    add_final_ic_to_dc_count(*symbol_table_head, *data_table_head, *IC, DC);

    /* Check program size */
    if (program_too_big(file_name, *IC, *DC)) {
        *error_flag = TRUE;
    }

    return *error_flag; 
}

/* Gets a string that contans an operand and returns the addressing type */
addressing_type get_addressing_type(char* operand){
    if (operand == NULL || strcmp(operand, "") == 0)
        return NO_OPERAND;
    else if (is_number(operand))
        return IMMEDIATE_ADDRESSING;
    else if (is_register(operand))
        return REGISTER_ADDRESSING;
    else
        return DIRECT_ADDRESSING;
}
