#define MY_DEBUG
#ifdef MY_DEBUG
#define LOG_DEBUG(x) printf("debug %s\n",x)
#else
#define LOG_DEBUG(x) 
#endif


void print_line_info(line_info *line);
void print_symbol_table(symbols_table_entry *symbol_table);
void print_data_table(data_table_entry *data_table);
void print_extern_table(extern_entry *ext);
void print_entry_table(entry_entry *ent);
