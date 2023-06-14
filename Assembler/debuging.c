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
    printf("printing data_table_entry:\n");
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
    printf("printing code_table_entry:\n");
    while (code_table != NULL)
    {
        printf("address: %ld\n", code_table->address);
        printf("ARE: %d\n", code_table->word.ARE);
        printf("opcode: %d\n", code_table->word.opcode);
        printf("source_addressing: %d\n", code_table->word.source_addressing);
        printf("source_addressing: %d\n", code_table->word.source_addressing);        
        printf("\n");
        code_table = code_table->next;
    }
    printf("-------------------------\n\n");
}

void print_extern_table(extern_entry* ext)
{
    printf("printing extern labels:\n");
    while (ext != NULL)
    {
        printf("name: %s    address: %ld\n", ext->name, ext->address);
        ext = ext->next;
    }
    printf("-------------------------\n\n");
}

void print_entry_table(entry_entry* ent)
{
    printf("printing entry table:\n");
    while (ent != NULL)
    {
        printf("name: %s    address: %ld\n", ent->name, ent->address);
        ent = ent->next;
    }
    printf("-------------------------\n\n");
}

void print_symbol_table(symbols_table_entry* symbol_table)
{
    printf("printing symbol table:\n");
    while (symbol_table != NULL)
    {
        printf("name: %s\n", symbol_table->name);
        printf("address: %ld\n", symbol_table->address);
        printf("length (L): %ld\n", symbol_table->L);
        printf("is_data: %s\n", (symbol_table->is_data) ? "TRUE" : "FALSE");
        symbol_table = symbol_table->next;
        printf("\n");
    }
    printf("-------------------------\n\n");
}

void print_line_info(line_info* line)
{
    printf("printing line info:\n");
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

