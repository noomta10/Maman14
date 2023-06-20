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
#include "info_check.h"



void reset_line_info(line_info* line)
{
    line->line_content = NULL; /* TODO: needs to be freed but receves error */
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

void free_code_table(code_table_entry* head){
    code_table_entry* next_entry;
    printf("Debug: freeing code table\n");
    while (head != NULL)
    {
        next_entry = head->next;
        free(head);
        head = next_entry;
    }
}

boolean add_data_to_table(line_info* line, symbols_table_entry** symbol_table_head, data_table_entry** data_table_head, extern_entry** ext_head, entry_entry** ent_head, long* DC, long* IC)
{   
    /* directive command is string */    
    if (strcmp(line->directive_command, "string") == 0)
        return add_string_to_table(line, data_table_head, symbol_table_head, *ext_head, DC);


    /* directive command is data */
    if (strcmp(line->directive_command, "data") == 0)
        return add_number_to_table(line, data_table_head, symbol_table_head, *ext_head, DC);

    /* directive command is entry */
    if (strcmp(line->directive_command, "entry") == 0)
        return add_entry_to_table(line, *symbol_table_head, *ext_head, ent_head);

    /* directive command is extern */
    if (strcmp(line->directive_command, "extern") == 0)
        return add_extern_to_table(line, *symbol_table_head, ext_head, *ent_head);

    printf("debug: need to check if directive command is string, data, entry or extern.");
    return FALSE;
}

boolean add_string_to_table(line_info* line, data_table_entry** data_table_head, symbols_table_entry** symbol_table_head, extern_entry* ext_head, long* DC)
{
    long L = 0;
    char* string_to_add = line->directive_data;
    data_types data_type = TYPE_STRING;
    data_table_entry* data_table_ptr = *data_table_head;
    data_table_entry* data_table_prev = NULL;
    SET_PREV_POINTER(data_table_prev, data_table_ptr)

    /*skip the " */
    skip_white_spaces(&string_to_add);
    string_to_add++;
    /*copying the string to data_table until " reached"*/
    while (*string_to_add != '"' && !end_of_string(string_to_add))
    {
        data_table_ptr = (data_table_entry*)malloc_with_check(sizeof(data_table_entry));
        data_table_ptr->type = TYPE_STRING;
        data_table_ptr->data.character = *string_to_add++;
        ADD_NODE_TO_LIST(data_table_prev, data_table_ptr, data_table_head);

        L++;
    }

    /*adding \0 to the end of the string */
    data_table_ptr = (data_table_entry*)malloc_with_check(sizeof(data_table_entry));
    data_table_ptr->data.character = '\0';
    data_table_ptr->type = TYPE_STRING;
    ADD_NODE_TO_LIST(data_table_prev, data_table_ptr, data_table_head);
    L++;


    /*check if there is a label and add it*/
    if (line->label_flag)
    {
        data_type = TYPE_STRING;
        if (!add_symbol_to_table(line, symbol_table_head, ext_head, TYPE_STRING, *DC, L))
            return FALSE;
    }

    *DC += L;
    return TRUE;        
}

boolean add_number_to_table(line_info* line, data_table_entry** data_table_head, symbols_table_entry** symbol_table_head, extern_entry* ext_head, long* DC)
{
    data_table_entry* data_table_ptr = *data_table_head;
    data_table_entry* data_table_prev = NULL;
    char* token;
    char* data_to_extract = line->directive_data;
    int i = 0;
    int L = 0;
    
    SET_PREV_POINTER(data_table_prev, data_table_ptr)
    
    token = (char*)malloc_with_check(sizeof(sizeof(data_to_extract)));

    /*reading numbers*/
    skip_white_spaces(&data_to_extract);
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
            return FALSE;
        }
    }/*end of reading numbers*/

    /*check if there is a label and add it*/
    if (line->label)
        if(!add_symbol_to_table(line, symbol_table_head, ext_head, TYPE_NUMBER, *DC, L))
            return FALSE;
    *DC += L;

    free(token);
    return TRUE;
}

boolean add_entry_to_table(line_info* line, symbols_table_entry* symbols_table_head, extern_entry* ext_head, entry_entry** ent_head)
{
    /*declaring pointers*/
    entry_entry* ent_ptr = *ent_head;
    entry_entry* ent_prev = NULL;
    char* entry_lable = line->directive_data;
    char* token;

    SET_PREV_POINTER(ent_prev, ent_ptr);

    skip_white_spaces(&entry_lable);
    token = copy_next_word(entry_lable);

    /* check existsents in tables */
    if(exists_in_entry_table(token, *ent_head))
    {
        printf("Error: in line %d %s\n label `%s` already declared as entry\n", line->line_number, line->line_content, token);
        return FALSE;
    }
    if(exists_in_extern_table(token, ext_head))
    {
        printf("Error: in line %d %s\n label `%s` already exists in current file\n", line->line_number, line->line_content, token);
        return FALSE;
    }

    /* adding label to table */
    ent_ptr = (entry_entry*)malloc_with_check(sizeof(entry_entry));
    ent_ptr->name = token;
    ent_ptr->address = 0;
    ADD_NODE_TO_LIST(ent_prev, ent_ptr, ent_head);

    /* if theres a lable print a warning */
    if(line->label_flag)
        printf("Warning: in line %d %s\n label `%s` is being igored\n", line->line_number, line->line_content, token);

    return TRUE;
}

boolean add_extern_to_table(line_info* line, symbols_table_entry* symbols_table_head, extern_entry** ext_head, entry_entry* ent_head)
{
    /*declaring pointers*/
    extern_entry* ext_ptr = *ext_head;
    extern_entry* ext_prev = NULL;
    char* entry_lable = line->directive_data;
    char* token;

    SET_PREV_POINTER(ext_prev, ext_ptr);

    skip_white_spaces(&entry_lable);
    token = copy_next_word(entry_lable);

    /* check existsents in tables */
    if(exists_in_symbol_table(token, symbols_table_head))
    {
        printf("Error: in line %d %s\n label `%s` already exists in current file\n", line->line_number, line->line_content, token);
        return FALSE;
    }
    if(exists_in_entry_table(token, ent_head))
    {
        printf("Error: in line %d %s\n label `%s` already exists in current file\n", line->line_number, line->line_content, token);
        return FALSE;
    }
    if(exists_in_extern_table(token, *ext_head))
    {
        printf("Error: in line %d %s\n label `%s` already declared as extern\n", line->line_number, line->line_content, token);
        return FALSE;
    }

    /* adding label to table */
    ext_ptr = (entry_entry*)malloc_with_check(sizeof(entry_entry));
    ext_ptr->name = token;
    ext_ptr->address = 0;
    ADD_NODE_TO_LIST(ext_prev, ext_ptr, ext_head);

    /* if theres a lable print a warning */
    if(line->label_flag)
        printf("Warning: in line %d %s\n label `%s` is being igored\n", line->line_number, line->line_content, token);

    return TRUE;
}











boolean add_instruction_to_table(line_info* line, symbols_table_entry** symbol_trable_head, extern_entry** ext_head, code_table_entry** code_table_head, uninitialized_symbols_table_entry** uninitialized_symbol_head, long* IC)
{
    code_table_entry* code_table_temp = NULL;
    code_table_entry* code_table_prev = NULL;
    
    boolean error_flag = FALSE;
    long L = 0;    
    long copy_IC = *IC;

    /* start adding to word memory from address 100 */
    if(*code_table_head == NULL)
        add_100_to_code_table(code_table_head, IC);

    /* set prev pointer */
    code_table_temp = *code_table_head;
    SET_PREV_POINTER(code_table_prev, code_table_temp);
   
    /*adding opcode word*/
    code_table_temp = get_opcode_word(line, IC);
    ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);


    /*adding extra words if needed*/
    /*if there is no operands*/
    if((line->source_operand == NULL || strcmp(line->source_operand, "") == 0) &&
        (line->target_operand == NULL || strcmp(line->target_operand, "") == 0))
    {
        L++;
    }
    /* if theres one operater*/
    else if(line->source_operand == NULL || line->target_operand == NULL || 
        strcmp(line->source_operand, "") == 0 || strcmp(line->target_operand, "") == 0)
    {
        if(line->source_operand == NULL || strcmp(line->source_operand, "") == 0)
        {
            if(is_register(line->target_operand))
            {
                code_table_temp = get_register_word(line->source_operand, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;
            }
            else
            {
                code_table_temp = get_extra_word(uninitialized_symbol_head, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;
            }
        }
        else /* line->taget_operation == NULL */
        {
            if(is_register(line->source_operand))
            {
                code_table_temp = get_register_word(line->source_operand, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;
            }
            else
            {
                code_table_temp = get_extra_word(uninitialized_symbol_head, line->source_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;
            }
        }
    }
    /* if both operands needs to be coded */
    else
    {
        /* if one are more operands are registers */
        if(is_register(line->source_operand) || is_register(line->target_operand))
        {
            /* if both operands are registers */
            if (is_register(line->source_operand) && is_register(line->target_operand))
            {
                code_table_temp = get_register_word(line->source_operand, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;
            }
            /* if source operand a register add the extra code word after register code word*/
            else if(is_register(line->source_operand))
            {
                code_table_temp = get_register_word(line->source_operand, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;

                code_table_temp = get_extra_word(uninitialized_symbol_head, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;
            }
            /* if target operand a register add the extra code word before register code word*/
            else if(is_register(line->target_operand))
            {
                code_table_temp = get_extra_word(uninitialized_symbol_head, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;

                code_table_temp = get_register_word(line->source_operand, line->target_operand, IC);
                ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
                L++;

            }
        }
        /* operands are not registers */
        else if(!is_register(line->source_operand) && !is_register(line->target_operand))
        {
            code_table_temp = get_extra_word(uninitialized_symbol_head, line->source_operand, IC);
            ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
            L++;

            code_table_temp = get_extra_word(uninitialized_symbol_head, line->target_operand, IC);
            ADD_NODE_TO_LIST(code_table_prev, code_table_temp, code_table_head);
            L++;
        }
    }

    /*adding label to symbol table*/
    if(line->label_flag)
        if(!add_symbol_to_table(line, symbol_trable_head, *ext_head, INSTRUCTION, copy_IC, L))
            error_flag = TRUE;

    /*IC += L;*/

    return !error_flag;
}

code_table_entry* get_opcode_word(line_info* line, long* IC)
{
    /* creating a code word with opcode info */
    code_table_entry* code_table_temp = (code_table_entry*)malloc_with_check(sizeof(code_table_entry));
    code_table_temp->type = TYPE_CODE_WORD;
    code_table_temp->value.code_word_value.opcode = get_opcode_bits(line->opcode);
    code_table_temp->value.code_word_value.ARE = ABSOLUTE;
    code_table_temp->value.code_word_value.source_addressing = get_addressing_type(line->source_operand);
    code_table_temp->value.code_word_value.target_addressing = get_addressing_type(line->target_operand);
    code_table_temp->address = (*IC)++;
    return code_table_temp;
}

code_table_entry* get_register_word(char* source_register, char* target_register, long* IC)
{
    /* creating a code word with register info*/
    code_table_entry* code_table_temp = (code_table_entry*)malloc_with_check(sizeof(code_table_entry));
    code_table_temp->type = TYPE_REGISTER_WORD;
    code_table_temp->value.register_word_value.ARE = ABSOLUTE;
    code_table_temp->value.register_word_value.source_register = get_register_number(source_register);
    code_table_temp->value.register_word_value.target_register = get_register_number(target_register);
    code_table_temp->address = (*IC)++;
    return code_table_temp;
}

code_table_entry* get_extra_word(uninitialized_symbols_table_entry** uninitialized_symbol_head, char* operand, long* IC)
{
    /* creating a code word with extra info*/

    uninitialized_symbols_table_entry* uninitialized_symbol_prev = NULL;
    uninitialized_symbols_table_entry* uninitialized_symbol_ptr = *uninitialized_symbol_head;

    SET_PREV_POINTER(uninitialized_symbol_prev, uninitialized_symbol_ptr);

    code_table_entry* code_table_temp = (code_table_entry*)malloc_with_check(sizeof(code_table_entry));
    code_table_temp->type = TYPE_EXTRA_CODE_WORD;
    if(is_number(operand))
    {
        code_table_temp->value.extra_code_word_value.ARE = ABSOLUTE;
        code_table_temp->value.extra_code_word_value.data = atoi(operand);
    }
    else
    {
        code_table_temp->value.extra_code_word_value.ARE = UNINITIALIZED_VALUE;
        code_table_temp->value.extra_code_word_value.data = UNINITIALIZED_VALUE;

        uninitialized_symbol_ptr = (uninitialized_symbols_table_entry*)malloc_with_check(sizeof(uninitialized_symbols_table_entry));
        uninitialized_symbol_ptr->address = *IC;/* TODO: mabey ic--*/
        uninitialized_symbol_ptr->name = operand;
        uninitialized_symbol_ptr->extra_code_word_value = &code_table_temp->value.extra_code_word_value;
        ADD_NODE_TO_LIST(uninitialized_symbol_prev, uninitialized_symbol_ptr, uninitialized_symbol_head);
        
    }
    code_table_temp->address = (*IC)++;
    return code_table_temp;
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

boolean add_symbol_to_table(line_info* line, symbols_table_entry** symbol_table_head, extern_entry* ext_head, line_type type, long address, long L)
{
    symbols_table_entry* symbol_table_ptr = *symbol_table_head;
    symbols_table_entry* symbol_table_prev = NULL;
    char* label_name = line->label;

    SET_PREV_POINTER(symbol_table_prev, symbol_table_ptr)
    
    if(exists_in_symbol_table(label_name, *symbol_table_head))
    {
        printf("error: in line %d %s\nlabel `%s` already already declared\n", line->line_number, line->line_content, label_name);
        return FALSE;
    }
    if (exists_in_extern_table(label_name, ext_head))
    {
        printf("error: in line %d %s\nlabel `%s` already declared as extern\n", line->line_number, line->line_content, label_name);
        return FALSE;
    }

    /*adding the label to symbol table*/
    symbol_table_ptr = (symbols_table_entry*)malloc_with_check(sizeof(symbols_table_entry));
    symbol_table_ptr->address_type = type;
    symbol_table_ptr->address = address;
    symbol_table_ptr->L = L;
    symbol_table_ptr->name = label_name;
    ADD_NODE_TO_LIST(symbol_table_prev, symbol_table_ptr, symbol_table_head);

    
    return TRUE;
}

void add_100_to_code_table(code_table_entry** code_table_head, long* IC)
{
    code_table_entry* code_table_ptr = *code_table_head;
    code_table_entry* code_table_prev = NULL;


    while (*IC < IC_START_ADDRESS)
    {
        code_table_ptr = (code_table_entry*)malloc_with_check(sizeof(code_table_entry));
        code_table_ptr->type = TYPE_CODE_WORD;
        code_table_ptr->value.code_word_value.opcode = UNINITIALIZED_VALUE;
        code_table_ptr->value.code_word_value.ARE = ABSOLUTE;
        code_table_ptr->value.code_word_value.source_addressing = UNINITIALIZED_VALUE;
        code_table_ptr->value.code_word_value.target_addressing = UNINITIALIZED_VALUE;
        code_table_ptr->address = (*IC)++;
        ADD_NODE_TO_LIST(code_table_prev, code_table_ptr, code_table_head);
    }
}

void set_code_table_to_ic_initial_address(code_table_entry** code_table_head)
{
    code_table_entry* code_table_ptr = *code_table_head;
    if (code_table_head == NULL)
        return;
    while (code_table_ptr->address != IC_START_ADDRESS)
    {
        code_table_ptr = code_table_ptr->next;
    }
    *code_table_head = code_table_ptr;
}



















