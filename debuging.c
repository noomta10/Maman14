#include <stdio.h>
#include "first_pass.h"
#include "debuging.h"



void print_data_table(data_table_entry* data_table)
{
    printf("printing data_table_entry:\n");
    while (data_table->next != NULL)
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

void print_extern_table(extern_entry* ext)
{
    printf("printing extern labels:\n");
    while (ext->next != NULL)
    {
        printf("name: %s    address: %ld\n", ext->name, ext->address);
        ext = ext->next;
    }
    printf("-------------------------\n\n");
}

void print_entry_table(entry_entry* ent)
{
    printf("printing entry table:\n");
    while (ent->next != NULL)
    {
        printf("name: %s    address: %ld\n", ent->name, ent->address);
        ent = ent->next;
    }
    printf("-------------------------\n\n");
}

void print_symbol_table(symbols_table_entry* symbol_table)
{
    printf("printing symbol table:\n");
    while (symbol_table->next != NULL)
    {
        printf("name: %s\n", symbol_table->name);
        printf("address: %ld\n", symbol_table->address);
        printf("length (L): %ld\n", symbol_table->L);
        printf("is_data: %s\n", (symbol_table->is_data) ? "TRUE" : "FALSE");
        printf("is_external: %s\n", (symbol_table->type == TYPE_EXTERN) ? "TRUE" : "FALSE");
        printf("is_entry: %s\n", (symbol_table->type == TYPE_ENTRY) ? "TRUE" : "FALSE");
        symbol_table = symbol_table->next;
        printf("\n");
    }
    printf("-------------------------\n\n");
}

void print_line_info(line_info* line)
{
    printf("is_label: %s\n", (line->is_label) ? "TRUE" : "FALSE");
    printf("label: %s\n", line->label);
    printf("is_instruction: %s\n", (line->is_instruction) ? "TRUE" : "FALSE");
    printf("instruction: %s\n", line->instruction);
    printf("is_data: %s\n", (line->is_data) ? "TRUE" : "FALSE");
    printf("instruction_data: %s\n", line->instruction);
    printf("instruction_data: %s\n", line->instruction_data);
    printf("opcode: %s\n", line->opcode);
    printf("source_operand: %s\n", line->source_operand);
    printf("target_operand: %s\n", line->target_operand);
    printf("-------------------------\n\n");
}