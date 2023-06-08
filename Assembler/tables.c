#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#include "assembler.h"
#include "tables.h"
#include "first_pass.h"
#include "utils.h"
#include "string_handling.h"



void reset_line_info(line_info* line)
{
    line->label_flag = FALSE;
    line->instruction_flag = FALSE;
    line->data_flag = FALSE;
    line->comma_flag = FALSE;
    line->extra_chars_flag = FALSE;
    line->label = NULL;
    line->instruction = NULL;
    line->opcode = NULL;
    line->source_operand = NULL;
    line->target_operand = NULL;
    line->instruction_data = NULL;
}

void free_data_table(data_table_entry* head)
{
    data_table_entry* temp;
    
    /*if head is null, return*/
    if(!head)
        return;
    
    /*freeing all nodes*/
    while(head->next != NULL)
    {
        temp = head->next;
        free(head);
        head = temp;
    }    
}

void free_symbols_table(symbols_table_entry* head) {
    symbols_table_entry* temp;

    /*if head is null, return*/
    if (!head)
        return;
    
    /*freeing all nodes*/
    while (head->next != NULL) {
        free(head->name);
        temp = head->next;
        free(head);
        head = temp;
    }
}

void free_extern_table(extern_entry* head) {
    extern_entry* temp;
    /*if head is null, return*/
    if (!head)
        return;

    /*freeing all nodes*/
    while (head->next != NULL) {
        free(head->name);
        temp = head->next;
        free(head);
        head = temp;
    }
}

void free_entry_table(entry_entry* head) {
    entry_entry* temp;
    /*if head is null, return*/
    if (!head)
        return;
    while (head->next != NULL) {
        free(head->name);
        temp = head->next;
        free(head);
        head = temp;
    }
}

boolean add_data_to_table(line_info* line, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head, extern_entry** ext_head, entry_entry** ent_head, long* DC)
{
    /*declaring pointers*/
    extern_entry* ext_ptr = *ext_head;
    extern_entry* ext_prev = NULL;

    entry_entry* ent_ptr = *ent_head;
    entry_entry* ent_prev = NULL;

    symbols_table_entry* symbol_table_ptr = *symbol_table_head;
    symbols_table_entry* symbol_table_prev = NULL;

    data_table_entry* data_table_ptr = *data_table_head;
    data_table_entry* data_table_prev = NULL;

    /*declaring variables*/
    char* data_to_extract = line->instruction_data;
    char* token;
    int i = 0;
    long L = 0;
    symbol_data_types data_type = DEFAULT;
    boolean error_flag = FALSE;
    
    /*instruction is a string*/    
    skip_white_spaces(&data_to_extract);
    if (strcmp(line->instruction, "string") == 0)
    {
        /*goto the end of the list*/
        while(data_table_ptr != NULL)
            data_table_ptr = data_table_ptr->next;
            
        token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));

        /*check if string starts with " and skip it"*/
        if (*data_to_extract != '"')
        {
            printf("Error: string data must start and end with \"\n");
            free(token);
            return FALSE;
        }
        data_to_extract++;

        /*copying the string to data_table until " reached"*/
        while (*data_to_extract != '"' && !end_of_string(data_to_extract))
        {
            data_table_ptr = (data_table_entry*)malloc_with_check(sizeof(data_table_entry));
            data_table_ptr->type = TYPE_STRING;
            data_table_ptr->data.character = *data_to_extract++;
            ADD_NODE_TO_LIST(data_table_prev, data_table_ptr, data_table_head);

            L++;
        }

        /*adding \0 to the end of the string */
        data_table_ptr = (data_table_entry*)malloc_with_check(sizeof(data_table_entry));
        data_table_ptr->data.character = '\0';
        data_table_ptr->type = TYPE_STRING;
        ADD_NODE_TO_LIST(data_table_prev, data_table_ptr, data_table_head);
        L++;

        /*check if string ends with " and skip it, else print error*/
        if(*data_to_extract == '"')
            data_to_extract++;
        else
        {
            printf("Error: string data must start and end with \"\n");
            free(token);
            return FALSE;
        }

        /*check if there are extra characters after the string*/
        if (!end_of_string(data_to_extract))
        {
            printf("Error: extra character after string \"\n");
            error_flag = TRUE;
        }

        /*check if there is a label and add it*/
        if (line->label)
        {
            data_type = TYPE_STRING;
            if (!add_symbol_to_table(line, symbol_table_head, data_type, ext_head, DC, L))
                error_flag = TRUE;
        }
        *DC += L;

        return !error_flag;
    }

    /*instruction is numbers*/
    if (strcmp(line->instruction, "data") == 0)
    {
        /*goto the end of the list*/
        while (data_table_ptr != NULL)
            data_table_ptr = data_table_ptr->next;
        token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));

        /*reading numbers*/
        while (!end_of_string(data_to_extract))
        {
            /*read the number*/
            if (*data_to_extract == '-' || *data_to_extract == '+')/*check for + or - sign*/
                token[i++] = *data_to_extract++;
            while (isdigit(*data_to_extract))
                token[i++] = *data_to_extract++;
            token[i] = '\0';

            /*checking for errors in the number*/
            if (strlen(token) == 0 || strcmp(token, "-") == 0 || strcmp(token, "+") == 0)
            {
                printf("Error: missing number\n");
                return FALSE;
            }

            /*adding the number to the data table*/
            data_table_ptr = (data_table_entry*)malloc_with_check(sizeof(data_table_entry));
            data_table_ptr->type = TYPE_NUMBER;
            data_table_ptr->data.number = atoi(token);
            ADD_NODE_TO_LIST(data_table_prev, data_table_ptr, data_table_head);
            L++;

            /*resetting the token*/
            i = 0;
            reset_str(token);

            /*check for comma and extra characters after the number*/
            skip_white_spaces(&data_to_extract);
            if (*data_to_extract == ',')
            {
                data_to_extract++;
                skip_white_spaces(&data_to_extract);
            }
            else if (!end_of_string(data_to_extract))
            {
                printf("Error: expected a comma between numbers\n");
                error_flag = TRUE;
            }
        }/*end of reading numbers*/

        /*check if there is a label and add it*/
        if (line->label && !error_flag)
            error_flag = !add_symbol_to_table(line, symbol_table_head, data_type, ext_head, DC, L);
        DC += L;

        free(token);
        return !error_flag;
    }

    /*instruction is entry or extern*/
    if (strcmp(line->instruction, "entry") == 0 || strcmp(line->instruction, "extern") == 0)
    {
        /*adding all lables*/
        while (!end_of_string(data_to_extract))
        {
            /*getting next label*/
            token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));
            while (!isspace(*data_to_extract))
            {
                token[i++] = *data_to_extract++;
            }
            token[i] = '\0';
            skip_white_spaces(&data_to_extract);
            
            /*if instruction is entry type, add the lable to entry table*/
            if (strcmp(line->instruction, "entry") == 0)
            {
                /*check if the lable already exist in entry or extern*/
                while (ext_ptr != NULL)
                {
                    if (strcmp(ext_ptr->name, token) == 0)
                    {
                        printf("Error: `%s` already diffined as extern\n", token);
                        free(token);
                        error_flag = TRUE;
                        goto SKIP_ADDING_ENTRY_LABLE;
                    }
                    ext_ptr = ext_ptr->next;
                }
                while(ent_ptr != NULL)
                {
                    if (strcmp(ent_ptr->name, token) == 0)
                    {
                        printf("Error: the lable `%s` already exist in entry\n", token);
                        free(token);
                        error_flag = TRUE;
                        goto SKIP_ADDING_ENTRY_LABLE;
                    }
                    ent_ptr = ent_ptr->next;
                }

                /*add the lable to entry table*/
                ent_ptr = malloc_with_check(sizeof(entry_entry));
                ent_ptr->name = token;
                ent_ptr->address = 0;
                ADD_NODE_TO_LIST(ent_prev, ent_ptr, ent_head);

                /*reseting for next use*/
                SKIP_ADDING_ENTRY_LABLE:
                symbol_table_ptr = *symbol_table_head;
                ext_ptr = *ext_head;
                ent_ptr = *ent_head;
                i = 0;

                /*if there is a label before entry, print warning*/
                if (line->label)
                    printf("Warning: label before .entry is undefined\n");
            }
            else/*type extern*/
            {
                /*check if the lable already exist*/
                while (ext_ptr != NULL)
                {
                    if (strcmp(ext_ptr->name, token) == 0)
                    {
                        printf("Error: double declaration of `%s` as extern\n", token);
                        free(token);
                        error_flag = TRUE;
                        goto NOT_ADD_EXTERN_LABLE;

                    }
                    ext_ptr = ext_ptr->next;
                }

                /*check if lable already defined as symbol*/
                while(symbol_table_ptr != NULL)
                {
                    if (strcmp(symbol_table_ptr->name, token) == 0)
                    {
                        printf("Error: the lable `%s` already exist in local file\n", token);
                        free(token);
                        error_flag = TRUE;
                        goto NOT_ADD_EXTERN_LABLE;
                    }
                    symbol_table_ptr = symbol_table_ptr->next;
                }

                /*check if lable already defined as entry*/
                while(ent_ptr != NULL)
                {
                    if (strcmp(ent_ptr->name, token) == 0)
                    {
                        printf("Error: the lable `%s` already exist in entry\n", token);
                        free(token);
                        error_flag = TRUE;
                        goto NOT_ADD_EXTERN_LABLE;
                    }
                    ent_ptr = ent_ptr->next;
                }
                
                /*add the lable to extern table*/
                ext_ptr = malloc_with_check(sizeof(extern_entry));
                ext_ptr->name = token;
                ext_ptr->address = 0;
                ADD_NODE_TO_LIST(ext_prev, ext_ptr, ext_head);
                i = 0;

                /*reseting for next use*/
                NOT_ADD_EXTERN_LABLE:
                ext_ptr = *ext_head;
                ent_ptr = *ent_head;
                symbol_table_ptr = *symbol_table_head;

                /*if there is a label before extern, print warning*/
                if (line->label)
                    printf("Warning: label before .extern is undefined\n");
            }

        }/*end of while*/
    }
    return !error_flag;
}

boolean add_symbol_to_table(line_info* line, symbols_table_entry** symbol_table_head, symbol_data_types data_type_head, extern_entry** ext_head, long* DC, long L)
{
    extern_entry* ext_ptr = *ext_head;
    symbols_table_entry* symbol_table_ptr = *symbol_table_head;
    symbols_table_entry* symbol_table_prev = NULL;

    
    while (symbol_table_ptr != NULL)
    {
        if (strcmp(symbol_table_ptr->name, line->label) == 0)/*checking if double declaration of label*/
        {
            printf("Error: label already exist");
            return FALSE;
        }
        symbol_table_ptr = symbol_table_ptr->next;
    }
    while(ext_ptr != NULL)
    {
        if (strcmp(ext_ptr->name, line->label) == 0)/*checking if lable declared alredy as extern*/
        {
            printf("Error: label already declared as extern");
            return FALSE;
        }
        ext_ptr = ext_ptr->next;
    }

    /*adding the label to symbol table*/
    symbol_table_ptr = malloc_with_check(sizeof(symbols_table_entry));
    symbol_table_ptr->address = *DC;
    symbol_table_ptr->L = L;
    symbol_table_ptr->name = line->label;
    ADD_NODE_TO_LIST(symbol_table_prev, symbol_table_ptr, symbol_table_head);

    
    return TRUE;
}

