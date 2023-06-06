#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"
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

char* get_word(char** str)
{
    int i = 0;
    char* res = (char*)malloc_with_check((MAX_LABEL_LENGTH + 1) * sizeof(char));
    char* temp = *str;

    if (str == NULL)
        return NULL;

    while (*temp != '\0' && i <= MAX_LINE_LENGTH)
    {
        if (isspace(*temp))
        {
            temp++;
            continue;
        }

        res[i++] = *temp++;
    }

    res[i] = '\0';
    return res;
}

boolean end_of_string(char* str)
{
    if (*str == '\0' || *str == '\n')
        return TRUE;
    return FALSE;
}


boolean ignore_line(char* line)
{
    if (*line == '\n' || *line == ';')
        return TRUE;
    return FALSE;
}

char* get_opcode(char** str)
{
    int i = 0;
    char* temp = *str;
    char* token;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        i++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

char* get_operand(char** str)
{
    int i = 0;
    char* temp = *str;
    char* token;
    while (!isspace(*temp) && *temp != '\0')
    {
        if (*temp == ',')
            break;
        temp++;
        i++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

char* get_instruction(char** str)
{
    char* temp = ++ * str; /*skip '.'*/
    char* token;
    int i = 0;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        i++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

char* get_label(char** str)
{
    char* temp = *str;
    char* token;
    int i = 0;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        i++;
    }
    token = (char*)malloc_with_check(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}
