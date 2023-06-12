typedef struct {
    long line_number;
    char* file_name;
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

void reset_line_info(line_info* line);