process: main.o processing.o miscellaneous.o
	gcc -o process main.o processing.o miscellaneous.o -lm

main.o: main.c processing.h miscellaneous.h
	gcc -c main.c

processing.o: processing.c processing.h
	gcc -c processing.c

miscellaneous.o: miscellaneous.c miscellaneous.h
	gcc -c miscellaneous.c