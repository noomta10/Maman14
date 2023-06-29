/* Line_info struct contains detailed information about the line */
typedef struct {
    long line_number;
    char* file_name;
    char* line_content;
    char* label;
    char* directive_data;
    char* directive_command; /* .data, .string, .entry, .extern */
    char* opcode; /* mov, cmp, add, sub, not, clr, lea, inc, dec, jmp, bne, red, prn, jsr, rts, stop */
    char* source_operand;
    char* target_operand;
    boolean comma_flag;
    boolean label_flag;
    boolean directive_flag; /* If it is one of: .data, .string, .entry, .extern */
    boolean instruction_flag; /* If it is one the opcodes */
    boolean extra_chars_flag; 
} line_info;

/* Get line_info struct and reset all its fields */
void reset_line_info(line_info* line);
