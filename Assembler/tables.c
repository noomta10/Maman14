#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#include "assembler.h"
#include "tables.h"
#include "first_pass.h"
#include "utils.h"
#include "string_handling.h"



void reset_entry(entry_entry* ent)
{
    ent->address = 0;
    ent->name = NULL;
    ent->next = NULL;
}

void reset_extern(extern_entry* ext)
{
    ext->address = 0;
    ext->name = NULL;
    ext->next = NULL;
}

void reset_symbol(symbols_table_entry* symbol)
{
    symbol->address = 0;
    symbol->data_type = 0;
    symbol->L = 0;
    symbol->name = NULL;
    symbol->next = NULL;
}

void reset_data(data_table_entry* data)
{
    data->type = 0;
    data->data.character = '\0';
    data->data.number = 0;
    data->next = NULL;
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

void free_data_table(data_table_entry* data_table)
{
    data_table_entry* temp;
    while (data_table->next != NULL)
    {
        temp = data_table->next;
        free(data_table);
        data_table = temp;
    }
}

void free_symbols_table(symbols_table_entry* symbol_table) {
    symbols_table_entry* temp;
    while (symbol_table->next != NULL) {
        free(symbol_table->name);
        temp = symbol_table->next;
        free(symbol_table);
        symbol_table = temp;
    }
}

void free_extern_table(extern_entry* ext) {
    extern_entry* temp;
    while (ext->next != NULL) {
        free(ext->name);
        temp = ext->next;
        free(ext);
        ext = temp;
    }
}

void free_entry_table(entry_entry* ent) {
    entry_entry* temp;
    while (ent->next != NULL) {
        free(ent->name);
        temp = ent->next;
        free(ent);
        ent = temp;
    }
}

boolean add_data_to_table(line_info* line, symbols_table_entry* symbol_table, data_table_entry* data_table, extern_entry* ext, entry_entry* ent, long* DC)
{
    extern_entry* ext_ptr = ext;/*temperery pointers are also changing original value*/
    entry_entry* ent_ptr = ent;
    symbols_table_entry* symbol_table_ptr = symbol_table;
    data_table_entry* data_table_ptr = data_table;
    char* data_to_extract = line->instruction_data;
    char* token;
    int i = 0;
    long L = 0;
    symbol_data_types data_type = DEFAULT; /* = (line->is_label) ? (symbol_data_types)malloc_with_check(sizeof(data_type)) : NULL;/*for label if exist*/
    boolean error_in_code = FALSE;
    
    skip_white_spaces(&data_to_extract);
    while (data_table_ptr->next != NULL)/*temp solution for not printing data table. fix, needs attention. to slow*/
        data_table_ptr = data_table_ptr->next;

    if (strcmp(line->instruction, "string") == 0)
    {
        token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));
        if (*data_to_extract != '"')
        {
            printf("Error: string data must start and end with \"\n");
            free(token);
            return FALSE;
        }
        data_to_extract++;/*skip "*/
        while (*data_to_extract != '"')
        {
            data_table->type = TYPE_STRING;
            data_table->data.character = *data_to_extract++;
            data_table->next = (data_table_entry*)malloc_with_check(sizeof(data_table));
            data_table = data_table->next;
            reset_data(data_table);
            L++;
        }
        data_table->data.character = '\0';
        data_table->type = TYPE_STRING;
        data_table->next = (data_table_entry*)malloc_with_check(sizeof(data_table));
        data_table = data_table->next;

        reset_data(data_table);
        L++;

        data_to_extract++;/*skip "*/

        if (!end_of_string(data_to_extract))
        {
            printf("Error: extra character after string \"\n");
            error_in_code = TRUE;
        }

        if (line->is_label)/*add label to table*/
        {
            data_type = TYPE_STRING;
            if (!add_symbol_to_table(line, symbol_table, data_type, ext, DC, L))
                error_in_code = TRUE;
        }
        *DC += L;

        return TRUE;
    }
    if (strcmp(line->instruction, "data") == 0)
    {
        token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));

        while (!end_of_string(data_to_extract))/*reading numbers until end of line reached*/
        {
            if (*data_to_extract == '-' || *data_to_extract == '+')/*check for + or - sign*/
                token[i++] = *data_to_extract++;

            while (isdigit(*data_to_extract))/*copy the number*/
                token[i++] = *data_to_extract++;
            token[i] = '\0';

            if (strlen(token) == 0 || strcmp(token, "-") == 0 || strcmp(token, "+") == 0)/*error checking no number, empty string*/
            {
                printf("Error: missing number\n");
                return FALSE;
            }
            data_table->type = TYPE_NUMBER;
            data_table->data.number = atoi(token);
            data_table->next = (data_table_entry*)malloc_with_check(sizeof(data_table));
            data_table = data_table->next;
            reset_data(data_table);


            L++;
            i = 0;
            reset_str(token);

            skip_white_spaces(&data_to_extract);
            if (*data_to_extract == ',')
            {
                data_to_extract++;
                skip_white_spaces(&data_to_extract);
            }
            else if (!end_of_string(data_to_extract))
            {
                printf("Error: expected a comma between numbers\n");
                error_in_code = TRUE;
                return FALSE;
            }
        }/*end of while reading numbers*/
        if (line->is_label && !error_in_code)
            error_in_code = !add_symbol_to_table(line, symbol_table, data_type, ext, DC, L);

        if (line->is_label)/*add label to table*/
        {
            data_type = TYPE_NUMBER;
            if (!add_symbol_to_table(line, symbol_table, data_type, ext, DC, L))
                error_in_code = TRUE;
        }
        DC += L;/*increases DC by the number of numbers in the instruction*/
        return TRUE;
    }
    if (strcmp(line->instruction, "entry") == 0 || strcmp(line->instruction, "extern") == 0)
    {
        while (!end_of_string(data_to_extract))/*more entry or extern labels to add*/
        {
            token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));
            while (!isspace(*data_to_extract))/*getting next label*/
            {
                token[i++] = *data_to_extract++;
            }
            token[i] = '\0';
            skip_white_spaces(&data_to_extract);
            
            if (strcmp(line->instruction, "entry") == 0)
            {
                while (ext_ptr->next != NULL)
                {
                    if (strcmp(ext_ptr->name, token) == 0)
                    {
                        printf("Error: `%s` already diffined as extern\n", token);
                        free(token);
                        error_in_code = TRUE;
                        goto NOT_ADD_ENTRY_LABLE;
                    }
                    ext_ptr = ext_ptr->next;
                }
                while(ent_ptr->next != NULL)
                {
                    if (strcmp(ent_ptr->name, token) == 0)
                    {
                        printf("Error: the lable `%s` already exist in entry\n", token);
                        free(token);
                        error_in_code = TRUE;
                        goto NOT_ADD_ENTRY_LABLE;
                    }
                    ent_ptr = ent_ptr->next;
                }
                ent_ptr->name = token;
                ent_ptr->address = *DC++;
                ent_ptr->next = (entry_entry*)malloc_with_check(sizeof(ent));
                reset_entry(ent_ptr->next);

                NOT_ADD_ENTRY_LABLE:
                symbol_table_ptr = symbol_table;
                ext_ptr = ext;
                ent_ptr = ent;
                i = 0;
                if (line->label)
                    printf("Warning: label before .entry is undefined\n");
            }
            else/*type extern*/
            {
                while (ext_ptr->next != NULL)
                {
                    if (strcmp(ext_ptr->name, token) == 0)
                    {
                        printf("Error: double declaration of `%s` as extern\n", token);
                        free(token);
                        error_in_code = TRUE;
                        goto NOT_ADD_EXTERN_LABLE;

                    }
                    ext_ptr = ext_ptr->next;
                }
                while(symbol_table_ptr->next != NULL)
                {
                    if (strcmp(symbol_table_ptr->name, token) == 0)
                    {
                        printf("Error: the lable `%s` already exist in local file\n", token);
                        free(token);
                        error_in_code = TRUE;
                        goto NOT_ADD_EXTERN_LABLE;
                    }
                    symbol_table = symbol_table->next;
                }
                while(ent_ptr->next != NULL)
                {
                    if (strcmp(ent_ptr->name, token) == 0)
                    {
                        printf("Error: the lable `%s` already exist in entry\n", token);
                        free(token);
                        error_in_code = TRUE;
                        goto NOT_ADD_EXTERN_LABLE;
                    }
                    ent_ptr = ent_ptr->next;
                }
                ext_ptr->name = token;
                ext_ptr->address = *DC;
                ext_ptr->next = (extern_entry*)malloc_with_check(sizeof(ext));
                reset_extern(ext_ptr->next);
                i = 0;

                NOT_ADD_EXTERN_LABLE:
                ext_ptr = ext;
                ent_ptr = ent;
                symbol_table_ptr = symbol_table;
                if (line->is_label)
                    printf("Warning: label before .extern is undefined\n");

            }

        }/*end of while*/
    }
    return !error_in_code;
}

boolean add_symbol_to_table(line_info* line, symbols_table_entry* symbol_table, symbol_data_types data_type, extern_entry* ext, long* DC, long L)
{
    extern_entry* ext_ptr = ext;
    symbols_table_entry* symbol_table_ptr = symbol_table;

    while (symbol_table_ptr->next != NULL)
    {
        if (strcmp(symbol_table->name, line->label) == 0)/*checking if double declaration of label*/
        {
            printf("Error: label already exist");
            return FALSE;
        }
        symbol_table_ptr = symbol_table_ptr->next;
    }
    while(ext_ptr->next != NULL)
    {
        if (strcmp(ext_ptr->name, line->label) == 0)/*checking if lable declared alredy as extern*/
        {
            printf("Error: label already declared as extern");
            return FALSE;
        }
        ext_ptr = ext_ptr->next;
    }
    symbol_table_ptr->data_type = data_type;
    symbol_table_ptr->address = *DC;
    symbol_table_ptr->L = L;
    symbol_table_ptr->name = line->label;
    symbol_table_ptr->next = (symbols_table_entry*)malloc_with_check(sizeof(symbol_table));
    symbol_table_ptr = symbol_table_ptr->next;
    reset_symbol(symbol_table_ptr);
    return TRUE;
}

