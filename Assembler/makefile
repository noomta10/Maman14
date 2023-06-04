assembler: assembler.o pre_assembler.o utils.o 
	gcc -ansi -Wall -pedantic assembler.o pre_assembler.o utils.o  -o assembler 

assembler.o:  assembler.c assembler.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

pre_assembler.o:  pre_assembler.c pre_assembler.h
	gcc -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o

check_syntax.o:  utils.c utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o

clean:
	rm -f assembler.o pre_assembler.o utils.o assembler
