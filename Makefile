all: main clean

main: main.o read_and_write.o functions.o
	gcc t_make.o read_and_write.o t_functions.o -o myapp -lpng

main.o: t_make.c read_and_write.o t_functions.o read_and_write.h functions.h
	gcc -c t_make.c

input.o: read_and_write.c read_and_write.h
	gcc -c read_and_write.c

functions.o: t_functions.c functions.h
	gcc -c t_functions.c

clean:
	rm -f *.o
