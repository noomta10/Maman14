
typedef struct {
    char *label;
    char *instruction;
    char *opcode;
    char *source_operand;
    char *target_operand;
    char *instruction_data;
	boolean comma_flag;
    boolean label_flag;
    boolean data_flag;
    boolean instruction_flag;
    boolean extra_chars_flag;
} line_info;



typedef enum {
	TYPE_STRING,
	TYPE_NUMBER,
	DEFAULT
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


typedef struct extern_entry{/*data table for extern flags */
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






boolean add_data_to_table(line_info* line, symbols_table_entry** symbol_table, data_table_entry** data_table, extern_entry** ext, entry_entry** ent, long* DC);
boolean add_symbol_to_table(line_info* line, symbols_table_entry** symbol_table, symbol_data_types data_type, extern_entry** ext, long* DC, long L);

void reset_line_info(line_info *line);

void free_data_table(data_table_entry*);
void free_symbols_table(symbols_table_entry*);
void free_extern_table(extern_entry*);
void free_entry_table(entry_entry*);