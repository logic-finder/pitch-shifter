#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "command_line.h"
#include "miscellaneous.h"

#define OP_SRC          "--src"
#define OP_SRC_ABBR     "-S"
#define OP_DEST         "--dest"
#define OP_DEST_ABBR    "-D"
#define OP_PITCH        "--pitch"
#define OP_PITCH_ABBR   "-P"
#define OP_SPEED        "--speed"
#define OP_SPEED_ABBR   "-T"
#define OP_SIZE         "--size"
#define OP_VB           "--verbose"
#define OP_HELP         "--help"
#define SUPPRESSION_CHAR      '*'
#define SUPPRESSION_OCCURRED   1
#define MAX_FACTOR_VALUE       3
#define MIN_SIZE_VALUE  2205
#define MAX_SIZE_VALUE  8820

static void handle_help_option(void);
static void handle_src_option(
   struct execution_options *,
   char **,
   unsigned int *,
   bool);
static void handle_dest_option(
   struct execution_options *,
   char **,
   unsigned int *,
   bool);
static void handle_pitch_option(
   struct execution_options *,
   char *,
   unsigned int *,
   bool);
static void handle_speed_option(
   struct execution_options *,
   char *,
   unsigned int *,
   bool);
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
         handle_src_option(options, argv, &checklist, false);
         argv++;
      }
      else if (strncmp(*argv, OP_SRC_ABBR, strlen(OP_DEST_ABBR)) == 0) {
         handle_src_option(options, argv, &checklist, true);
         argv++;
      }
      else if (strncmp(*argv, OP_DEST, strlen(OP_DEST)) == 0) {
         handle_dest_option(options, argv, &checklist, false);
         argv++;
      }
      else if (strncmp(*argv, OP_DEST_ABBR, strlen(OP_DEST_ABBR)) == 0) {
         handle_dest_option(options, argv, &checklist, true);
         argv++;
      }
      else if (strncmp(*argv, OP_PITCH, strlen(OP_PITCH)) == 0) {
         handle_pitch_option(options, *(argv + 1), &checklist, false);
         argv++;
      }
      else if (strncmp(*argv, OP_PITCH_ABBR, strlen(OP_PITCH_ABBR)) == 0) {
         handle_pitch_option(options, *(argv + 1), &checklist, true);
         argv++;
      }
      else if (strncmp(*argv, OP_SPEED, strlen(OP_SPEED)) == 0) {
         handle_speed_option(options, *(argv + 1), &checklist, false);
         argv++;
      }
      else if (strncmp(*argv, OP_SPEED_ABBR, strlen(OP_SPEED_ABBR)) == 0) {
         handle_speed_option(options, *(argv + 1), &checklist, true);
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
          "     -S            Equivalent to --src.\n"
          "  --dest           The name of the output .wav file.\n"
          "      -D           Equivalent to --dest.\n"
          "  --pitch          Modify pitch, meanwhile keeping speed the same.\n"
          "                   The value of 2 would yield 1 octave high.\n"
          "       -P          Equivalent to --pitch.\n"
          "  --speed          Modify speed, meanwhile keeping pitch the same.\n"
          "                   The value of 2 would yield the doubled length.\n"
          "       -T          Equivalent to --speed.\n"
          "  [--size]         Assign a specific grain size.\n"
          "  [--verbose]      Display the metadata of the input .wav file.\n\n");
   printf("(Example) ./process --src in.wav --dest out.wav --pitch 0.8\n");
}

/* Note: last character */
inline static int lastch(const char *s) {
   return s[strlen(s) - 1];
}

static int detect_suppression(
   char *option_name,
   struct execution_options *options,
   char **argv
) {
   char *input_option_name = argv[0];
   char *input_option_value = argv[1];
   int difference
      = strlen(input_option_name) - strlen(option_name);
   
   if (input_option_value == NULL)
      raise_err("Failed to get data for this option: %s.", option_name);
   if (difference > 1)
      handle_unknown_argument(input_option_name);
   if (difference == 1)
      if (lastch(input_option_name) != SUPPRESSION_CHAR)
         handle_unknown_argument(input_option_name);
      else
         return SUPPRESSION_OCCURRED;

   return 0;
}

static void handle_src_option(
   struct execution_options *options,
   char **argv,
   unsigned int *checklist,
   bool is_abbreviated
) {
   int result;

   if (!is_abbreviated)
      result = detect_suppression(OP_SRC, options, argv);
   else
      result = detect_suppression(OP_SRC_ABBR, options, argv);
   if (result == SUPPRESSION_OCCURRED)
      options->suppress_src_path = true;
   options->src_name = argv[1];
   *checklist |= 1 << 0;
}

static void handle_dest_option(
   struct execution_options *options,
   char **argv,
   unsigned int *checklist,
   bool is_abbreviated
) {
   int result;

   if (!is_abbreviated)
      result = detect_suppression(OP_DEST, options, argv);
   else
      result = detect_suppression(OP_DEST_ABBR, options, argv);
   if (result == SUPPRESSION_OCCURRED)
      options->suppress_dest_path = true;
   options->dest_name = argv[1];
   *checklist |= 1 << 1;
}

static void get_factor_value(
   char *option_name,
   char *src,
   struct execution_options *options
) {
   char *indicator;

   if (src == NULL)
      raise_err("Failed to get data for this option: %s.", option_name);
   errno = 0;
   options->factor = strtod(src, &indicator);
   if (indicator == src)
      raise_err("An invalid %s value: %s.", option_name, src);
   if (errno == ERANGE)
      raise_err("An invalid %s value: %s.", option_name, src);
   if (options->factor < 0 || options->factor > MAX_FACTOR_VALUE)
      raise_err("A %s value out of range: %s.", option_name, src);
}

static void handle_pitch_option(
   struct execution_options *options,
   char *src,
   unsigned int *checklist,
   bool is_abbreviated
) {
   char *option_name;

   if (!is_abbreviated) option_name = OP_PITCH;
   else option_name = OP_PITCH_ABBR;

   get_factor_value(option_name, src, options);
   options->mode = 1;
   *checklist |= 1 << 2;
}

static void handle_speed_option(
   struct execution_options *options,
   char *src,
   unsigned int *checklist,
   bool is_abbreviated
) {
   char *option_name;

   if (!is_abbreviated) option_name = OP_SPEED;
   else option_name = OP_PITCH_ABBR;
   
   get_factor_value(option_name, src, options);
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