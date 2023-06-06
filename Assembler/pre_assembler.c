#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "tables.h"
#include "debuging.h"
#include "pre_assembler.h"
#include "utils.h"


static void free_table_memory(mcros_table_entry** first_mcro_entry) {
	mcros_table_entry* current_mcro_entry = *first_mcro_entry;
	mcros_table_entry* next_mcro_entry;
	
	/* Free mcros table */
	while (current_mcro_entry->next != NULL) {
		next_mcro_entry = current_mcro_entry->next;
		free(current_mcro_entry->name);
		free(current_mcro_entry->value);
		free(current_mcro_entry);
		current_mcro_entry = next_mcro_entry;
	} 

	/* Free last mcro entry */
	free(current_mcro_entry->name);
	free(current_mcro_entry->value);
	free(current_mcro_entry);

}


static void rename_file(char* file_name) {
	char* new_file_name = add_file_postfix(file_name, ".am");

	/* If file with '.am' postfix exists, remove it */
	FILE* new_file = fopen(new_file_name, "r");
	if (new_file != NULL) {
		fclose(new_file);
		remove(new_file_name);
	}

	(void)rename("template_file.txt", new_file_name);
}


static boolean handle_existing_mcro(mcros_table_entry** first_mcro_entry, char* first_word, FILE* template_file) {
	mcros_table_entry* current_mcro_entry = *first_mcro_entry;

	/* If the mcros table is empty, return FALSE */
	if (first_mcro_entry == NULL) {
		return FALSE;
	}

	/* Loops through the table to check if the first word is a defined macro in the table */
	while (current_mcro_entry) {
		/* If the first word is a defined macro in the table, write its value to the template file, and return TRUE */
		if (strcmp(first_word, current_mcro_entry->name) == 0) {
			fprintf(template_file, "%s", current_mcro_entry->value);
			return TRUE;
		}

		current_mcro_entry = current_mcro_entry->next;
	}

	return FALSE;
}


static void add_mcro_to_table(char* mcro_name, char* mcro_value, mcros_table_entry** first_mcro_entry) {
	mcros_table_entry* new_mcro_entry = malloc_with_check(sizeof(mcros_table_entry));
	mcros_table_entry* current_mcro_entry;
	int total_length;

	/* If table is empty, add first mcro */
	if (*first_mcro_entry == NULL) {
		new_mcro_entry->name = malloc_with_check(strlen(mcro_name) + 1);
		new_mcro_entry->value = malloc_with_check(strlen(mcro_value) + 1);
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
	new_mcro_entry->name = malloc_with_check(strlen(mcro_name) + 1);
	new_mcro_entry->value = malloc_with_check(strlen(mcro_value) + 1);
	new_mcro_entry->next = NULL;
	strcpy(new_mcro_entry->name, mcro_name);
	strcpy(new_mcro_entry->value, mcro_value);
	current_mcro_entry->next = new_mcro_entry;
	current_mcro_entry = new_mcro_entry;
}


void pre_assembler(FILE* source_file, char* file_name) {
	char line[MAX_LINE_LENGTH];
	mcros_table_entry* first_mcro_entry = NULL;
	char* mcro_name = NULL;
	char* saved_line = NULL;
	char* saved_mcro_name = NULL;
	char* first_word;

	LOG_DEBUG("pre assember starting");

	/* Create an empty file, close it, an reopen it in append mode */
	FILE* template_file = fopen("template_file.txt", "w");
	if (template_file == NULL) {
		printf("Error: The file 'template_file.txt' could not be opened\n");
		return;
	}

	fclose(template_file);
	template_file = fopen("template_file.txt", "a");
	if (template_file == NULL) {
		printf("Error: The file 'template_file.txt' could not be opened\n");
		return;
	}

	/* Read file line by line until the end */
	while (fgets(line, sizeof(line), source_file) != NULL) {
		
		/* Allocate memory for the line, and copy it */
		saved_line = malloc_with_check(sizeof(line));
		strcpy(saved_line, line);
		first_word = strtok(line, " \t\n");

		/* If it is an empty line, print it to the template file continue to next line */
		if (first_word == NULL) {
			fprintf(template_file, "%s", "\n");
			continue;
		}

		/* If first word is a defined mcro in the table, write its value to the template file and continue to next line */
		if (handle_existing_mcro(&first_mcro_entry, first_word, template_file)){
			continue;
	    }

		/* If its the beginning of a mcro definition */
		if (strcmp(first_word, "mcro") == 0) {
			mcro_name = strtok(NULL, " \t\n");

			/* Allocate memory for the mcro name, and copy it */
			saved_mcro_name = malloc_with_check(strlen(mcro_name) + 1);
			strcpy(saved_mcro_name, mcro_name); 

			/* Mcros definition, adds its value to the table */
			while (TRUE) {
				LOG_DEBUG("pre assember loop on macros");
				fgets(line, sizeof(line), source_file);
				saved_line = malloc_with_check(sizeof(line));
				strcpy(saved_line, line);
				first_word = strtok(line, " \t\n");
				LOG_DEBUG(first_word);
				/* Stop if "endmcro" encountered */
				if (strcmp(first_word, "endmcro") == 0) {
					break;
				}

				add_mcro_to_table(saved_mcro_name, saved_line, &first_mcro_entry);
			}

			continue;
		}

		/* If the line is not a mcro name or definition, write it to the template file */
		fprintf(template_file, "%s", saved_line);
	}

	/* Close the file */
	fclose(template_file);

	/* Rename template file to '.am' file */
	rename_file(file_name);

	/* free allocated memory */
	free(saved_line);
	free(saved_mcro_name);
	free_table_memory(&first_mcro_entry);

	LOG_DEBUG("pre assember done");
}
