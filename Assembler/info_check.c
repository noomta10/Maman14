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
    while (*line_content++ != '\0')
    {
        if (*line_content == '\n')
            return FALSE;
    }
    if ((c = getc(am_file)) == EOF)
        return FALSE;

    /* reads rest of line */
    while ((c = getc(am_file)) != '\n' || c == EOF);

    return TRUE;
}

boolean bad_label(char* label)
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
        *label == '@')
    {
        printf("Error: label is a reserved word\n");
        return TRUE;
    }

    /* Check if label is too long or too short */
    if (strlen(label) > MAX_LABEL_LENGTH)
    {
        printf("Error: label to long\n");
        return TRUE;
    }
    if (strlen(label) == 0)
    {
        printf("Error: label to short\n");
        return TRUE;
    }

    /* Check if label starts with a letter */
    if (!isalpha(label[i])) {
        printf("Error: label must start with a letter\n");
        return TRUE;
    }

    /* Check if label has invalid characters */
    for (i = 1; i < strlen(label); i++)
    {
        if (!isalnum(label[i]))
        {
            printf("Error: label %s has invalid characters\n", label);
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

