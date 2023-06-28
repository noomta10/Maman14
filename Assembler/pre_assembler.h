/* Mcros_table_entry represents an entry in the mcro table.
Contains a pointer to the next entry, the name of the mcro, and its value. */
typedef struct mcros_table_entry {
	struct mcros_table_entry* next;
	char* name;
	char* value;
} mcros_table_entry;

/* Get a pointer to the source file and the file name.
Read the file libe by line and convert the mcros in the file.
If errors were found, return FALSE, else, TRUE. */
boolean pre_assembler(FILE* source_file, char* file_name);
