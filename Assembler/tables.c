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

boolean add_data_to_table(line_info* line, symbols_table_entry* symbol_table, data_table_entry* data_table, extern_entry* ext, entry_entry* ent, long* DC, boolean* error_in_code)
{
    extern_entry* temp_ext = ext;
    entry_entry* temp_ent = ent;
    char* data_to_extract = line->instruction_data;
    char* token = (char*)malloc_with_check(sizeof(char) * (MAX_LABEL_LENGTH + 1));
    int i = 0;
    long L = 0;
    symbol_data_types data_type = (symbol_data_types)malloc_with_check(sizeof(data_type));/*for label if exist*/

    skip_white_spaces(&data_to_extract);
    while (data_table->next != NULL)/*temp solution for not printing data table. fix, needs attention. to slow*/
        data_table = data_table->next;

    if (strcmp(line->instruction, "string") == 0)
    {
        data_type = TYPE_STRING;/*for label if exist*/
        if (*data_to_extract != '"')
        {
            printf("Error: string data must start with \"\n");
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
        data_table = data_table->next;
        data_table = (data_table_entry*)malloc_with_check(sizeof(data_table));
        reset_data(data_table);
        L++;

        data_to_extract++;/*skip "*/
        *DC += L;

        if (!end_of_string(data_to_extract))
        {
            printf("Error: extra character after string \"\n");
            return FALSE;
        }

        if (line->is_label && !*error_in_code)
            *error_in_code = !add_symbol_to_table(line, symbol_table, data_type, DC, L);

        return TRUE;
    }
    if (strcmp(line->instruction, "data") == 0)
    {
        data_type = TYPE_NUMBER;/*for label*/

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
                *error_in_code = TRUE;
                return FALSE;
            }
        }/*end of while reading numbers*/
        if (line->is_label && !*error_in_code)
            *error_in_code = !add_symbol_to_table(line, symbol_table, data_type, DC, L);

        DC += L;/*increases DC by the number of numbers in the instruction*/
        return TRUE;
    }
    if (strcmp(line->instruction, "entry") == 0 || strcmp(line->instruction, "extern") == 0)
    {
        while (!end_of_string(data_to_extract))/*more entry or extern labels to add*/
        {
            while (!isspace(*data_to_extract))/*getting next label*/
            {
                token[i++] = *data_to_extract++;
            }
            token[i] = '\0';
            skip_white_spaces(&data_to_extract);
            /*if(!end_of_string(data_to_extract))
            {
                printf("Error: extra characters after entry\n");
                return FALSE;
            }*/

            if (strcmp(line->instruction, "entry") == 0)
            {

                while (temp_ent->next != NULL)
                {
                    if (strcmp(temp_ent->name, token) == 0)
                    {
                        printf("Error: entry already exist\n");
                        return FALSE;
                    }
                    temp_ent = temp_ent->next;
                }
                temp_ent->name = token;
                temp_ent->address = *DC;
                temp_ent->next = (entry_entry*)malloc_with_check(sizeof(ent));
                reset_entry(temp_ent->next);
            }
            else/*add function .... fix*/
            {
                while (temp_ext->next != NULL)
                {
                    if (strcmp(temp_ext->name, token) == 0)
                    {
                        printf("Error: extern already exist\n");
                        return FALSE;
                    }
                    temp_ext = temp_ext->next;
                }
                temp_ext->name = token;
                temp_ext->address = *DC;
                temp_ext->next = (extern_entry*)malloc_with_check(sizeof(ext));
                reset_extern(temp_ext->next);
            }

        }/*end of while*/
    }
    return TRUE;
}

boolean add_symbol_to_table(line_info* line, symbols_table_entry* symbol_table, symbol_data_types data_type, long* DC, long L)
{
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
    symbol_table_ptr->data_type = data_type;
    symbol_table_ptr->address = *DC;
    symbol_table_ptr->L = L;
    symbol_table_ptr->name = line->label;
    symbol_table_ptr->next = (symbols_table_entry*)malloc_with_check(sizeof(symbol_table));
    symbol_table_ptr = symbol_table_ptr->next;
    reset_symbol(symbol_table_ptr);
    return TRUE;
}

