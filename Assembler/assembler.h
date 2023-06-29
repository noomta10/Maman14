#define _CRT_SECURE_NO_WARNINGS

#define NO_ERRORS 0 /* Return value for main function- program was completed successfully */
#define NO_FILES_GIVEN_ERROR -1 /* Return value for main function- no files were given */
#define MAX_LINE_LENGTH 81 /* Maximum line length plus one for '\n' */
#define IC_START_ADDRESS 100 /* Initial address of IC in the code image */
#define MAX_LABEL_LENGTH 31 /* Maximum label length */
#define MAX_PROGRAM_LENGTH 1024 /* Maximum words in the program */
#define MAX_DATA_NUMBER_VALUE 2047 /* Maximum value of 12 bits number in data word */
#define MAX_INSTRUCTION_NUMBER_VALUE 511 /* Maximum value of 10 bits number in instruction word */
#define MIN_DATA_NUMBER_VALUE -2048 /* Minmun  value of 12 bits number in data word */
#define MIN_INSTRUCTION_NUMBER_VALUE -512 /* Minimum value of 10 bits number in instruction word */

/* Print detailed error */
#define PRINT_ERROR(file_name, line_number, line_content, error_message)\
printf("Error in file '%s', line %ld: \"%s\":\n%s\n",file_name, line_number, line_content, error_message)

/* Booleans- TRUE and FALSE */
typedef enum {FALSE = 0, TRUE = 1} boolean;
