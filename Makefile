all: main clean

main: main.o read_and_write.o functions.o
	gcc main.o read_and_write.o large_functions.o -o myapp -lpng

main.o: main.c read_and_write.o large_functions.o read_and_write.h large_functions.h
	gcc -c main.c

input.o: read_and_write.c read_and_write.h
	gcc -c read_and_write.c

functions.o: large_functions.c large_functions.h
	gcc -c large_functions.c

clean:
	rm -f *.o
