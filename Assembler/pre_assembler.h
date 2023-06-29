/* Mcros_table_entry represents an entry in the mcro table.
Contains a pointer to the next entry, the name of the mcro, and its value. */
typedef struct mcros_table_entry {
	struct mcros_table_entry* next;
	char* name;
	char* value;
} mcros_table_entry;

/* Get a pointer to the source file and the file name.
Read the file line by line and handle the mcros in the file.
If errors were found, pre-assembler failed, return FALSE, and continue to the next files if there are any.
Else, pre-assembler succeeded, return TRUE. */
boolean pre_assembler(FILE* source_file, char* file_name);
