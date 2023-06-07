typedef struct mcros_table_entry {
	struct mcros_table_entry* next;
	char* name;
	char* value;
} mcros_table_entry;


void pre_assembler(FILE* source_file, char* file_name);
