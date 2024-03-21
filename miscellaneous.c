#include <stdio.h>
#include <stdlib.h>
#include "miscellaneous.h"

void raise_err(char *err_msg, ...) {
   va_list ap;
   
   va_start(ap, err_msg);
   vfprintf(stderr, err_msg, ap);
   va_end(ap);
   fprintf(stderr, "\n");
   exit(EXIT_FAILURE);
}

int get_endianness(void) {
   int test = 1;
   char *test_ptr = (char *) &test;

   /* 0 for big-endian; 1 for little. */
   return test_ptr[0] == 0 ? 0 : 1;
}

extern void endrev16(uint16_t *);
extern void endrev32(uint32_t *);