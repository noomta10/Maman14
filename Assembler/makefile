assembler: assembler.o pre_assembler.o utils.o first_pass.o string_handling.o tables.o debuging.o
	gcc -ansi -Wall -pedantic assembler.o pre_assembler.o utils.o first_pass.o debuging.o string_handling.o tables.o -o assembler 

assembler.o:  assembler.c assembler.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

pre_assembler.o:  pre_assembler.c pre_assembler.h
	gcc -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o

utils.o:  utils.c utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o

first_pass.o:  first_pass.c first_pass.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o

string_handling.o:  string_handling.c string_handling.h
	gcc -c -ansi -Wall -pedantic string_handling.c -o string_handling.o

tables.o:  tables.c tables.h
	gcc -c -ansi -Wall -pedantic tables.c -o tables.o

debuging.o:  debuging.c debuging.h
	gcc -c -ansi -Wall -pedantic debuging.c -o debuging.o

clean:
	rm -f assembler.o pre_assembler.o utils.o assembler *.o
