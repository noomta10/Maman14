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
#include "info_check.h"


/* Gets a string and returns true if the string starts with a piread (full stop) */
boolean is_directive(char* str) {
    if (*str == '.') {
        return TRUE;
    }
    return FALSE;
}


/* checks if the string that was reseved as argument is a label */
boolean is_label(char* str) {
    while (!isspace(*str) && !end_of_string(str)) {
        if (*str == ':') {
            return TRUE;
        }
        str++;
    }
    return FALSE;
}


/* Checks if the line that was read - line_content, is the whole line. If it is, the function will return true.
If its not, will return false and read the rest of the line */
boolean line_too_long(FILE* am_file, char* line_content) {
    int c;

    /* Checks if if full line was read */
    while (*line_content != '\0') {
        if (*line_content == '\n') {
            return FALSE;
        }
        line_content++;
    }

    if ((c = getc(am_file)) == EOF || c == '\n') {
        return FALSE;
    }

    /* Reads rest of line */
    while ((c = getc(am_file)) != '\n' && c != EOF);

    return TRUE;
}


/* Gets a label and the location of the label and returns true if the label if valid. 
Else retruns false and prints a error */
boolean invalid_label(char* file_name, char* label, char* line_content, long line_number) {
    int i = 0;

    /* Check if label is a reserved word */
    if (is_reserved_name(label)) {
        PRINT_ERROR(file_name, line_number, line_content, "The label is a reserved word.");
        return TRUE;
    }

    /* Check if label is too long */
    if (strlen(label) > MAX_LABEL_LENGTH) {
        PRINT_ERROR(file_name, line_number, line_content, "The label is too long.");
        return TRUE;
    }

    /* Checks if the label is empty */
    if (strlen(label) == 0) {
        PRINT_ERROR(file_name, line_number, line_content, "The label is empty.");
        return TRUE;
    }

    /* Check if label starts with a letter */
    if (!isalpha(label[i])) {
        PRINT_ERROR(file_name, line_number, line_content, "The label must start with a letter.");
        return TRUE;
    }

    /* Check if label has invalid characters */
    for (i = 1; i < strlen(label); i++) {
        if (!isalnum(label[i])) {
            PRINT_ERROR(file_name, line_number, line_content, "The label has invalid characters.");
            return TRUE;
        }
    }

    return FALSE;
}


/* Returns true if the string starts with a comma and skips it. Else, returns false */
boolean check_comma(char** str) {
    if (**str == ',') {
        (*str)++;
        return TRUE;
    }
    return FALSE;
}


/* Returns true if the string is a number */
boolean is_number(char* string) {
    int i = 0;
 
    if (string_is_empty(string)) {
        return FALSE;
    }

    /* Check for singed number */
    if (string[0] == '-' || string[0] == '+') {
        i++;
    }

    for (; i < strlen(string); i++) {
        if (!isdigit(string[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

/* Returns true if the line is a command or empty line */
boolean ignore_line(char* line) {
    /* Epmty line or comment line */
    if (end_of_string(line) || *line == ';') {
        return TRUE;
    }
    return FALSE;
}


/* Calls function to check if the instruction line is valid */
boolean valid_instruction_line(line_info* line) {
    /* Check if theres more or less operands then nedded */
    if (!check_extra_or_missing_operands(line)) {
        return FALSE;
    }

    /* Check if operands are valid */
    if (!check_operands(line)) {
        return FALSE;
    }
    return TRUE;
}


/* Calls the right functions to check the directive line */
boolean valid_directive_line(line_info* line) {
    if (strcmp(line->directive_command, "data") == 0) {
        if (!valid_data_command(line)) {
            return FALSE;
        }
    }
    else if (strcmp(line->directive_command, "string") == 0) {
        if (!valid_string_command(line)) {
            return FALSE;
        }
    }
    else if (strcmp(line->directive_command, "entry") == 0) {
        if (!valid_entry_command(line)) {
            return FALSE;
        }
    }
    else if (strcmp(line->directive_command, "extern") == 0) {
        if (!valid_extern_command(line)) {
            return FALSE;
        }
    }
    else {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid directive command.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if the line is a valid data command */
boolean valid_data_command(line_info* line) {
    if (string_is_empty(line->directive_data)) {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Missing data.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if the line is a valid string command */
boolean valid_string_command(line_info* line) {
    char* directive_data = line->directive_data;

    /* Set i to the end of the line and skip the null character */
    int i = strlen(directive_data) -1 ;
    
    /* Check if string is empty */
    if (end_of_string(directive_data)) {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Missing data.");
        return FALSE;
    }

    /* Skip white spaced and update the length i */
    while (isspace(*directive_data) && i >= 0) {
        i--;
        directive_data++;
    }

    /* Check if string starts with quotes */
    if (*directive_data != '"') {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "String must start with quotes.");
        return FALSE;
    }

    /* check if string ends with quotes */
    while (isspace(directive_data[i])) {
        i--;
    }

    if (directive_data[i] != '"' || i == 0) {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "String must end with quotes.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if the line is a valid entry command */
boolean valid_entry_command(line_info* line) {
    if (string_is_empty(line->directive_data)) { 
        /* No entry labels were given */
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "No entry labels were given.");
        return FALSE;
    }

    /* Ilegal comma at the end of the line */
    if (extra_comma(line->line_content)){
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Extra comma at end of line.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if the line is a valid extern command */
boolean valid_extern_command(line_info* line) {
    /* No extern labels were given */
    if(string_is_empty(line->directive_data)) { 
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "No extern labels were given.");
        return FALSE;
    }

    /* Ilegal comma at the end of the line */
    if(extra_comma(line->line_content)){
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Extra comma at end of line.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if there is the right amount of operands */
boolean check_extra_or_missing_operands(line_info* line) {
 /* Checking sub, mov, add, lea commands */
    if (strcmp(line->opcode, "sub") == 0 || 
        strcmp(line->opcode, "mov") == 0 ||
        strcmp(line->opcode, "add") == 0 ||
        strcmp(line->opcode, "cmp") == 0 ||
        strcmp(line->opcode, "lea") == 0) {
        /* Checking for missing operand */
        if (string_is_empty(line->source_operand) || string_is_empty(line->target_operand)) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Missing operand.");
            return FALSE;
        }
    }
    /* Checking not, clr, inc, dec, jmp, bne, red, prn, jsr commands */
    else if (strcmp(line->opcode, "not") == 0 ||
        strcmp(line->opcode, "clr") == 0 ||
        strcmp(line->opcode, "inc") == 0 ||
        strcmp(line->opcode, "dec") == 0 ||
        strcmp(line->opcode, "jmp") == 0 ||
        strcmp(line->opcode, "bne") == 0 ||
        strcmp(line->opcode, "red") == 0 ||
        strcmp(line->opcode, "prn") == 0 ||
        strcmp(line->opcode, "jsr") == 0) {
        /* Checking for missing operand */
        if (string_is_empty(line->source_operand)) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Missing operand.");
            return FALSE;
        }

        /* Checking for extra operand */
        if(!string_is_empty(line->target_operand)){
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Extra operand.");
            return FALSE;
        }
        line->target_operand = line->source_operand; /*moving source operand to target operand*/
        line->source_operand = NULL;
    }
    /* Checking rts, stop commands */
    else if (strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0) {
        /* Checking for extra operand */
        if (!string_is_empty(line->source_operand) || !string_is_empty(line->target_operand)) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Extra operand.");
            return FALSE;
        }
    }
    /* Invalid opcode */
    else {
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid opcode.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if the operands are valid */
boolean check_operands(line_info* line) {
    /* Checking if source operand is valid */
    if (!valid_source_operand(line)) {
        return FALSE;
    }
    
    /* Checking target operand */
    if (!valid_target_operand(line)) {
        return FALSE;
    }
    return TRUE;
}


/* checks if the source operand is valid */
boolean valid_source_operand(line_info* line) {
    if (strcmp(line->opcode, "lea") == 0) {
        /* Source operand must be a label */
        if (get_addressing_type(line->source_operand) != DIRECT_ADDRESSING) {
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid source operand.");
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
        strcmp(line->opcode, "jsr") == 0 ||
        strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0) {
        /* Source operand must be empty */
        if (get_addressing_type(line->source_operand) != NO_OPERAND) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid source operand.");
                return FALSE;
        }
    }

    /* If source operand is a label, check it */
    if (get_addressing_type(line->source_operand) == DIRECT_ADDRESSING && invalid_label(line->file_name, line->source_operand, line->line_content, line->line_number)) {
        return FALSE;
    }

    if (get_addressing_type(line->source_operand) == IMMEDIATE_ADDRESSING &&( instruction_number_too_big(line->source_operand) || instruction_number_too_small(line->source_operand))){
        PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Operand numbers must be between -512 and 511.");
        return FALSE;
    }
    return TRUE;
}


/* Checks if the target operand is valid */
boolean valid_target_operand(line_info* line) {
    if (strcmp(line->opcode, "mov") == 0 ||
        strcmp(line->opcode, "add") == 0 ||
        strcmp(line->opcode, "sub") == 0 ||
        strcmp(line->opcode, "not") == 0 ||
        strcmp(line->opcode, "clr") == 0 ||
        strcmp(line->opcode, "lea") == 0 ||
        strcmp(line->opcode, "inc") == 0 ||
        strcmp(line->opcode, "dec") == 0 ||
        strcmp(line->opcode, "jmp") == 0 ||
        strcmp(line->opcode, "bne") == 0 ||
        strcmp(line->opcode, "red") == 0 ||
        strcmp(line->opcode, "jsr") == 0) {
        /* Target operand can't be empty a number*/
        if (get_addressing_type(line->target_operand) != DIRECT_ADDRESSING &&
            get_addressing_type(line->target_operand) != REGISTER_ADDRESSING) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid target operand.");
                return FALSE;
        }
    }
    else if (strcmp(line->opcode, "rts") == 0 ||
        strcmp(line->opcode, "stop") == 0) {
            /* Target operand must be empty */
            if (get_addressing_type(line->target_operand) != NO_OPERAND) {
                PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Invalid target operand.");
                return FALSE;
            }
    }

    /* If target address is an lable, check it */
    if (get_addressing_type(line->target_operand) == DIRECT_ADDRESSING && invalid_label(line->file_name, line->target_operand, line->line_content, line->line_number)) {
        return FALSE;
    }

    /* Check if operand numbers are in the valid range */
    if (get_addressing_type(line->target_operand) == IMMEDIATE_ADDRESSING && (instruction_number_too_big(line->target_operand) || instruction_number_too_small(line->target_operand))){
            PRINT_ERROR(line->file_name, line->line_number, line->line_content, "Operand numbers must be between -512 and 511.");
            return FALSE;
    }
    return TRUE;
}


/* Check if the symbol is already defined in symbol_table */
boolean exists_in_symbol_table(char* symbol, symbols_table_entry* symbol_table) {
    while (symbol_table != NULL) {
        if (strcmp(symbol_table->name, symbol) == 0) {
            return TRUE;
        }
        symbol_table = symbol_table->next;
    }
    return FALSE;
}


/* Checks if symbol already exists in extern table */
boolean exists_in_extern_table(char* symbol, extern_entry* external_table) {
    while (external_table != NULL) {
        if (strcmp(external_table->name, symbol) == 0) {
            return TRUE;
        }
        external_table = external_table->next;
    }
    return FALSE;
}


/* Checks if symbol already exists in entry table */
boolean exists_in_entry_table(char* symbol, entry_entry* entry_table) {
    while (entry_table != NULL) {
        if (strcmp(entry_table->name, symbol) == 0) {
            return TRUE;
        }
        entry_table = entry_table->next;
    }
    return FALSE;
}


/* Checks if the program size (IC + DC) is bigger than what the imagenary PC can hold */
boolean program_too_big(char* file_name, long IC, long DC) {
    char* as_file_name;

    /* If the program is too big, print error and return TRUE */
    if (IC + DC >= MAX_PROGRAM_LENGTH) {
        as_file_name = add_file_postfix(file_name, ".as");
        printf("Error: The file '%s' you provided is too large for the imaginary computer.\n", as_file_name);
        free(as_file_name);
        return TRUE;
    }
    return FALSE;
}


/* Checks if the operand is a regiser */
boolean is_register(char* operand) {
    if (operand[0] == '@' && operand[1] == 'r' && operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0') {
        return TRUE;
    }
	return FALSE;
}


/* Checks if the number is too big to code in to the memory word */
boolean data_number_too_big(char* string_number) {
    int number = atoi(string_number);

    if (number > MAX_DATA_NUMBER_VALUE){
        return TRUE;
    }
    return FALSE;
}


/* Returns true if the mumber is bigger then MAX_INSTRUCTION_NUMBER_VALUE */
boolean instruction_number_too_big(char* string_number) {
    int number = atoi(string_number);

    if (number > MAX_INSTRUCTION_NUMBER_VALUE) {
        return TRUE;
    }
    return FALSE;
}


/* Returns true if the number is smaller then MIN_INSTRUCTION_NUMBER_VALUE */
boolean instruction_number_too_small(char* string_number) {
    int number = atoi(string_number);

    if (number < MIN_INSTRUCTION_NUMBER_VALUE) {
        return TRUE;
    }
    return FALSE;
}


/* Checks if the number is too small to code in to the memory word */
boolean data_number_too_small(char* string_number) {
    int number = atoi(string_number);

    if (number < MIN_DATA_NUMBER_VALUE) {
        return TRUE;
    }
    return FALSE;
}


/* Checks if the string is empty (contains white spaces) */
boolean string_is_empty(char* string) {
    int i;

    if (string == NULL) {
        return TRUE;
    }

    for (i = 0; i < strlen(string); i++) {
        if (!isspace(string[i])) {
            return FALSE;
        }
    }
    return TRUE;
}


/* Checks for extra command in the end of the line */
boolean extra_comma(char* line) {
    int i = 0;

    /* Go to '\0' position */
    while (line[i] != '\0') {
        i++;
    }

    do {
        /* One step back */
        i--;

        if (line[i] == ',') {
            return TRUE;
        }
    } while (isspace(line[i]));

    return FALSE;
}
