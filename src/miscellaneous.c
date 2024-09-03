#include <stdio.h>
#include <stdlib.h>
#include "miscellaneous.h"

extern void endrev16(uint16_t *);
extern void endrev32(uint32_t *);

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

int count_digit(uint32_t number) {
   int count = 1;

   while (number /= 10)
      count++;
   
   return count;
}

static void move_cursor_to_line_start(int n) {
   for (int i = 0; i < n; i++)
      putchar('\b');
}

void print_progress_bar(uint32_t current, uint32_t total, int total_digit) {
   int i;
   int progress_percent = (current * 100 / total);
   int progress = progress_percent / 5;
   int remains = 20 - progress;

   /* In progress: [***************-----] _75% (_750/1000) */

   /* printf("\033[%dD", 44 + total_digit * 2); */
   move_cursor_to_line_start(44 + total_digit * 2);
   fputs("In progress: [", stdout);
   for (i = 0; i < progress; i++)
      putchar('*');
   for (i = 0; i < remains; i++)
      putchar('-');
   printf("] %3d%% (%*d/%d)",
          progress_percent, total_digit, current, total);
}