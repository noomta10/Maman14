#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "first_pass.h"
#include "debuging.h"
#include "encoding.h"



void print_data_table(data_table_entry* data_table)
{
    printf("printing data_table_entry:\n\n");
    while (data_table != NULL)
    {
        if (data_table->type == TYPE_STRING)
        {
            printf("string: %c  address: %ld\n", data_table->data.character, data_table->address);
            data_table = data_table->next;
            continue;
        }
        if (data_table->type == TYPE_NUMBER)
        {
            printf("data int: %ld   address: %ld\n", data_table->data.number, data_table->address);
            data_table = data_table->next;
            continue;
        }
    }
    printf("-------------------------\n\n");

}

void print_code_word(code_table_entry* code_table)
{
    printf("printing code_table_entry:\n\n");
    while (code_table != NULL)
    {
        if (code_table->type == TYPE_CODE_WORD)
        {
            printf("code word address: %ld\n", code_table->address);
            printf("ARE: %d\n", code_table->value.code_word_value.ARE);
            printf("opcode: %d\n", code_table->value.code_word_value.opcode);
            printf("source_operand_addressing: %d\n", code_table->value.code_word_value.source_addressing);
            printf("target_operand_addressing: %d\n", code_table->value.code_word_value.target_addressing);
            printf("\n" );
        }
        if (code_table->type == TYPE_REGISTER_WORD)
        {
            printf("register word address: %ld\n", code_table->address);
            printf("ARE: %d\n", code_table->value.register_word_value.ARE);
            printf("source_register: %d\n", code_table->value.register_word_value.source_register);
            printf("target_register: %d\n", code_table->value.register_word_value.target_register);
            printf("\n");
        }
        if (code_table->type == TYPE_EXTRA_CODE_WORD)
        {
            printf("extra code word address: %ld\n", code_table->address);
            printf("ARE: %d\n", code_table->value.extra_code_word_value.ARE);
            printf("data: %d\n", code_table->value.extra_code_word_value.data);
            printf("\n");
        }
        code_table = code_table->next;
    }
    printf("-------------------------\n\n");
}

void print_extern_table(extern_entry* ext)
{
    printf("printing extern labels:\n\n");
    while (ext != NULL)
    {
        printf("name: %s\naddress: %ld\n", ext->name, ext->address);
        printf("line number: %ld\n", ext->line_number);
        printf("\n");
        ext = ext->next;
    }
    printf("-------------------------\n\n");
}

void print_entry_table(entry_entry* ent)
{
    printf("printing entry table:\n\n");
    while (ent != NULL)
    {
        printf("name: %s    address: %ld\n", ent->name, ent->address);
        ent = ent->next;
    }
    printf("-------------------------\n\n");
}

void print_symbol_table(symbols_table_entry* symbol_table)
{
    printf("printing symbol table:\n\n");
    while (symbol_table != NULL)
    {
        printf("name: %s\n", symbol_table->name);
        printf("address: %ld\n", symbol_table->address);
        printf("length (L): %ld\n", symbol_table->L);
        printf("line is: `%s`\n", (symbol_table->address_type == DIRECTIVE) ? "DIRECTIVE" : "INSTRUCTION");
        symbol_table = symbol_table->next;
        printf("\n");
    }
    printf("-------------------------\n\n");
}

void print_line_info(line_info* line)
{
    printf("printing line info:\n\n");
    printf("line info: %ld %s\n", line->line_number, line->line_content);
    printf("label_flag: %s\n", (line->label_flag) ? "TRUE" : "FALSE");
    printf("label: %s\n", line->label);
    printf("directive_flag: %s\n", (line->directive_flag) ? "TRUE" : "FALSE");
    printf("directive_command: %s\n", line->directive_command);
    printf("directive_data: %s\n", line->directive_data);
    printf("instruction_flag: %s\n", (line->instruction_flag) ? "TRUE" : "FALSE");
    printf("opcode: %s\n", line->opcode);
    printf("source_operand: %s\n", line->source_operand);
    printf("target_operand: %s\n", line->target_operand);
    printf("comma_flag: %s\n", (line->comma_flag) ? "TRUE" : "FALSE");
    printf("extra_chars_flag: %s\n", (line->extra_chars_flag) ? "TRUE" : "FALSE");
}

void print_uninitialized_symbols_table(uninitialized_symbols_table_entry* uninitialized_symbols_table)
{
    printf("printing uninitialized symbols table:\n\n");
    while (uninitialized_symbols_table != NULL)
    {
        printf("name: %s\n", uninitialized_symbols_table->name);
        printf("address: %ld\n", uninitialized_symbols_table->address);
        printf("ARE: %d\n", uninitialized_symbols_table->extra_code_word_value->ARE);
        printf("data: %d\n", uninitialized_symbols_table->extra_code_word_value->data);
        printf("\n");
        uninitialized_symbols_table = uninitialized_symbols_table->next;
    }
    printf("-------------------------\n\n");
}

void print_code_table_in_binary( code_table_entry* code_table)
{
    int i = 0;
    printf("printing code_table_entry in binary:\n\n");
    while (code_table != NULL)
    {
        printf("%d:  ", i++);
        if (code_table->type == TYPE_CODE_WORD)
        {
            print_decimal_to_binary(code_table->value.code_word_value.source_addressing, 3);
            print_decimal_to_binary(code_table->value.code_word_value.opcode, 4);
            print_decimal_to_binary(code_table->value.code_word_value.target_addressing, 3);
            print_decimal_to_binary(code_table->value.code_word_value.ARE, 2);
            printf("\n");
        }
        if (code_table->type == TYPE_REGISTER_WORD)
        {
            print_decimal_to_binary(code_table->value.register_word_value.target_register, 5);
            print_decimal_to_binary(code_table->value.register_word_value.source_register, 5);
            print_decimal_to_binary(code_table->value.register_word_value.ARE, 2);
            printf("\n");
        }
        if (code_table->type == TYPE_EXTRA_CODE_WORD)
        {
            print_decimal_to_binary(code_table->value.extra_code_word_value.data, 10);
            print_decimal_to_binary(code_table->value.extra_code_word_value.ARE, 2);
            printf("\n");
        }
        code_table = code_table->next;
    }
    printf("-------------------------\n\n");
}