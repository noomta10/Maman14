#include "first_pass.h"
#include "utils.h"
#include "tables.h"


boolean first_pass(FILE *am_file, symbols_table_entry* symbol_table, data_table_entry* data_table, entry_entry* ent, extern_entry* ext, long* IC, long* DC) /*processes file*/
{
    char line_content[MAX_LINE_LENGTH];
    line_info* line = (line_info*)malloc_with_check(sizeof(line_info));
    boolean* error_in_code = (boolean*)malloc_with_check(sizeof(error_in_code));
    //*DC = *IC = 0;/*end of page 48*/
    *error_in_code = FALSE;
    reset_line_info(line); /*initializing line*/

    while (fgets(line_content, MAX_LINE_LENGTH, am_file) != NULL) /*reading .as file line by line*/
    {
        if (ignore_line(line_content))
        { /*empty or command line*/
            printf("debug: empty or command line\n");
            continue;
        }
        if (!extract_command_info(line_content, line))/*extracting line info*/
        {
            *error_in_code = TRUE;
        }
        process_line_first_pass(line, IC, DC, symbol_table, data_table, ent, ext, error_in_code); /*processing line*/

        reset_line_info(line);                                    /*freeing line memory*/
        reset_str(line_content);               /*clearing line content*/
    }


    /*freeing memory*/
    free(line);
    free(error_in_code); 

    /*create tables and files*/

    return !*error_in_code; /*return true if code is good otherwise returns false*/
}

/*processes line*/
void process_line_first_pass(line_info* line, long* IC, long* DC, symbols_table_entry* symbol_table, data_table_entry* data_table,
    entry_entry* ent, extern_entry* ext, boolean* error_in_code)
{
    static int line_number = 0;
    line_number++;
    //boolean label_exists;

    printf("debug: line number: %d\n", line_number);

    if (!validate_line(line)) /*validating line*/
    {
        printf("debug: invalid line\n");
        *error_in_code = TRUE;
    }

    if (line->is_instruction) {/*if line is data. fix, is data*/
        if (!add_data_to_table(line, symbol_table, data_table, ext, ent, DC, error_in_code))
            *error_in_code = TRUE;
    }


    /*continue here */
}

void reset_str(char* str) /*clears string*/
{
    while (*str != '\0')
        *str++ = '\0';
}

void reset_line_info(line_info* line)
{
    line->is_label = FALSE;
    line->is_instruction = FALSE;
    line->is_data = FALSE;
    line->comma = FALSE;
    line->label = NULL;
    line->instruction = NULL;
    line->opcode = NULL;
    line->source_operand = NULL;
    line->target_operand = NULL;
    line->instruction_data = NULL;
}

boolean validate_line(line_info* line) /*validates line*/
{
    if (line->is_label) /*checking label*/
    {
        if (bad_label(line->label))
            return FALSE;
    }
    if (line->is_instruction) /*checking instruction*/
    {
        if (!check_instruction(line->instruction))
            return FALSE;
    }
    else if (!line->is_instruction) /*command line*/
    {
        if (strcmp(line->opcode, "sub") == 0 || /*checking for missing operand in sub, mov, add, lea*/
            strcmp(line->opcode, "mov") == 0 ||
            strcmp(line->opcode, "add") == 0 ||
            strcmp(line->opcode, "lea") == 0)
        {
            if (line->source_operand == NULL || line->target_operand == NULL)
            {
                printf("Error: missing operand\n");
                return FALSE;
            }
        }
        else if (strcmp(line->opcode, "not") == 0 ||
            strcmp(line->opcode, "clr") == 0 ||
            strcmp(line->opcode, "inc") == 0 ||
            strcmp(line->opcode, "dec") == 0 ||
            strcmp(line->opcode, "jmp") == 0 ||
            strcmp(line->opcode, "bne") == 0 ||
            strcmp(line->opcode, "red") == 0 ||
            strcmp(line->opcode, "prn") == 0 ||
            strcmp(line->opcode, "jsr") == 0)
        { /*checking for missing operand*/
            if (line->source_operand == NULL)
            {
                printf("Error: missing operand\n");
                return FALSE;
            }
            else if (line->target_operand != NULL && strcmp(line->target_operand, "") != 0)
            { /*checking for extra operand*/
                printf("Error: extra operand\n");
                return FALSE;
            }
            line->target_operand = line->source_operand; /*moving source operand to target operand*/
            line->source_operand = NULL;
        }
        else if (strcmp(line->opcode, "rts") == 0 ||
            strcmp(line->opcode, "stop") == 0)
        {
            if ((line->source_operand != NULL && strcmp(line->source_operand, "")) ||
                (line->target_operand != NULL && strcmp(line->target_operand, "")))
            { /*checking for extra operand*/
                printf("Error: extra operand\n");
                return FALSE;
            }
        }
        else
        {
            printf("Error: invalid opcode\n");
            return FALSE;
        }
    }
    return TRUE;
}

boolean extract_command_info(char* content, line_info* line)
{

    char* token;

    skip_white_spaces(&content);
    if (is_label(content)) /*check for label*/
    {
        token = get_label(&content);
        line->is_label = TRUE;
        line->label = token;
    }
    else
    {
        line->is_label = FALSE;
    }

    skip_white_spaces(&content);

    if (is_instruction(content)) /*check if instruction sentence*/
    {
        token = get_instruction(&content);
        line->is_instruction = TRUE;
        line->instruction = token;
        line->instruction_data = content; /*instruction data unchecked*/
        return TRUE;
    }
    else
    {
        line->is_instruction = FALSE;
    }

    skip_white_spaces(&content);

    token = get_opcode(&content); /*get opcode*/
    if (token != NULL)
        line->opcode = token;
    else
    {
        printf("Error: invalid opcode\n");
        return FALSE;
    }

    skip_white_spaces(&content);

    token = get_operand(&content); /*get source operand*/
    if (token != NULL)
        line->source_operand = token;
    else
        return TRUE;

    skip_white_spaces(&content);
    line->comma = check_comma(&content); /*check for comma*/
    skip_white_spaces(&content);

    token = get_operand(&content); /*get target operand*/
    if (token != NULL)
        line->target_operand = token;
    else
        return TRUE;

    skip_white_spaces(&content);

    if (!end_of_string(content)) /*check for extra operands*/
    {
        printf("Error: extra operand\n");
        return FALSE;
    }

    return TRUE;
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

boolean end_of_string(char* str)
{
    if (*str == '\0' || *str == '\n')
        return TRUE;
    return FALSE;
}

void skip_white_spaces(char** str)
{
    char* temp = *str;
    while (*temp == ' ' || *temp == '\t') /*skip  white space*/
        temp++;
    *str = temp;
}

boolean check_instruction(char* str)
{
    if (strcmp(str, "data") || strcmp(str, "string") ||
        strcmp(str, "entry") || strcmp(str, "extern"))
        return TRUE;
    return FALSE;
}

boolean ignore_line(char* line)
{
    if (*line == '\n' || *line == ';')
        return TRUE;
    return FALSE;
}

boolean bad_label(char* str)
{
    if (strlen(str) > MAX_LABEL_LENGTH) /*label to long*/
    {
        printf("Error: label to long");
        return TRUE;
    }
    if (strlen(str) == 0) /*label to short. can happen?? fix*/
    {
        printf("Error: label to short");
        return TRUE;
    }
    return FALSE;
}

char* get_word(char** str)
{
    int i = 0;
    char* res = (char*)malloc_with_check(MAX_LINE_LENGTH);
    char* temp = *str;

    if (str == NULL)
        return NULL;

    while (*temp != '\0' && i < MAX_LINE_LENGTH)
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



