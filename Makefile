process: main.o wave_file.o miscellaneous.o execution_options.o \
		env_data.o command_line.o envfile_reader.o processing.o
	gcc -O -Wall -W -pedantic -o process \
	main.o wave_file.o miscellaneous.o execution_options.o \
	env_data.o command_line.o envfile_reader.o processing.o

main.o: main.c wave_file.h miscellaneous.h execution_options.h \
		env_data.h command_line.h envfile_reader.h processing.h 
	gcc -c main.c

wave_file.o: wave_file.c wave_file.h miscellaneous.h
	gcc -c wave_file.c

miscellaneous.o: miscellaneous.c miscellaneous.h
	gcc -c miscellaneous.c

execution_options.o: execution_options.c execution_options.h \
		miscellaneous.h
	gcc -c execution_options.c

env_data.o: env_data.c env_data.h miscellaneous.h
	gcc -c env_data.c

command_line.o: command_line.c command_line.h miscellaneous.h
	gcc -c command_line.c

envfile_reader.o: envfile_reader.c envfile_reader.h \
		execution_options.h miscellaneous.h env_data.h
	gcc -c envfile_reader.c

processing.o: processing.c processing.h miscellaneous.h
	gcc -c processing.c