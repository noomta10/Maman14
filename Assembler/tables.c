#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#include "assembler.h"
#include "line_info.h"

#include "tables.h"
#include "first_pass.h"
#include "utils.h"
#include "string_handling.h"



void reset_line_info(line_info* line)
{
    line->line_content = NULL; /* needs to be freed but receves error */
    line->label = NULL;
    line->directive_data = NULL;
    line->directive_command = NULL;
    line->opcode = NULL;
    line->source_operand = NULL;
    line->target_operand = NULL;
    line->comma_flag = FALSE;
    line->label_flag = FALSE;
    line->directive_flag = FALSE;
    line->instruction_flag = FALSE;
    line->extra_chars_flag = FALSE;
}

void free_data_table(data_table_entry* head)
{
    data_table_entry* temp;
    printf("free data table\n");
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
    symbols_table_entry* next_entry;
    printf("Debug: freeing symbols table\n");
    FREE_TABLE(head, next_entry);
}

void free_extern_table(extern_entry* head) {
    extern_entry* next_entry;
    printf("Debug: freeing extern table\n");
    FREE_TABLE(head, next_entry);
}

void free_entry_table(entry_entry* head) {
    entry_entry* next_entry;
    printf("Debug: freeing entry table\n");
    FREE_TABLE(head, next_entry);

}

boolean add_data_to_table(line_info* line, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head, extern_entry** ext_head, entry_entry** ent_head, long* DC)
{
    /*declaring pointers*/
    extern_entry* ext_ptr = *ext_head;
    extern_entry* ext_prev = NULL;

    entry_entry* ent_ptr = *ent_head;
    entry_entry* ent_prev = NULL;

    data_table_entry* data_table_ptr = *data_table_head;
    data_table_entry* data_table_prev = NULL;

    symbols_table_entry* symbol_table_ptr = *symbol_table_head;

    /*declaring variables*/
    char* data_to_extract = line->directive_data;
    char* token;
    int i = 0;
    long L = 0;
    data_types data_type = TYPE_STRING;
    boolean error_flag = FALSE;

    /*setting prev pointers to the end*/
    SET_PREV_POINTER(ext_prev, ext_ptr)
    SET_PREV_POINTER(ent_prev, ent_ptr)
    SET_PREV_POINTER(data_table_prev, data_table_ptr)
    
    /*instruction is a string*/    
    skip_white_spaces(&data_to_extract);
    if (strcmp(line->directive_command, "string") == 0)
    {
        /*goto the end of the list*/
        while(data_table_ptr != NULL)
            data_table_ptr = data_table_ptr->next;
            
        /*check if string starts with " and skip it"*/
        if (*data_to_extract != '"')
        {
            printf("Error: string data must start and end with \"\n");
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
            if (!add_symbol_to_table(line->label, symbol_table_head, ext_head, data_type, DC, L))
                error_flag = TRUE;
        }
        *DC += L;

        return !error_flag;
    }

    /*instruction is numbers*/
    else if (strcmp(line->directive_command, "data") == 0)
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
            error_flag = !add_symbol_to_table(line->label, symbol_table_head, ext_head, data_type, DC, L);
        DC += L;

        free(token);
        return !error_flag;
    }

    /* directive is entry or extern */
    else if (strcmp(line->directive_command, "entry") == 0 || strcmp(line->directive_command, "extern") == 0)
    {
        /* getting next label */
        token = copy_next_word(data_to_extract);
        
        /* checking for extra characters after stirng */
        data_to_extract += strlen(token);
        if (!end_of_string(data_to_extract))
        {
            printf("Warning: in file %s line %ld %s extra characters after label.\n", line->file_name, line->line_number, line->line_content);
            error_flag = TRUE;
        }
            
        /*if instruction is entry type, add the lable to entry table*/
        if (strcmp(line->directive_command, "entry") == 0)
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
            ent_ptr = (entry_entry*)malloc_with_check(sizeof(entry_entry));
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
                printf("Warning: ignoring label before .entry\n");
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
                printf("Warning: ignoring label before .extern\n");
        }
    }

    else
    {
        printf("Error: in line %ld %s\n undefined directive command\n", line->line_number, line->line_content);
        error_flag = TRUE;
    }
    return !error_flag;
}

boolean add_instruction_to_table(line_info* line, symbols_table_entry** symbol_trable_head, extern_entry** ext_head, code_table_entry** code_table_head, long* IC)
{
    code_table_entry* code_table_temp = *code_table_head;
    code_table_entry* code_table_prev = NULL;
    

    boolean error_flag = FALSE;
    long L = 1;    

    SET_PREV_POINTER(code_table_prev, code_table_temp);
   
    code_table_temp = (code_table_entry*)malloc_with_check(sizeof(code_table_entry));

    
    /*adding the word to the code table*/
    code_table_temp->word.opcode = get_opcode_bits(line->opcode);
    code_table_temp->word.ARE = ABSOLUTE;
    code_table_temp->word.source_addressing = get_addressing_type(line->source_operand);
    code_table_temp->word.target_addressing = get_addressing_type(line->source_operand);
    code_table_temp->address = *IC;
    ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
    
    L += extra_words_for_addressing(line);

    if(line->label_flag)
        if(!add_symbol_to_table(line->label, symbol_trable_head, ext_head, INSTRUCTION, IC, L))
            error_flag = TRUE;

    *IC += L;

    return !error_flag;
}

int extra_words_for_addressing(line_info* line)
{
    if (line->source_operand == NULL && line->target_operand == NULL)
        return 0;
    if(line->source_operand == NULL || line->target_operand == NULL)
        return 1;
    if(is_register(line->source_operand) == 0 && is_register(line->target_operand) == 0)
        return 1;
    return 2;
}

boolean add_symbol_to_table(char* lable_name, symbols_table_entry** symbol_table_head, extern_entry** ext_head, line_type type, long* address, long L)
{
    extern_entry* ext_ptr = *ext_head;
    symbols_table_entry* symbol_table_ptr = *symbol_table_head;
    symbols_table_entry* symbol_table_prev = NULL;

    SET_PREV_POINTER(symbol_table_prev, symbol_table_ptr)
    
    while (symbol_table_ptr != NULL)
    {
        if (strcmp(symbol_table_ptr->name, lable_name) == 0)/*checking if double declaration of label*/
        {
            printf("Error: label already exist\n");
            return FALSE;
        }
        symbol_table_ptr = symbol_table_ptr->next;
    }
    while(ext_ptr != NULL)
    {
        if (strcmp(ext_ptr->name, lable_name) == 0)/*checking if lable declared alredy as extern*/
        {
            printf("Error: label already declared as extern\n");
            return FALSE;
        }
        ext_ptr = ext_ptr->next;
    }

    /*adding the label to symbol table*/
    symbol_table_ptr = (symbols_table_entry*)malloc_with_check(sizeof(symbols_table_entry));
    symbol_table_ptr->address_type = type;
    symbol_table_ptr->address = *address;
    symbol_table_ptr->L = L;
    symbol_table_ptr->name = lable_name;

    ADD_NODE_TO_LIST(symbol_table_prev, symbol_table_ptr, symbol_table_head);

    
    return TRUE;
}

