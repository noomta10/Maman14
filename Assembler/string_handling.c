#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "assembler.h"
#include "line_info.h"
#include "utils.h"
#include "tables.h"
#include "string_handling.h"
#include "info_check.h"


/* Clears the string */
void reset_str(char* str) {
    while (*str != '\0') {
        *str++ = '\0';
    }
}


/* Skips white spaces */
void skip_white_spaces(char** str) {
    char* temp = *str;

    /* Skip white spaces */
    while (*temp == ' ' || *temp == '\t') { 
        temp++;
    }
    *str = temp;
}


/* Returns a copy of the next word in the string */
char* copy_next_word(char* string) {
    int word_length = 0;
    char* result;
    char* string_ptr = string;

    /* Get the length of the next word */
    while (!isspace(*string_ptr) && !end_of_string(string_ptr) && *string_ptr != ',') {
        word_length++;
        string_ptr++;
    }

    result = (char*)malloc_with_check(sizeof(char) * (word_length + 1));
    strncpy(result, string, word_length);
    result[word_length] = '\0';
    return result;    
}


/* Returns a copy of the string */
char* copy_string(char* string) {
    char* result = malloc_with_check(sizeof(string)* (strlen(string) + 1));
    strcpy(result, string);
    return result;
}


/* Return true if theres no more characters that are not white spases */
boolean end_of_string(char* str) {
    if (string_is_empty(str)) {
        return TRUE;
    }
    return FALSE;
}


/* Returns a copy of the opcode from the line str */
char* get_opcode(char** str) {
    int opcode_length = 0;
    char* temp = *str;
    char* token;

    /* Gets opcode length */
    while (!isspace(*temp) && *temp != '\0') {
        temp++;
        opcode_length++;
    }

    /* Copies it */
    token = (char*)malloc_with_check(sizeof(char) * (opcode_length +1));
    strncpy(token, *str, opcode_length);
    token[opcode_length] = '\0';
    *str = temp;
    return token;
}


/* Returns a copy of the operand and skips it in the string */
char* get_operand(char** str) {
    int operand_length = 0;
    char* temp = *str;
    char* token;

    /* Gets the operand length */
    while (!isspace(*temp) && *temp != '\0') {
        if (*temp == ',') {
            break;
        }
        temp++;
        operand_length++;
    }

    /* Copies it */
    token = (char*)malloc_with_check(sizeof(char) * (operand_length + 1));
    strncpy(token, *str, operand_length);
    token[operand_length] = '\0';
    *str = temp;
    return token;
}


/* Gets the copys the directive command and skips it in the original string */
char* get_directive(char** str) {
    char* temp = ++ * str; /* Skip '.' */
    char* token;
    int directive_length = 0;

    /* Get the length */
    while (!isspace(*temp) && *temp != '\0') {
        temp++;
        directive_length++;
    }

    /* Copy it */
    token = (char*)malloc_with_check(sizeof(char) * (directive_length + 1));
    strncpy(token, *str, directive_length);
    token[directive_length] = '\0';
    *str = temp;
    return token;
}


/* Returns a copy of the label and skips it in the original string */
char* get_label(char** str) {
    char* temp = *str;
    char* token;
    int label_length = 0;

    /* Get the length */
    while (*temp != ':') {
        temp++;
        label_length++;
    }

    /* Copy it */
    token = (char*)malloc_with_check(sizeof(char) * (label_length + 1));
    strncpy(token, *str, label_length);
    token[label_length] = '\0';
    *str = ++temp; /* And skip : */
    return token;
}


/* Returns the opcode code */
opcode_type get_opcode_bits(char* opcode) {
    if (strcmp(opcode, "mov") == 0) {
        return MOV_OPCODE;
    }
    if (strcmp(opcode, "cmp") == 0) {
        return CMP_OPCODE;
    }
    if (strcmp(opcode, "add") == 0) {
        return ADD_OPCODE;
    }
    if (strcmp(opcode, "sub") == 0) {
        return SUB_OPCODE;
    }
    if (strcmp(opcode, "not") == 0) {
        return NOT_OPCODE;
    }
    if (strcmp(opcode, "clr") == 0) {
        return CLR_OPCODE;
    }
    if (strcmp(opcode, "lea") == 0) {
        return LEA_OPCODE;
    }
    if (strcmp(opcode, "inc") == 0) {
        return INC_OPCODE;
    }
    if (strcmp(opcode, "dec") == 0) {
        return DEC_OPCODE;
    }
    if (strcmp(opcode, "jmp") == 0) {
        return JMP_OPCODE;
    }
    if (strcmp(opcode, "bne") == 0) {
        return BNE_OPCODE;
    }
    if (strcmp(opcode, "red") == 0) {
        return RED_OPCODE;
    }
    if (strcmp(opcode, "prn") == 0) {
        return PRN_OPCODE;
    }
    if (strcmp(opcode, "jsr") == 0) {
        return JSR_OPCODE;
    }
    if (strcmp(opcode, "rts") == 0) {
        return RTS_OPCODE;
    }
    if (strcmp(opcode, "stop") == 0) {
        return STOP_OPCODE;
    }
    return 0; 
}


/* Returns the register code */
register_type get_register_number(char* register_name) {
    if (register_name == NULL || strcmp(register_name, "") == 0 || !is_register(register_name)) {
        return NO_REGISTER;
    }
    if (strcmp(register_name, "@r0") == 0) {
        return R0;
    }
    if (strcmp(register_name, "@r1") == 0) {
        return R1;
    }
    if (strcmp(register_name, "@r2") == 0) {
        return R2;
    }
    if (strcmp(register_name, "@r3") == 0) {
        return R3;
    }
    if (strcmp(register_name, "@r4") == 0) {
        return R4;
    }
    if (strcmp(register_name, "@r5") == 0) {
        return R5;
    }
    if (strcmp(register_name, "@r6") == 0) {
        return R6;
    }
    return R7;
}


/* Removes the new line character from the end of the string */
void remove_new_line_character(char* string) {
    if (string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
}
