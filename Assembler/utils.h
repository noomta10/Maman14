#pragma once

char* add_file_postfix(char* file_name, char* postfix);
void* malloc_with_check(size_t length);
void* realloc_with_check(char* ptr, size_t length);
char* get_next_element(char* line);

boolean is_register(char* operand);