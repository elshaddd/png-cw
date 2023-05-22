all: main clean

main: main.o read_and_write.o functions.o
	gcc main_make.o read_and_write.o functions.o -o myapp -lpng

main.o: main_make.c read_and_write.o functions.o read_and_write.h functions.h
	gcc -c main_make.c

input.o: read_and_write.c read_and_write.h
	gcc -c read_and_write.c

functions: functions.c functions.h
	gcc -c functions.c

clean:
	rm -f *.o
