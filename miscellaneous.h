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
 * 
 */
void validate_execution(int, char **, struct execution_options *);
void open_wav(struct execution_options *, FILE **, FILE **);
void close_wav(FILE *, FILE *);
struct execution_options *realize_execution_options(void);

#endif