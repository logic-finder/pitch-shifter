#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "command_line.h"
#include "miscellaneous.h"

#define OP_SRC   "--src"
#define OP_DEST  "--dest"
#define OP_PITCH "--pitch"
#define OP_SPEED "--speed"
#define OP_SIZE  "--size"
#define OP_VB    "--verbose"
#define OP_HELP  "--help"
#define MAX_FACTOR_VALUE 3
#define MIN_SIZE_VALUE 2205
#define MAX_SIZE_VALUE 8820

static void handle_help_option(void);
static void handle_src_option(struct execution_options *, char *, unsigned int *);
static void handle_dest_option(struct execution_options *, char *, unsigned int *);
static void handle_pitch_option(struct execution_options *, char *, unsigned int *);
static void handle_speed_option(struct execution_options *, char *, unsigned int *);
static void handle_size_option(struct execution_options *, char *);
static void handle_verbose_option(struct execution_options *);
static void handle_unknown_argument(char *);

void inspect_execution_options(
   int argc,
   char **argv,
   struct execution_options *options
) {
   unsigned int val;
   unsigned int checklist = 0;  /* bit-field */
   int indicator = 0;

   if (argc == 1)
      raise_err(
         "Some command line arguments are required to run.\n"
         "Enter ./process --help to check such arguments.");

   argv++;  /* Skip the program name. */
   if (strncmp(*argv, OP_HELP, strlen(OP_HELP)) == 0) {
      handle_help_option();
      exit(EXIT_SUCCESS);
   }
   while (*argv != NULL) {
      if (strncmp(*argv, OP_SRC, strlen(OP_SRC)) == 0) {
         handle_src_option(options, *(argv + 1), &checklist);
         argv++;
      }
      else if (strncmp(*argv, OP_DEST, strlen(OP_DEST)) == 0) {
         handle_dest_option(options, *(argv + 1), &checklist);
         argv++;
      }
      else if (strncmp(*argv, OP_PITCH, strlen(OP_PITCH)) == 0) {
         handle_pitch_option(options, *(argv + 1), &checklist);
         argv++;
      }
      else if (strncmp(*argv, OP_SPEED, strlen(OP_SPEED)) == 0) {
         handle_speed_option(options, *(argv + 1), &checklist);
         argv++;
      }
      else if (strncmp(*argv, OP_SIZE, strlen(OP_SIZE)) == 0) {
         handle_size_option(options, *(argv + 1));
         argv++;
      }
      else if (strncmp(*argv, OP_VB, strlen(OP_VB)) == 0)
         handle_verbose_option(options);
      else
         handle_unknown_argument(*argv);
      argv++;
   }

   val = checklist & 1;
   if (val == 0) {
      indicator = 1;
      fprintf(stderr, "Failure to find the required field: %s.\n", OP_SRC);
   }
   val = checklist & 2;
   if (val == 0) {
      indicator = 1;
      fprintf(stderr, "Failure to find the required field: %s.\n", OP_DEST);
   }
   val = (checklist >> 2) & 3;
   if (val == 3) {
      indicator = 1;
      fprintf(stderr, "%s and %s can't be set both.\n", OP_PITCH, OP_SPEED);
   }
   else if (val == 0) {
      indicator = 1;
      fprintf(stderr, "At least %s or %s needs to be set.\n", OP_PITCH, OP_SPEED);
   }
   if (indicator == 1)
      exit(EXIT_FAILURE);
}

static void handle_help_option(void) {
   printf("Usage: ./process\n"
          "  --help           Display the manual that you are reading now.\n"
          "  --src            The name of the input .wav file.\n"
          "  --dest           The name of the output .wav file.\n"
          "  --pitch          Modify pitch, meanwhile keeping speed the same.\n"
          "                   The value of 2 would yield 1 octave high.\n"
          "  --speed          Modify speed, meanwhile keeping pitch the same.\n"
          "                   The value of 2 would yield the doubled length.\n"
          "  [--size]         Assign a specific grain size.\n"
          "  [--verbose]      Display the metadata of the input .wav file.\n\n");
   printf("(Example) ./process --src in.wav --dest out.wav --pitch 0.8\n");
}

static void handle_src_option(
   struct execution_options *options,
   char *src,
   unsigned int *checklist
) {
   if (src == NULL)
      raise_err("Failed to get data for this option: %s.", OP_SRC);
   options->src_name = src;
   *checklist |= 1 << 0;
}

static void handle_dest_option(
   struct execution_options *options,
   char *src,
   unsigned int *checklist
) {
   if (src == NULL)
      raise_err("Failed to get data for this option: %s.", OP_DEST);
   options->dest_name = src;
   *checklist |= 1 << 1;
}

static void handle_pitch_option(
   struct execution_options *options,
   char *src,
   unsigned int *checklist
) {
   char *indicator;

   if (src == NULL)
      raise_err("Failed to get data for this option: %s.", OP_PITCH);
   errno = 0;
   options->factor = strtod(src, &indicator);
   if (indicator == src)
      raise_err("An invalid %s value: %s.", OP_PITCH, src);
   if (errno == ERANGE)
      raise_err("An invalid %s value: %s.", OP_PITCH, src);
   if (options->factor < 0 || options->factor > MAX_FACTOR_VALUE)
      raise_err("A %s value out of range: %s.", OP_PITCH, src);
   options->mode = 1;
   *checklist |= 1 << 2;
}

static void handle_speed_option(
   struct execution_options *options,
   char *src,
   unsigned int *checklist
) {
   char *indicator;

   if (src == NULL)
      raise_err("Failed to get data for this option: %s.\n", OP_SPEED);
   errno = 0;
   options->factor = strtod(src, &indicator);
   if (indicator == src)
      raise_err("An invalid %s value: %s.", OP_SPEED, src);
   if (errno == ERANGE)
      raise_err("An invalid %s value: %s.", OP_SPEED, src);
   if (options->factor < 0 || options->factor > MAX_FACTOR_VALUE)
      raise_err("A %s value out of range: %s.", OP_SPEED, src);
   options->mode = 2;
   *checklist |= 1 << 3;
}

static void handle_size_option(struct execution_options *options, char *src) {
   if (src == NULL)
      raise_err("Failed to get data for this option: %s.\n", OP_SIZE);
   options->size = atoi(src);
   if (options->size == 0)
      raise_err("An invalid %s value: %s.", OP_SIZE, src);
   if (options->size < MIN_SIZE_VALUE || options->size > MAX_SIZE_VALUE)
      raise_err("A %s value out of range: %s.", OP_SIZE, src);
}

static void handle_verbose_option(struct execution_options *options) {
   options->verbose = true;
}

static void handle_unknown_argument(char *src) {
   raise_err("An unknown argument: %s.", src);
}