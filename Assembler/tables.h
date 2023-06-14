#define UNINITIALIZED_VALUE 0


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


typedef struct {
	unsigned int ARE: 2;
	unsigned int target_addressing: 3;
	unsigned int opcode: 4;
	unsigned int source_addressing: 3;
} code_word;

typedef struct {
	unsigned int ARE: 2;
	unsigned int data: 10;
} extra_code_word;

typedef struct {
	unsigned int ARE: 2;
	unsigned int target_register: 5;
	unsigned int source_register: 5;
} register_word;

typedef union {
	code_word code_word_value;
	extra_code_word extra_code_word_value;
	register_word register_word_value;
} word_value;

typedef enum {
	TYPE_CODE_WORD,
	TYPE_EXTRA_CODE_WORD,
	TYPE_REGISTER_WORD
} word_type;

typedef enum {
	MOV_OPCODE = 0,
	CMP_OPCODE = 1,
	ADD_OPCODE = 2,
	SUB_OPCODE = 3,
	NOT_OPCODE = 4,
	CLR_OPCODE = 5,
	LEA_OPCODE = 6,
	INC_OPCODE = 7,
	DEC_OPCODE = 8,
	JMP_OPCODE = 9,
	BNE_OPCODE = 10,
	RED_OPCODE = 11,
	PRN_OPCODE = 12,
	JSR_OPCODE = 13,
	RTS_OPCODE = 14,	
	STOP_OPCODE = 15
} opcode_type;

typedef enum {
	NO_OPERAND = 0,
	IMMEDIATE_ADDRESSING = 1,
	DIRECT_ADDRESSING = 3,
	REGISTER_ADDRESSING = 5
} addressing_type;

typedef enum {
	ABSOLUTE = 0,
	EXTERNAL = 1,
	RELOCATABLE = 2
} ARE_type;

typedef enum {
	NO_REGISTER = 0,
	R0 = 0,
	R1 = 1,
	R2 = 2,
	R3 = 3,
	R4 = 4,
	R5 = 5,
	R6 = 6,
	R7 = 7
} register_type;

typedef enum {
	DIRECTIVE, 
	INSTRUCTION
} line_type;

typedef enum {
	TYPE_STRING,
	TYPE_NUMBER
} data_types;

typedef struct code_table_entry{/*code table for code values*/
	struct code_table_entry* next;
	word_type type;
	word_value value;
	long address;/* IC */
} code_table_entry; 


typedef union{
	char character;
	long number;
} data_value;

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
	line_type address_type;
	data_types data_type;
	boolean is_data;
} symbols_table_entry;

typedef struct uninitialized_symbols_table_entry{/*struct for the symbols table*/
	struct uninitialized_symbols_table_entry *next;/*pointer to the next entry*/
	long address;/*address of IC or DC*/
	char* name;
	extra_code_word* extra_code_word_value;
} uninitialized_symbols_table_entry;






boolean add_data_to_table(line_info* line, symbols_table_entry** symbol_table, data_table_entry** data_table, extern_entry** ext, entry_entry** ent, long* DC);
boolean add_instruction_to_table(line_info* line, symbols_table_entry** symbol_trable_head, extern_entry** ext_head, code_table_entry** code_table_head, long* IC);
boolean add_symbol_to_table(char* lable_name, symbols_table_entry** symbol_table_head, extern_entry** ext_head, line_type type, long* address, long L);


void free_data_table(data_table_entry*);
void free_symbols_table(symbols_table_entry*);
void free_extern_table(extern_entry*);
void free_entry_table(entry_entry*);
void free_code_table(code_table_entry* head);

int extra_words_for_addressing(line_info* line);

code_table_entry* get_opcode_word(line_info* line, long* IC);
code_table_entry* get_register_word(char* source_register, char* target_register, long* IC);
code_table_entry* get_extra_word(char* operand, long* IC);
