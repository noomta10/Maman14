/* Get file name and a desired postfix, return a pointer to the new concatenated name */
char* add_file_postfix(char* file_name, char* postfix);

/* Dynamically allocate memory. Exit the program if the allocation failed */
void* malloc_with_check(size_t length);

/* Dynamically reallocate memory. Exit the program if the reallocation failed */
void* realloc_with_check(char* ptr, size_t length);

/* Check if a given name is reserved. If it is return TRUE, else, FALSE */
boolean is_reserved_name(char* name);
