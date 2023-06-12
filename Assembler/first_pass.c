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


boolean first_pass(FILE *am_file, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head,
                   entry_entry** ent_head, extern_entry** ext_head, long* IC, long* DC) /*processes file*/
{
    char line_content[MAX_LINE_LENGTH];
    line_info* line = NULL;
    boolean* error_flag = (boolean *)malloc_with_check(sizeof(boolean));
    *DC = *IC = 0;/*end of page 48*/

    
    line = (line_info*)malloc_with_check(sizeof(line_info)); /*allocating memory for line*/
    line->line_number = 1;

    /*reading .as file line by line entil the end*/
    while (fgets(line_content, MAX_LINE_LENGTH, am_file) != NULL) 
    {
        if (check_line_length(am_file, line_content))
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
        process_line_first_pass(line, IC, DC, symbol_table_head, data_table_head, ent_head, ext_head, error_flag); /*processing line*/

        /*reseting variables*/
        reset_line_info(line);
        reset_str(line_content);
    }


    /*if line was read, free it*/
    if(line)
        free(line);

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
    if (line->directive_flag) {
        if (!add_data_to_table(line, symbol_table_head, data_table, ext, ent, DC))
            *error_flag = TRUE;
    }


    /*continue here */
}


boolean validate_line(line_info* line)
{
    boolean valid = TRUE;
    /*label checking*/
    if (line->label_flag)
    {
        if (bad_label(line->label))
            valid = FALSE;
    }

    /*checks for errors in directive lines*/
    if (line->directive_flag) 
    {
        if(strcmp(line->directive_command, "data") == 0)
        {
            if(strcmp(line->directive_data, "") == 0)
            {
                printf("Error: in line %ld %s\nmissing data\n", line->line_number, line->line_content);
                valid = FALSE;
            }
        }
        else if(strcmp(line->directive_command, "string") == 0)
        {
            if(strcmp(line->directive_data, "") == 0)
            {
                printf("Error: in line\n%ld %s\nmissing data\n", line->line_number, line->line_content);
                valid = FALSE;
            }
        }
        else if(strcmp(line->directive_command, "entry") == 0)
        {
            if(strcmp(line->directive_data, "") == 0)
            {
                printf("Warning: in line:\n%ld %s\nno entry labels given\n", line->line_number, line->line_content);
                valid = FALSE;
            }
        }
        else if(strcmp(line->directive_command, "extern") == 0)
        {
            if(strcmp(line->directive_data, "") == 0)
            {
                printf("Warning:\n%ld %s no extern labels given\n", line->line_number, line->line_content);
                valid = FALSE;
            }
        }
        else
        {
            printf("Error: invalid directive command\n");
            valid = FALSE;
        }            
    }
    /*checks for errors in command lines*/
    if (!line->directive_flag)
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
                valid = FALSE;
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
                valid = FALSE;
            }
            /*checking for extra operand*/
            else if (line->target_operand != NULL && strcmp(line->target_operand, "") != 0)
            { 
                printf("Error: extra operand\n");
                valid = FALSE;
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
                valid = FALSE;
            }
        }
        /*invalid opcode*/
        else
        {
            printf("Error: invalid opcode\n");
            valid = FALSE;
        }
    }
    return valid;
}

void extract_command_info(char* content, line_info* line)
{
    char* token;
    
    line->line_content = (char*) malloc_with_check(sizeof(char)*(MAX_LINE_LENGTH+1));
    strcpy(line->line_content, content);

    /*if there's a label save it, else set label flag to false*/
    skip_white_spaces(&content);
    if (is_label(content)) /*check for label*/
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

boolean is_directive(char* str)
{
    if (*str == '.')
        return TRUE;
    return FALSE;
}

boolean is_label(char* str)
{
    char* temp = str;
    while (!isspace(*str++))
        if (*str == ':')
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


boolean bad_label(char* label)
{
    int i = 0;

    /*debug*/
    printf("%s", label);

    /* Check if label is a reserved word */
    if(strcmp(label, "mov") == 0 || strcmp(label, "cmp") == 0 ||
       strcmp(label, "add") == 0 || strcmp(label, "sub") == 0 ||
       strcmp(label, "not") == 0 || strcmp(label, "clr") == 0 ||
       strcmp(label, "lea") == 0 || strcmp(label, "inc") == 0 ||
       strcmp(label, "dec") == 0 || strcmp(label, "jmp") == 0 ||
       strcmp(label, "bne") == 0 || strcmp(label, "red") == 0 ||
       strcmp(label, "prn") == 0 || strcmp(label, "jsr") == 0 ||
       strcmp(label, "rts") == 0 || strcmp(label, "stop") == 0 ||
       strcmp(label, "data") == 0 || strcmp(label, "string") == 0 ||
       strcmp(label, "entry") == 0 || strcmp(label, "extern") == 0 ||
       *label == '@')
    {
        printf("Error: label is a reserved word");
        return TRUE;
    }

    /* Check if label is too long or too short */
    if (strlen(label) > MAX_LABEL_LENGTH)
    {
        printf("Error: label to long");
        return TRUE;
    }
    if (strlen(label) == 0)
    {
        printf("Error: label to short");
        return TRUE;
    }

    /* Check if label starts with a letter */
    if (!isalpha(label[i])) {
        printf("Error: label must start with a letter\n");
        return TRUE;
    }

    /* Check if label has invalid characters */
    for (i = 1; i < strlen(label) - 1; i++)
    {   
        if (!isalnum(label[i])) 
        {
            printf("Error: label has invalid characters\n");
            return TRUE;
        }
    }

    return FALSE;
}

boolean check_line_length(FILE* am_file, char* line_content)
{
    int c;

    /* checks if if full line was read */
    while (*line_content++ != '\0')
        if (*line_content == '\n')
            return FALSE;
    if ((c = getc(am_file)) == EOF || c == '\n')
        return FALSE;

    /* reads rest of line */
    while ((c = getc(am_file)) != '\n' || c == EOF);

    return TRUE;
}
