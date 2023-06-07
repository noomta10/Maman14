#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "assembler.h"
#include "tables.h"
#include "first_pass.h"
#include "utils.h"
#include "string_handling.h"






boolean first_pass(FILE *am_file, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head,
                   entry_entry** ent_head, extern_entry** ext_head, long* IC, long* DC) /*processes file*/
{
    char line_content[MAX_LINE_LENGTH];
    line_info* line = NULL;
    boolean* error_flag = (boolean *)malloc_with_check(sizeof(boolean));
    int line_number = 0;/*debugging*/
    *DC = *IC = 0;/*end of page 48*/

    /*reading .as file line by line entil the end*/
    while (fgets(line_content, MAX_LINE_LENGTH, am_file) != NULL) 
    {
        line = (line_info*)malloc_with_check(sizeof(line_info)); /*allocating memory for line*/

        /*debugging*/
        line_number++;
        printf("debug: line number: %d\n", line_number);
        printf("debug: line content: %s\n", line_content);

        /*checking if line is empty or command line*/
        if (ignore_line(line_content))
        { 
            printf("debug: empty or command line\n");
            continue;
        }

        /*processing line*/
        extract_command_info(line_content, line);
        process_line_first_pass(line, IC, DC, symbol_table_head, data_table_head, ent_head, ext_head, error_flag); /*processing line*/

        /*reseting variables*/
        reset_line_info(line);
        reset_str(line_content);
    }


    /*if line was read, free it*/
    if(line)
        free(line);

    /*create tables and files*/

    return !error_flag; /*returning error flag*/
}

void process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry** symbol_table_head, data_table_entry** data_table,
    entry_entry** ent, extern_entry** ext, boolean* error_flag)
{
    /*validating line*/
    if (!validate_line(line)) 
    {
        printf("debug: invalid line\n");
        *error_flag = TRUE;
    }

    /*if the line contans data, add it to the memory*/
    if (line->instruction_flag) {
        if (!add_data_to_table(line, symbol_table_head, data_table, ext, ent, DC))
            *error_flag = TRUE;
    }


    /*continue here */
}


boolean validate_line(line_info* line)
{
    /*label checking*/
    if (line->label_flag)
    {
        if (bad_label(line->label))
            return FALSE;
    }

    /*checks for errors in instruction lines*/
    if (line->instruction_flag) 
    {
        if (!check_instruction(line->instruction))
            return FALSE;
    }

    /*checks for errors in command lines*/
    else if (!line->instruction_flag)
    {
        /*checking sub, mov, add, lea commands*/
        if (strcmp(line->opcode, "sub") == 0 || 
            strcmp(line->opcode, "mov") == 0 ||
            strcmp(line->opcode, "add") == 0 ||
            strcmp(line->opcode, "lea") == 0)
        {
            /*checking for missing operand*/
            if (line->source_operand == NULL || line->target_operand == NULL)
            {
                printf("Error: missing operand\n");
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
                printf("Error: missing operand\n");
                return FALSE;
            }
            /*checking for extra operand*/
            else if (line->target_operand != NULL && strcmp(line->target_operand, "") != 0)
            { 
                printf("Error: extra operand\n");
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
                printf("Error: extra operand\n");
                return FALSE;
            }
        }

        /*invalid opcode*/
        else
        {
            printf("Error: invalid opcode\n");
            return FALSE;
        }
    }
    return TRUE;
}

void extract_command_info(char* content, line_info* line)
{
    char* token;
    
    /*if there's a label save it, else set label flag to false*/
    skip_white_spaces(&content);
    if (is_label(content)) /*check for label*/
    {
        token = get_label(&content);
        line->label_flag = TRUE;
        line->label = token;
    }
    else
    {
        line->label_flag = FALSE;
    }

    /*if there's an instruction save it, else set instruction flag to false*/
    skip_white_spaces(&content);
    if (is_instruction(content))
    {
        token = get_instruction(&content);
        line->instruction_flag = TRUE;
        line->instruction = token;
        line->instruction_data = content;
        return;
    }
    else
    {
        line->instruction_flag = FALSE;
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

boolean is_instruction(char* str)
{
    if (*str == '.')
        return TRUE;
    return FALSE;
}

boolean is_label(char* str)
{
    char* temp = str;
    while (!isspace(*temp) && *temp != '\0')
        temp++;
    if (*--temp == ':')
        return TRUE;
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



boolean check_instruction(char* str)
{
    if (strcmp(str, "data") || strcmp(str, "string") ||
        strcmp(str, "entry") || strcmp(str, "extern"))
        return TRUE;
    return FALSE;
}

boolean bad_label(char* str)
{
    /*check if label is a reserved word*/
    if(strcmp(str, "mov") == 0 || strcmp(str, "cmp") == 0 ||
       strcmp(str, "add") == 0 || strcmp(str, "sub") == 0 ||
       strcmp(str, "not") == 0 || strcmp(str, "clr") == 0 ||
       strcmp(str, "lea") == 0 || strcmp(str, "inc") == 0 ||
       strcmp(str, "dec") == 0 || strcmp(str, "jmp") == 0 ||
       strcmp(str, "bne") == 0 || strcmp(str, "red") == 0 ||
       strcmp(str, "prn") == 0 || strcmp(str, "jsr") == 0 ||
       strcmp(str, "rts") == 0 || strcmp(str, "stop") == 0 ||
       strcmp(str, "data") == 0 || strcmp(str, "string") == 0 ||
       strcmp(str, "entry") == 0 || strcmp(str, "extern") == 0 ||
       *str == '@')
    {
        printf("Error: label is a reserved word");
        return TRUE;
    }

    /*check if label is too long or too short*/
    if (strlen(str) > MAX_LABEL_LENGTH)
    {
        printf("Error: label to long");
        return TRUE;
    }
    if (strlen(str) == 0)
    {
        printf("Error: label to short");
        return TRUE;
    }

    return FALSE;
}


