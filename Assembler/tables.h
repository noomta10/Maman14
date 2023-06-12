#define ADD_NODE_TO_LIST(prev, ptr, head) \
/*seting the list root if list is empty*/ \
if (!*(head)) \
	*(head) = (ptr);  \
/*adding the node to the list*/ \
if(prev) \
	(prev)->next = (ptr); \
(prev) = (ptr);\
(ptr) = (ptr)->next = NULL;

#define SET_PREV_POINTER(prev, head_ptr) \
while(head_ptr) \
{ \
	(prev) = (head_ptr); \
	(head_ptr) = (head_ptr)->next; \
}

#define FREE_TABLE(current_entry, next_entry) \
/* If table is empty, return */ \
if (!current_entry) \
	return; \
/* Free all nodes */ \
while (current_entry->next != NULL) { \
	free(current_entry->name); \
	next_entry = current_entry->next; \
	free(current_entry); \
	current_entry = next_entry; \
}



typedef enum {
	TYPE_STRING,
	TYPE_NUMBER
} data_types;


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
	data_value data; /* Character or number */
	data_types type; /* String or number */
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
	data_types data_type;
	data_types type;
	boolean is_data;
} symbols_table_entry;






boolean add_data_to_table(line_info* line, symbols_table_entry** symbol_table, data_table_entry** data_table, extern_entry** ext, entry_entry** ent, long* DC);
boolean add_symbol_to_table(line_info* line, symbols_table_entry** symbol_table, data_types data_type, extern_entry** ext, long* DC, long L);



void free_data_table(data_table_entry*);
void free_symbols_table(symbols_table_entry*);
void free_extern_table(extern_entry*);
void free_entry_table(entry_entry*);