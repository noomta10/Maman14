#include "first_pass.h"

int main()
{
    /*debugging*/

    symbols_table_entry* symbol_table = (symbols_table_entry *) malloc_or_exit(sizeof(symbol_table));
    data_table_entry *data_table = (data_table_entry *)malloc_or_exit(sizeof(data_table));
    entry_entry* ent = (entry_entry *)malloc_or_exit(sizeof(ent));
    extern_entry* ext = (extern_entry *) malloc_or_exit(sizeof(ext));
    
    reset_entry(ent);
    reset_extern(ext);
    reset_symbol(symbol_table);
    reset_data(data_table);
    /*symbols_table_entry* symbol_table_ptr = symbol_table;
    data_table_entry* data_table_ptr = data_table;
    entry_entry* ent_ptr = ent;
    extern_entry* ext_ptr = ext;*/

    long IC;
    long DC;
 
    first_pass("C:\\Users\\yonie\\source\\repos\\YoniEastwood\\maman14\\test.as", symbol_table, data_table, ent, ext,  &IC, &DC);

    printf("IC = %ld\n", IC);
    printf("DC = %ld\n", DC);
    printf("\n");
    print_data_table(data_table); /*debugging*/
    print_symbol_table(symbol_table); /*debugging*/
    print_entry_table(ent); /*debugging*/
    print_extern_table(ext); /*debugging*/

    return 0;
}

boolean first_pass(char *am_file_name, symbols_table_entry *symbol_table, data_table_entry *data_table, entry_entry *ent, extern_entry *ext, long *IC, long *DC) /*processes file*/
{
    FILE *am_file = fopen(am_file_name, "r");
    if (am_file == NULL)
    {
        perror(am_file_name);
        exit(1);
    }
    char line_content[MAX_LINE_LENGTH];
    line_info *line = (line_info *)malloc_or_exit(sizeof(line_info));
    boolean *error_in_code = (boolean *)malloc_or_exit(sizeof(error_in_code));
    *DC = *IC = 0;/*end of page 48*/
    *error_in_code = FALSE;
    reset_line_info(line); /*initializing line*/

    while (fgets(line_content, MAX_LINE_LENGTH, am_file) != NULL) /*reading .as file line by line*/
    {
        if (ignore_line(line_content))
        { /*empty or command line*/
            printf("debug: empty or command line\n");
            continue;
        }
        if (!extract_command_info(line_content, line))/*extracting line info*/
        {
            *error_in_code = TRUE;          
        }                   
        process_line_first_pass(line, IC, DC, symbol_table, data_table, ent, ext, error_in_code); /*processing line*/
                          
        //print_line_info(line); /*debugging*/
        //print_data_table(data_table); /*debugging*/
        //print_symbol_table(symbol_table); /*debugging*/
        //print_entry_table(ent); /*debugging*/
        //print_extern_table(ext); /*debugging*/

        reset_line_info(line);                                    /*freeing line memory*/
        reset_str(line_content);                 /*clearing line content*/
    }


    fclose(am_file); /*closing file*/
    free(line);      /*freeing line memory*/
    /*create tables and files*/

    return !*error_in_code; /*return true if code is good otherwise returns false*/
}

/*processes line*/
void process_line_first_pass(line_info *line, long *IC, long *DC, symbols_table_entry *symbol_table, data_table_entry *data_table,
entry_entry *ent, extern_entry *ext, boolean *error_in_code)
{
    static int line_number = 0;
    line_number++;
    //boolean label_exists;

    printf("debug: line number: %d\n", line_number);

    if (!validate_line(line)) /*validating line*/
    {
        printf("debug: invalid line\n");
        *error_in_code = TRUE;
    }

    if(line->is_instruction){/*if line is data. fix, is data*/
        if(!add_data_to_table(line, symbol_table, data_table, ext, ent, DC, error_in_code))
            *error_in_code = TRUE;
    }


    /*continue here */
}

void reset_str(char *str) /*clears string*/
{
    while(*str != '\0')
        *str++ = '\0';
}

void reset_line_info(line_info *line)
{
    line->is_label = FALSE;
    line->is_instruction = FALSE;
    line->is_data = FALSE;
    line->comma = FALSE;
    line->label = NULL;
    line->instruction = NULL;
    line->opcode = NULL;
    line->source_operand = NULL;
    line->target_operand = NULL;
    line->instruction_data = NULL;
}

boolean validate_line(line_info *line) /*validates line*/
{
    if (line->is_label) /*checking label*/
    {
        if (bad_label(line->label))
            return FALSE;
    }
    if (line->is_instruction) /*checking instruction*/
    {
        if (!check_instruction(line->instruction))
            return FALSE;
    }
    else if (!line->is_instruction) /*command line*/
    {
        if (strcmp(line->opcode, "sub") == 0 || /*checking for missing operand in sub, mov, add, lea*/
            strcmp(line->opcode, "mov") == 0 ||
            strcmp(line->opcode, "add") == 0 ||
            strcmp(line->opcode, "lea") == 0)
        {
            if (line->source_operand == NULL || line->target_operand == NULL)
            {
                printf("Error: missing operand\n");
                return FALSE;
            }
        }
        else if (strcmp(line->opcode, "not") == 0 ||
                 strcmp(line->opcode, "clr") == 0 ||
                 strcmp(line->opcode, "inc") == 0 ||
                 strcmp(line->opcode, "dec") == 0 ||
                 strcmp(line->opcode, "jmp") == 0 ||
                 strcmp(line->opcode, "bne") == 0 ||
                 strcmp(line->opcode, "red") == 0 ||
                 strcmp(line->opcode, "prn") == 0 ||
                 strcmp(line->opcode, "jsr") == 0)
        { /*checking for missing operand*/
            if (line->source_operand == NULL)
            {
                printf("Error: missing operand\n");
                return FALSE;
            }
            else if (line->target_operand != NULL && strcmp(line->target_operand, "") != 0)
            { /*checking for extra operand*/
                printf("Error: extra operand\n");
                return FALSE;
            }
            line->target_operand = line->source_operand; /*moving source operand to target operand*/
            line->source_operand = NULL;
        }
        else if (strcmp(line->opcode, "rts") == 0 ||
                 strcmp(line->opcode, "stop") == 0)
        {
            if ((line->source_operand != NULL && strcmp(line->source_operand, "")) ||
                (line->target_operand != NULL && strcmp(line->target_operand, "")))
            { /*checking for extra operand*/
                printf("Error: extra operand\n");
                return FALSE;
            }
        }
        else
        {
            printf("Error: invalid opcode\n");
            return FALSE;
        }
    }
    return TRUE;
}

boolean extract_command_info(char *content, line_info *line)
{
    
    char *token;

    skip_white_spaces(&content);
    if (is_label(content)) /*check for label*/
    {
        token = get_label(&content);
        line->is_label = TRUE;
        line->label = token;
    }
    else
    {
        line->is_label = FALSE;
    }

    skip_white_spaces(&content);

    if (is_instruction(content)) /*check if instruction sentence*/
    {
        token = get_instruction(&content);
        line->is_instruction = TRUE;
        line->instruction = token;
        line->instruction_data = content; /*instruction data unchecked*/
        return TRUE;
    }
    else
    {
        line->is_instruction = FALSE;
    }

    skip_white_spaces(&content);

    token = get_opcode(&content); /*get opcode*/
    if (token != NULL)
        line->opcode = token;
    else
    {
        printf("Error: invalid opcode\n");
        return FALSE;
    }

    skip_white_spaces(&content);

    token = get_operand(&content); /*get source operand*/
    if (token != NULL)
        line->source_operand = token;
    else
        return TRUE;

    skip_white_spaces(&content);
    line->comma = check_comma(&content); /*check for comma*/
    skip_white_spaces(&content);

    token = get_operand(&content); /*get target operand*/
    if (token != NULL)
        line->target_operand = token;
    else
        return TRUE;

    skip_white_spaces(&content);

    if (!end_of_string(content)) /*check for extra operands*/
    {
        printf("Error: extra operand\n");
        return FALSE;
    }

    return TRUE;
}

boolean is_instruction(char *str)
{
    if (*str == '.')
        return TRUE;
    return FALSE;
}

boolean is_label(char *str)
{
    char *temp = str;
    while (!isspace(*temp) && *temp != '\0')
        temp++;
    if (*--temp == ':')
        return TRUE;
    return FALSE;
}

boolean check_comma(char **str)
{
    if (**str == ',')
    {
        (*str)++;
        return TRUE;
    }
    return FALSE;
}

char *get_label(char **str)
{
    char *temp = *str;
    char *token;
    int i = 0;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        i++;
    }
    token = (char *) malloc_or_exit(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

char *get_opcode(char **str)
{
    int i = 0;
    char *temp = *str;
    char *token;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        i++;
    }
    token = (char*)malloc_or_exit(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

char *get_operand(char **str)
{
    int i = 0;
    char *temp = *str;
    char *token;
    while (!isspace(*temp) && *temp != '\0')
    {
        if (*temp == ',')
            break;
        temp++;
        i++;
    }
    token = (char*)malloc_or_exit(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

char *get_instruction(char **str)
{
    char *temp = ++*str; /*skip '.'*/
    char *token;
    int i = 0;
    while (!isspace(*temp) && *temp != '\0')
    {
        temp++;
        i++;
    }
    token = (char*)malloc_or_exit(sizeof(char) * (i + 1));
    strncpy(token, *str, i);
    token[i] = '\0';
    *str = temp;
    return token;
}

boolean end_of_string(char *str)
{
    if (*str == '\0' || *str == '\n')
        return TRUE;
    return FALSE;
}

void skip_white_spaces(char **str)
{
    char *temp = *str;
    while (*temp == ' ' || *temp == '\t') /*skip  white space*/
        temp++;
    *str = temp;
}

boolean check_instruction(char *str)
{
    if (strcmp(str, "data") || strcmp(str, "string") ||
        strcmp(str, "entry") || strcmp(str, "extern"))
        return TRUE;
    return FALSE;
}

boolean ignore_line(char *line)
{
    if (*line == '\n' || *line == ';')
        return TRUE;
    return FALSE;
}

boolean bad_label(char *str)
{
    if (strlen(str) > MAX_LABEL_LENGTH) /*label to long*/
    {
        printf("Error: label to long");
        return TRUE;
    }
    if (strlen(str) == 0) /*label to short. can happen?? fix*/
    {
        printf("Error: label to short");
        return TRUE;
    }
    return FALSE;
}

char *get_word(char **str)
{
    int i = 0;
    char *res = (char *) malloc_or_exit(MAX_LINE_LENGTH);
    char *temp = *str;

    if (str == NULL)
        return NULL;

    while (*temp != '\0' && i < MAX_LINE_LENGTH)
    {
        if (isspace(*temp))
        {
            temp++;
            continue;
        }

        res[i++] = *temp++;
    }

    res[i] = '\0';
    return res;
}

void *malloc_or_exit(size_t size)
{
    void *ptr;
    if ((ptr = malloc(size)) == NULL)
    {
        perror("malloc ");
        exit(1);
    }
    return ptr;
}

boolean add_data_to_table(line_info *line, symbols_table_entry *symbol_table, data_table_entry *data_table, extern_entry *ext, entry_entry *ent, long *DC, boolean *error_in_code)
{
    extern_entry* temp_ext = ext;
    entry_entry* temp_ent = ent;
    char * data_to_extract = line->instruction_data;
    char *token = (char *) malloc_or_exit(sizeof(char) * (MAX_LABEL_LENGTH + 1));
    int i = 0;
    long L = 0;
    skip_white_spaces(&data_to_extract);
    symbol_data_types data_type = (symbol_data_types)malloc_or_exit(sizeof(data_type));/*for label if exist*/
    data_table_entry* data_table_ptr = data_table;
    
    if(strcmp(line->instruction, "string") == 0)
    {
        data_type = TYPE_STRING;/*for label if exist*/
        if(*data_to_extract != '"')
        {
            printf("Error: string data must start with \"\n");
            return FALSE;
        }
        data_to_extract++;/*skip "*/
        while(*data_to_extract != '"')
        {
            data_table_ptr->type = TYPE_STRING;
            data_table_ptr->data.character = *data_to_extract++;
            data_table_ptr->next = (data_table_entry*)malloc_or_exit(sizeof(data_table));
            data_table_ptr = data_table_ptr->next;
            reset_data(data_table);
            L++;
        }
        data_table_ptr->data.character = '\0';
        L++;

        data_to_extract++;/*skip "*/
        *DC += L;

        if(!end_of_string(data_to_extract))
        {
            printf("Error: extra character after string \"\n");
            return FALSE;
        }

        if (line->is_label && !*error_in_code)
            *error_in_code = !add_symbol_to_table(line, symbol_table, data_type, DC, L);
           
        return TRUE;
    }
    if(strcmp(line->instruction, "data") == 0)
    {
        data_type = TYPE_NUMBER;/*for label*/

        while(!end_of_string(data_to_extract))/*reading numbers until end of line reached*/
        {
            if (*data_to_extract == '-' || *data_to_extract == '+')/*check for + or - sign*/
                token[i++] = *data_to_extract++;

            while(isdigit(*data_to_extract))/*copy the number*/
                token[i++] = *data_to_extract++;
            token[i] = '\0';

            if(strlen(token) == 0 || strcmp(token, "-") == 0 || strcmp(token, "+") == 0)/*error checking no number, empty string*/
            {
                printf("Error: missing number\n");
                return FALSE;
            }
            data_table_ptr->type = TYPE_NUMBER;
            data_table_ptr->data.number = atoi(token);
            data_table_ptr->next = (data_table_entry*)malloc_or_exit(sizeof(data_table));
            data_table_ptr = data_table_ptr->next;
            reset_data(data_table);


            L++;
            i = 0;
            reset_str(token);

            skip_white_spaces(&data_to_extract);
            if (*data_to_extract == ',')
            {
                data_to_extract++;
                skip_white_spaces(&data_to_extract);
            }
            else if (!end_of_string(data_to_extract))
            {
                printf("Error: expected a comma between numbers\n");
                *error_in_code = TRUE;
                return FALSE;
            }
        }/*end of while reading numbers*/
        if (line->is_label && !*error_in_code)
            *error_in_code = !add_symbol_to_table(line, symbol_table, data_type, DC, L);
            
        DC += L;/*increases DC by the number of numbers in the instruction*/
        return TRUE;
    }
    if(strcmp(line->instruction, "entry") == 0 || strcmp(line->instruction, "extern") == 0)
    {
        while(!end_of_string(data_to_extract))/*more entry or extern labels to add*/
        {
            while (!isspace(*data_to_extract))/*getting next label*/
            {
                token[i++] = *data_to_extract++;
            }
            token[i] = '\0';
            skip_white_spaces(&data_to_extract);
            /*if(!end_of_string(data_to_extract))
            {
                printf("Error: extra characters after entry\n");
                return FALSE;
            }*/
            
            if(strcmp(line->instruction, "entry") == 0)/*add function to add to entry list. unable to check for existens. fix*/
            {
                
                while(temp_ent->next != NULL)
                {
                    if(strcmp(temp_ent->name, token) == 0)
                    {
                        printf("Error: entry already exist\n");
                        return FALSE;
                    }
                    temp_ent = temp_ent->next;
                }
                temp_ent->name = token;
                temp_ent->address = *DC;
                temp_ent->next = (entry_entry*)malloc_or_exit(sizeof(ent));
                reset_entry(temp_ent->next);
            }
            else/*add function .... fix*/
            {
                while(temp_ext->next != NULL)
                {
                    if(strcmp(temp_ext->name, token) == 0)
                    {
                        printf("Error: extern already exist\n");
                        return FALSE;
                    }
                    temp_ext = temp_ext->next;
                }
                temp_ext->name = token;
                temp_ext->address = *DC;
                temp_ext->next = (extern_entry *)malloc_or_exit(sizeof(ext));
                reset_extern(temp_ext->next);
            }
            
        }/*end of while*/
    }
    return TRUE;
}

boolean add_symbol_to_table(line_info* line, symbols_table_entry* symbol_table, symbol_data_types data_type, long* DC, long L)
{
    symbols_table_entry* symbol_table_ptr = symbol_table;

    while (symbol_table_ptr->next != NULL)
    {
        if (strcmp(symbol_table->name, line->label) == 0)/*checking if double declaration of label*/
        {
            printf("Error: label already exist");
                return FALSE;
        }
    }
    symbol_table_ptr->data_type = data_type;
    symbol_table_ptr->address = *DC;
    symbol_table_ptr->L = L;
    symbol_table_ptr->name = line->label;
    symbol_table_ptr->next = (symbols_table_entry*)malloc_or_exit(sizeof(symbol_table));
    reset_symbol(symbol_table_ptr);
    return TRUE;
}


void reset_entry(entry_entry* ent)
{
    ent->address = 0;
    ent->name = NULL;
    ent->next = NULL;
}

void reset_extern(extern_entry* ext)
{
    ext->address = 0;
    ext->name = NULL;
    ext->next = NULL;
}

void reset_symbol(symbols_table_entry* symbol)
{
    symbol->address = 0;
    symbol->data_type = 0;
    symbol->L = 0;
    symbol->name = NULL;
    symbol->next = NULL;
}

void reset_data(data_table_entry* data)
{
    data->type = 0;
    data->data.character = '\0';
    data->data.number = 0;
    data->next = NULL;
}




void print_line_info(line_info *line)
{
    printf("is_label: %s\n", (line->is_label)? "TRUE" : "FALSE");
    printf("label: %s\n", line->label);
    printf("is_instruction: %s\n", (line->is_instruction)? "TRUE" : "FALSE");
    printf("instruction: %s\n", line->instruction);
    printf("is_data: %s\n", (line->is_data)? "TRUE" : "FALSE");
    printf("instruction_data: %s\n", line->instruction);
    printf("instruction_data: %s\n", line->instruction_data);
    printf("opcode: %s\n", line->opcode);
    printf("source_operand: %s\n", line->source_operand);
    printf("target_operand: %s\n", line->target_operand);
    printf("-------------------------\n\n");
}

void print_data_table(data_table_entry *data_table)
{
    printf("printing data_table_entry:\n");
    while(data_table->next != NULL)
    {
        if(data_table->type == TYPE_STRING)
        {
            printf("string: %c  address: %ld\n", data_table->data.character, data_table->address);
            data_table = data_table->next;
            continue;
        }
        if(data_table->type == TYPE_NUMBER)
        {
            printf("data int: %ld   address: %ld\n", data_table->data.number, data_table->address);
            data_table = data_table->next;
            continue;
        }
    }
    printf("-------------------------\n\n");

}

void print_extern_table(extern_entry *ext)
{
    printf("printing extern labels:\n");
    while(ext->next != NULL)
    {
        printf("name: %s    address: %ld\n", ext->name, ext->address);
        ext = ext->next;
    }
    printf("-------------------------\n\n");
}

void print_entry_table(entry_entry *ent)
{
    printf("printing entry table:\n");
    while(ent->next != NULL)
    {
        printf("name: %s    address: %ld\n", ent->name, ent->address);
        ent = ent->next;
    }
    printf("-------------------------\n\n");
}

void print_symbol_table(symbols_table_entry *symbol_table)
{
    printf("printing symbol table:\n");
    while (symbol_table->next != NULL)
    {
        printf("name: %s\n", symbol_table->name);
        printf("address: %ld\n", symbol_table->address);
        printf("length (L): %ld\n", symbol_table->L);
        printf("is_data: %s\n", (symbol_table->is_data) ? "TRUE" : "FALSE");
        printf("is_external: %s\n", (symbol_table->type == TYPE_EXTERN) ? "TRUE" : "FALSE");
        printf("is_entry: %s\n", (symbol_table->type == TYPE_ENTRY) ? "TRUE" : "FALSE");
        symbol_table = symbol_table->next;
    }
    printf("-------------------------\n\n");
}