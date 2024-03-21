#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "command_line.h"
#include "miscellaneous.h"

#define OP_SRC  "--src"
#define OP_DEST "--dest"
#define OP_MODE "--mode"
#define OP_FAC  "--factor"
#define OP_SIZE "--size"
#define OP_VB   "--verbose"
#define PITCH "pitch"
#define SPEED "speed"
#define MAX_FACTOR_VALUE 3
#define MIN_SIZE_VALUE 2205
#define MAX_SIZE_VALUE 8820

static void handle_common_task(char **, char *, int);
static void handle_src_option(char **, char *, int *);
static void handle_dest_option(char **, char *, int *);
static void handle_mode_option(struct execution_options *, char *, int *);
static void handle_factor_option(struct execution_options *, char *, int *);
static void handle_size_option(struct execution_options *, char *);
static void handle_verbose_option(struct execution_options *);

void inspect_execution_options(int argc, char **argv, struct execution_options *options) {
   int required_option_count = 0;

   argv++;  /* Skip the program name. */
   while (*argv != NULL) {
      if (strncmp(*argv, OP_SRC, strlen(OP_SRC)) == 0)
         handle_src_option(&options->src_name, *(argv + 1), &required_option_count);
      else if (strncmp(*argv, OP_DEST, strlen(OP_DEST)) == 0)
         handle_dest_option(&options->dest_name, *(argv + 1), &required_option_count);
      else if (strncmp(*argv, OP_MODE, strlen(OP_MODE)) == 0)
         handle_mode_option(options, *(argv + 1), &required_option_count);
      else if (strncmp(*argv, OP_FAC, strlen(OP_FAC)) == 0)
         handle_factor_option(options, *(argv + 1), &required_option_count);
      else if (strncmp(*argv, OP_SIZE, strlen(OP_SIZE)) == 0)
         handle_size_option(options, *(argv + 1));
      else if (strncmp(*argv, OP_VB, strlen(OP_VB)) == 0)
         handle_verbose_option(options);
      argv++;
   }

   if (required_option_count == 4)
      return;

   printf("Usage: ./process\n"
          "  --src            The name of the input .wav file.\n"
          "  --dest           The name of the output .wav file.\n"
          "  --mode           pitch: Modify pitch, meanwhile keeping speed the same.\n"
          "                   speed: Modify speed, meanwhile keeping pitch the same.\n"
          "  --factor         In case of pitch, the value of 2 would yield 1 octave high.\n"
          "                   In case of speed, the value of 2 would yield the doubled length.\n"
          "  [--size]         Assign a specific grain size.\n"
          "  [--verbose]      Show input .wav file metadata.\n\n");
   raise_err("Insufficient argument count: Expected 4, Current %d\n"
             "(Example) ./process --verbose --src in.wav --dest "
             "out.wav --mode pitch --factor 1.06 --size 2205",
             required_option_count);
}

static void handle_common_task(char **dest, char *src, int limit) {
   if (src == NULL) raise_err("Failed to get data for an option.");
   *dest = malloc(limit);
   if (*dest == NULL) raise_err("Failed to allocate memory dynamically.");
   strncpy(*dest, src, limit);
}

static void handle_src_option(char **dest, char *src, int *counter) {
   handle_common_task(dest, src, FILENAME_MAX);
   (*counter)++;
}

static void handle_dest_option(char **dest, char *src, int *counter) {
   handle_common_task(dest, src, FILENAME_MAX);
   (*counter)++;
}

static void handle_mode_option(struct execution_options *options, char *src, int *counter) {
   handle_common_task(&options->_mode, src, 6);
   options->_mode[5] = '\0';
   if (strncmp(options->_mode, PITCH, 5) == 0)
      options->mode = 1;
   else if (strncmp(options->_mode, SPEED, 5) == 0)
      options->mode = 2;
   else
      raise_err("An invalid %s value: %s.", OP_MODE, options->_mode);
   (*counter)++;
}

static void handle_factor_option(struct execution_options *options, char *src, int *counter) {
   char *indicator;
   
   handle_common_task(&options->_factor, src, 5);
   options->_factor[4] = '\0';
   errno = 0;
   options->factor = strtod(options->_factor, &indicator);
   if (indicator == options->_factor)
      raise_err("An invalid %s value: %s.", OP_FAC, options->_factor);
   if (errno == ERANGE)
      raise_err("An invalid %s value: %s.", OP_FAC, options->_factor);
   if (options->factor < 0 || options->factor > MAX_FACTOR_VALUE)
      raise_err("A %s value out of range: %s.", OP_FAC, options->_factor);
   (*counter)++;
}

static void handle_size_option(struct execution_options *options, char *src) {
   handle_common_task(&options->_size, src, 5);
   options->_size[4] = '\0';
   options->size = atoi(options->_size);
   if (options->size == 0)
      raise_err("An invalid %s value: %s.", OP_SIZE, options->_size);
   if (options->size < MIN_SIZE_VALUE || options->size > MAX_SIZE_VALUE)
      raise_err("A %s value out of range: %s.", OP_SIZE, options->_size);
}

static void handle_verbose_option(struct execution_options *options) {
   options->verbose = true;
}