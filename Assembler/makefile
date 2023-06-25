assembler: assembler.o pre_assembler.o utils.o first_pass.o string_handling.o tables.o debuging.o info_check.o second_pass.o encoding.o write_files.o
	gcc -g -ansi -Wall -pedantic assembler.o pre_assembler.o utils.o first_pass.o string_handling.o tables.o debuging.o info_check.o second_pass.o encoding.o write_files.o -o assembler 

assembler.o:  assembler.c assembler.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o

pre_assembler.o:  pre_assembler.c pre_assembler.h
	gcc -c -g -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o

utils.o:  utils.c utils.h
	gcc -c -g -ansi -Wall -pedantic utils.c -o utils.o

first_pass.o:  first_pass.c first_pass.h
	gcc -c -g -ansi -Wall -pedantic first_pass.c -o first_pass.o

string_handling.o:  string_handling.c string_handling.h
	gcc -c -g -ansi -Wall -pedantic string_handling.c -o string_handling.o

tables.o:  tables.c tables.h
	gcc -c -g -ansi -Wall -pedantic tables.c -o tables.o

debuging.o:  debuging.c debuging.h
	gcc -c -g -ansi -Wall -pedantic debuging.c -o debuging.o

info_check.o:  info_check.c info_check.h
	gcc -c -g -ansi -Wall -pedantic info_check.c -o info_check.o

second_pass.o:  second_pass.c second_pass.h
	gcc -c -g -ansi -Wall -pedantic second_pass.c -o second_pass.o

encoding.o:  encoding.c encoding.h
	gcc -c -g -ansi -Wall -pedantic encoding.c -o encoding.o

write_files.o:  write_files.c write_files.h
	gcc -c -g -ansi -Wall -pedantic write_files.c -o write_files.o

clean:
	rm -f assembler.o pre_assembler.o utils.o assembler *.o