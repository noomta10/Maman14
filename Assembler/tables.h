

typedef enum {
	TYPE_STRING,
	TYPE_NUMBER,

	/*unused so for*/
	TYPE_ENTRY,
	TYPE_EXTERN
} data_types, symbol_data_types;


typedef union{
	char character;
	long number;
} data_value;

typedef enum{/*unused so far*/
	EXTERNAL,
	RELOCATEABLE
} symbol_type;


typedef struct data_table_entry{/*data table for data values*/
	struct data_table_entry *next;
	long address; /*address of DC*/
	data_value data;
	data_types type;	
} data_table_entry;


typedef struct extern_entry{/*data talel for extern flags */
	struct extern_entry *next;
	long address;
	char *name;
} extern_entry;

typedef struct entry_entry{/*data table for entry flags */
	struct entry_entry *next;
	long address;
	char *name;
} entry_entry;

typedef struct symbols_table_entry{/*struct for the symbols table*/
	struct symbols_table_entry *next;/*pointer to the next entry*/
	long address;/*address of IC or DC*/
	long L;/*if data type, length of data*/
	char * name;
	symbol_data_types data_type;
	data_types type;
	boolean is_data;
} symbols_table_entry;






boolean add_data_to_table(line_info* line, symbols_table_entry* symbol_table, data_table_entry* data_table, extern_entry* ext, entry_entry* ent, long* DC, boolean* error_in_code);
boolean add_symbol_to_table(line_info* line, symbols_table_entry* symbol_table, symbol_data_types data_type, long* DC, long L);

void reset_data(data_table_entry* data);
void reset_extern(extern_entry *ext);
void reset_entry(entry_entry *ent);
void reset_symbol(symbols_table_entry* symbol);
void reset_line_info(line_info *line);