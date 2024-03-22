#ifndef EXECUTION_OPTIONS_H
#define EXECUTION_OPTIONS_H

#include <stdbool.h>

#define _LEN_EXECUTION_OPTIONS 0  /* except self */

struct execution_options {
   char *src_name;
   char *dest_name;
   int mode;
   double factor;
   int size;
   bool verbose;
   
   void (*unrealize)(struct execution_options *);
   
   /* fields to be freed */
   struct execution_options *self;
};

/*
 * realize_execution_options: This function creates
 * a new struct execution_options.
 */
struct execution_options *realize_execution_options(void);

#endif