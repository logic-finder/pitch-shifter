#ifndef EXECUTION_OPTIONS_H
#define EXECUTION_OPTIONS_H

#include <stdbool.h>

#define _LEN_EXECUTION_OPTIONS 5

struct execution_options {
   int mode;
   double factor;
   int size;
   bool verbose;
   
   void (*unrealize)(struct execution_options *);
   
   /* fields to be freed */
   char *src_name;
   char *dest_name;
   char *_mode;
   char *_factor;
   char *_size;
   struct execution_options *self;
};

/*
 * realize_execution_options: This function creates
 * a new struct execution_options.
 */
struct execution_options *realize_execution_options(void);

#endif