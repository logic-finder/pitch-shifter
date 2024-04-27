#include <stdio.h>
#include <stdlib.h>
#include "execution_options.h"
#include "miscellaneous.h"

#define LEN_EXECUTION_OPTIONS 0  /* except self */
#define DEFAULT_SIZE 2205

static void unrealize(struct execution_options *);

struct execution_options *realize_execution_options(void) {
   struct execution_options *objptr;

   objptr = malloc(sizeof(struct execution_options));
   if (objptr == NULL)
      raise_err("Failed to create a new struct execution_options.");
   objptr->self = objptr;
   objptr->unrealize = unrealize;
   objptr->size = DEFAULT_SIZE;
   objptr->verbose = false;
   objptr->suppress_src_path = false;
   objptr->suppress_dest_path = false;

   return objptr;
};

static void *iterate(struct execution_options *objptr, int idx) {
   switch (idx) {
      /* case 0: return objptr->ptr; */
      default:
         raise_err("Failed to access the field of struct execution_options.");
   }
}

static void unrealize(struct execution_options *objptr) {
   int i;

   for (i = 0; i < LEN_EXECUTION_OPTIONS; i++)
      free(iterate(objptr, i));
   free(objptr->self);
}