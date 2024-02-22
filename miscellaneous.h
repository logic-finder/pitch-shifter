#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

struct execution_options {
   char *src_name;
   char *dest_name;
   int mode;
   double factor;
   int size;
   bool verbose;
   /**********/
   char *__mode;
   char *__factor;
   char *__size;
   /**********/
   void (*cleanup_execution_options)(struct execution_options *);
   void *(*_iterate_execution_options)(struct execution_options *, int);
   struct execution_options *self;
   int length;
};

/*
 * raise_err: This funciton prints an error to the
 * stderr stream.
 */
void raise_err(char *, ...);

/*
 * get_endianness: This function checks which endianness
 * this machine follows. The return value 0 means big
 * endian, while 1 means little endian.
 */
int get_endianness(void);

/*
 * validate_execution: This function checks the command line
 * arguments.
 */
void validate_execution(int, char **, struct execution_options *);

/*
 * open_wav: This function opens two streams for
 * the input wav file and the output wave file
 * before processings are to take place.
 */
void open_wav(struct execution_options *, FILE **, FILE **);

/*
 * close_wav: This function closes two streams for
 * the input wav file and the output wave file
 * after all processings have been done.
 */
void close_wav(FILE *, FILE *);

/*
 * realize_execution_options: This function creates
 * a new struct execution_options.
 */
struct execution_options *realize_execution_options(void);

#endif