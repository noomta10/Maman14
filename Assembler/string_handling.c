#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "assembler.h"
#include "line_info.h"
#include "string_handling.h"
#include "utils.h"


void reset_str(char* str) /*clears string*/
{
    while (*str != '\0')
        *str++ = '\0';
}

void skip_white_spaces(char** str)
{
    char* temp = *str;
    while (*temp == ' ' || *temp == '\t') /*skip  white space*/
        temp++;
    *str = temp;
}

char* copy_next_word(char* string)
{
    int word_length = 0;
    char* result;
    char* string_ptr = string;

    /* get the length of the next word */
    while (!isspace(*string_ptr))
    {
        word_length++;
        string_ptr++;
    }

    result = (char*)malloc_with_check(sizeof(char) * (word_length + 1));
    strncpy(result, string, word_length);
    result[word_length] = '\0';

    return result;    
}

char* copy_string(char* string)
{
    char* result = malloc_with_check(sizeof(string)* (strlen(string) + 1));
    strcpy(result, string);
    return result;
}

boolean end_of_string(char* str)
{
    skip_white_spaces(&str);
    if (*str == '\0' || *str == '\n')
        return TRUE;
    return FALSE;
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

char* get_opcode(char** str)
{
    int opcode_length = 0;
    char* temp = *str;
    char* token;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        opcode_length++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (opcode_length +1));
    strncpy(token, *str, opcode_length);
    token[opcode_length] = '\0';
    *str = temp;
    return token;
}

char* get_operand(char** str)
{
    int operand_length = 0;
    char* temp = *str;
    char* token;
    while (!isspace(*temp) && *temp != '\0')
    {
        if (*temp == ',')
            break;
        temp++;
        operand_length++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (operand_length + 1));
    strncpy(token, *str, operand_length);
    token[operand_length] = '\0';
    *str = temp;
    return token;
}

char* get_directive(char** str)
{
    char* temp = ++ * str; /*skip '.'*/
    char* token;
    int directive_length = 0;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        directive_length++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (directive_length + 1));
    strncpy(token, *str, directive_length);
    token[directive_length] = '\0';
    *str = temp;
    return token;
}

char* get_label(char** str)
{
    char* temp = *str;
    char* token;
    int label_length = 0;

        printf("get_label\n");

    
    while (*temp != ':')
    {
        temp++;
        label_length++;
    }
    
    token = (char*)malloc_with_check(sizeof(char) * (label_length + 1));
    printf("token: %p\n", token);

    strncpy(token, *str, label_length);
    token[label_length] = '\0';
    *str = ++temp; /* and skip : */

    return token;
}
