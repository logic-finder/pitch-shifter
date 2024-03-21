process: main.o wave_file.o execution_options.o \
			command_line.o processing.o miscellaneous.o
	gcc -o process main.o wave_file.o execution_options.o \
	command_line.o processing.o miscellaneous.o

main.o: main.c wave_file.h execution_options.h \
		  command_line.h processing.h miscellaneous.h
	gcc -c main.c

wave_file.o: wave_file.c wave_file.h miscellaneous.h
	gcc -c wave_file.c

execution_options.o: execution_options.c execution_options.h \
							miscellaneous.h
	gcc -c execution_options.c

command_line.o: command_line.c command_line.h
	gcc -c command_line.c

processing.o: processing.c processing.h miscellaneous.h
	gcc -c processing.c

miscellaneous.o: miscellaneous.c miscellaneous.h
	gcc -c miscellaneous.c