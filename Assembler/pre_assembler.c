#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "line_info.h"
#include "tables.h"
#include "debuging.h"
#include "pre_assembler.h"
#include "utils.h"


static void free_table_memory(mcros_table_entry* first_mcro_entry) {
	mcros_table_entry* current_mcro_entry = first_mcro_entry;
	mcros_table_entry* next_mcro_entry;
	
	/* Free mcros table */
	while (current_mcro_entry) {
		next_mcro_entry = current_mcro_entry->next;
		free(current_mcro_entry->name);
		free(current_mcro_entry->value);
		free(current_mcro_entry);
		current_mcro_entry = next_mcro_entry;
	} 
}


static boolean handle_existing_mcro(mcros_table_entry* first_mcro_entry, char* first_word, FILE* am_file) {
	mcros_table_entry* current_mcro_entry = first_mcro_entry;

	/* Loops through the table to check if the first word is a defined macro in the table */
	while (current_mcro_entry) {
		/* If the first word is a defined macro in the table, write its value to the .am file, and return TRUE */
		if (strcmp(first_word, current_mcro_entry->name) == 0) {
			fprintf(am_file, "%s", current_mcro_entry->value);
			return TRUE;
		}

		current_mcro_entry = current_mcro_entry->next;
	}

	return FALSE;
}


static void add_mcro_to_table(char* mcro_name, char* mcro_value, mcros_table_entry** first_mcro_entry) {
	mcros_table_entry* new_mcro_entry = (mcros_table_entry*)malloc_with_check(sizeof(mcros_table_entry));
	mcros_table_entry* current_mcro_entry;
	int total_length;

	/* If table is empty, add first mcro */
	if (*first_mcro_entry == NULL) {
		new_mcro_entry->name = (char*)malloc_with_check(strlen(mcro_name) + 1);
		new_mcro_entry->value = (char*)malloc_with_check(strlen(mcro_value) + 1);
		new_mcro_entry->next = NULL;
		strcpy(new_mcro_entry->name, mcro_name);
		strcpy(new_mcro_entry->value, mcro_value);
		*first_mcro_entry = new_mcro_entry;
		return;
	}

	current_mcro_entry = *first_mcro_entry;

	/* If mcro already exists in the table, append value to mcro */
	while (current_mcro_entry->next != NULL) {
		if (strcmp(current_mcro_entry->name, mcro_name) == 0) {
			total_length = strlen(current_mcro_entry->value) + strlen(mcro_value);
			current_mcro_entry->value = realloc_with_check(current_mcro_entry->value, (total_length + 1));
			strcpy(current_mcro_entry->value, current_mcro_entry->value);
			strcat(current_mcro_entry->value, mcro_value);
			current_mcro_entry->value[total_length] = '\0';
			return;
		}

		current_mcro_entry = current_mcro_entry->next;
	}

	/* Check last mcro */
	if (strcmp(current_mcro_entry->name, mcro_name) == 0) {
		total_length = strlen(current_mcro_entry->value) + strlen(mcro_value);
		current_mcro_entry->value = realloc_with_check(current_mcro_entry->value, (total_length + 1));
		strcpy(current_mcro_entry->value, current_mcro_entry->value);
		strcat(current_mcro_entry->value, mcro_value);
		current_mcro_entry->value[total_length] = '\0';
		return;
	}

	/* If mcro does not exist, add it to the table */
	new_mcro_entry->name = (char*)malloc_with_check(strlen(mcro_name) + 1);
	new_mcro_entry->value = (char*)malloc_with_check(strlen(mcro_value) + 1);
	new_mcro_entry->next = NULL;
	strcpy(new_mcro_entry->name, mcro_name);
	strcpy(new_mcro_entry->value, mcro_value);
	current_mcro_entry->next = new_mcro_entry;
	current_mcro_entry = new_mcro_entry;
}


static boolean handle_mcro_line(char line[], FILE* am_file, mcros_table_entry** first_mcro_entry, FILE* source_file, char* saved_line, char* file_name, char* am_file_name, boolean* error_flag, int* line_number) {
	char* first_word;
	char* mcro_name = NULL;
	char* saved_mcro_name = NULL;
	char* as_file_name;

	first_word = strtok(line, " \t\n");

	/* If it is an empty line, print it to the .am file continue to next line */
	if (first_word == NULL) {
		fprintf(am_file, "\n");
		return TRUE;
	}

	/* If first word is a defined mcro in the table, write its value to the .am file and continue to next line */
	if (handle_existing_mcro(*first_mcro_entry, first_word, am_file)) {
		return TRUE;
	}

	/* If its the beginning of a mcro definition */
	if (strcmp(first_word, "mcro") == 0) {
		mcro_name = strtok(NULL, " \t\n");

		/* Allocate memory for the mcro name, and copy it */
		saved_mcro_name = (char*)malloc_with_check(strlen(mcro_name) + 1);
		strcpy(saved_mcro_name, mcro_name);
		
		/* Check if mcro name is a reserved word */
		if (is_reserved_name(mcro_name)) {
			as_file_name = add_file_postfix(file_name, ".as");
			printf("Pre-assembler error in file \"% s\", line %d: mcro \"% s\" must not be an instruction or a directive name\n", as_file_name, *line_number, mcro_name);
			free(as_file_name);
			*error_flag = TRUE;
		}

		/* Mcros definition, adds its value to the table, add all its lines to the mcros table and continue to next line */
		while (TRUE) {
			fgets(line, MAX_LINE_LENGTH, source_file);
			(*line_number)++;
			strcpy(saved_line, line);
			LOG_DEBUG(line);
			first_word = strtok(line, " \t\n");
			/* Stop if "endmcro" was encountered */
			if (strcmp(first_word, "endmcro") == 0) {
				break;
			}

			add_mcro_to_table(saved_mcro_name, saved_line, first_mcro_entry);
		}
		
		free(saved_mcro_name);
		return TRUE;
	}

	return FALSE;
}


boolean pre_assembler(FILE* source_file, char* file_name) {
	boolean error_flag = FALSE;
	char line[MAX_LINE_LENGTH];
	int line_number = 0;
	mcros_table_entry* first_mcro_entry = NULL;
	char* saved_line = NULL;
	FILE* am_file;
	boolean line_is_mcro_related = FALSE;
	char* am_file_name = add_file_postfix(file_name, ".am");

	LOG_DEBUG("Pre-assembler start");

	/* Create an empty .am file */
	am_file = fopen(am_file_name, "w");
	if (am_file == NULL) {
		fprintf(stderr, "Error: The file '%s' could not be opened\n", am_file_name);
		return FALSE;
	}

	/* Read file line by line until the end */
	while (fgets(line, MAX_LINE_LENGTH, source_file) != NULL) {
		line_number++;
		/* Allocate memory for the line, and copy it */
		saved_line = (char*)malloc_with_check(sizeof(line));
		strcpy(saved_line, line);

		line_is_mcro_related = handle_mcro_line(line, am_file, &first_mcro_entry, source_file, saved_line, file_name, am_file_name, &error_flag, &line_number);
		/* If the line is a mcro name or definiton, handle it */
		if (line_is_mcro_related == TRUE) {
			continue;
		}

		/* If the line is not a mcro name or definition, write it to the .am file */
		fprintf(am_file, "%s", saved_line);
	}

	if (error_flag) {
		return FALSE;
	}
	/* Close the file */
	fclose(am_file);

	/* Free allocated memory */
	free(am_file_name);
	free(saved_line);

	/* If there are mcros in the table, free them */
	if (first_mcro_entry) {
		free_table_memory(first_mcro_entry);
	}

	LOG_DEBUG("pre assember done");
}
