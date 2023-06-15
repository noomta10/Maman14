#pragma once



boolean is_directive(char*);
boolean is_label(char*);


boolean line_too_long(FILE* am_file, char* line_content);

boolean bad_label(char* label);


boolean check_comma(char** string);
