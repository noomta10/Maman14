#define _CRT_SECURE_NO_WARNINGS

#define MAX_LINE_LENGTH 81 /* Max line length plus one for \n */
#define IC_START_ADDRESS 100 
#define MAX_LABEL_LENGTH 31

typedef enum {FALSE = 0, TRUE = 1} boolean;

typedef struct {
    long line_number;
    char* line_content;
    char* label;
    char* directive_data;
    char* directive_command;
    char* opcode;
    char* source_operand;
    char* target_operand;
    boolean comma_flag;
    boolean label_flag;
    boolean directive_flag;// .data, .string, .entry, .extern
    boolean instruction_flag;
    boolean extra_chars_flag;
} line_info;