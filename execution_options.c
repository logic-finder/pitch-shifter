#include <stdlib.h>
#include "execution_options.h"
#include "miscellaneous.h"

#define DEFAULT_SIZE 2205

static void *iterate(struct execution_options *, int);
static void unrealize(struct execution_options *);

struct execution_options *realize_execution_options(void) {
   struct execution_options *objptr;

   objptr = malloc(sizeof(struct execution_options));
   if (objptr == NULL)
      raise_err("Failed to create a new struct execution_options.");
   objptr->size = DEFAULT_SIZE;
   objptr->verbose = false;
   objptr->self = objptr;
   objptr->unrealize = unrealize;

   return objptr;
};

static void *iterate(struct execution_options *objptr, int idx) {
   switch (idx) {
      case 0: return objptr->src_name;
      case 1: return objptr->dest_name;
      case 2: return objptr->_mode;
      case 3: return objptr->_factor;
      case 4: return objptr->_size;
      default:
         raise_err("Failed to access the field of struct execution_options.");
   }
}

static void unrealize(struct execution_options *objptr) {
   int i;

   for (i = 0; i < _LEN_EXECUTION_OPTIONS; i++)
      free(iterate(objptr, i));
   free(objptr->self);
}