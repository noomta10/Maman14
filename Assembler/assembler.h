#define _CRT_SECURE_NO_WARNINGS

#define MAX_LINE_LENGTH 81 /* Max line length plus one for \n */
#define IC_START_ADDRESS 100 
#define MAX_LABEL_LENGTH 31
#define MAX_PROGRAM_LENGTH 1024
#define MAX_DATA_NUMBER_VALUE 2047
#define MIN_DATA_NUMBER_VALUE -2048
#define PRINT_ERROR(file_name, line_number, line_content, error_message)\
printf("Error in file '%s', line %d: \"%s\":\n%s\n",file_name, line_number, line_content, error_message)

typedef enum {FALSE = 0, TRUE = 1} boolean;

