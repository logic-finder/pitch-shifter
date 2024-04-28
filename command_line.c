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
         "Enter ./pitsh --help to check such arguments.");

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
   printf("Usage: ./pitsh\n"
          "       --help      Display the manual that you are reading now.\n"
          "  --src or -S      The path of the input .wav file.\n"
          " --dest or -D      The path of the output .wav file.\n"
          "--pitch or -P      Modify pitch, meanwhile keeping speed the same.\n"
          "                   The value of 2 would yield 1 octave high.\n"
          "--speed or -T      Modify speed, meanwhile keeping pitch the same.\n"
          "                   The value of 2 would yield the doubled length.\n"
          " --src* / -S*      The SRC_PATH from .env file does not affect.\n"
          "--dest* / -D*      The DEST_PATH from .env file does not affect.\n"
          "     [--size]      Assign a specific grain size.\n"
          "  [--verbose]      Display the metadata of the input .wav file.\n"
          "\n"
          "<Note>\n"
          "--src and --dest are required. Also, between --pitch and --speed,\n"
          "only either one is required; can't be set together.\n"
          "--pitch and --speed value range: 0 ~ 3 (inclusive)\n"
          "--size value range: 2205 ~ 8820 (inclusive)\n"
          "\n"
          "<.env File>\n"
          "            #      Lines starting with # are comments and ignored.\n"
          "     SRC_PATH      The program will search --src file from this directory.\n"
          "    DEST_PATH      The program will save the result under this directory.\n"
          "\n"
          "<Example>\n"
          "./pitsh --src in.wav --dest out.wav --pitch 0.84\n");
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
      raise_err("%s: Failed to get data for the option %s.",
         __func__, option_name);
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
      raise_err("%s: Failed to get data for this option: %s.",
         __func__, option_name);
   errno = 0;
   options->factor = strtod(src, &indicator);
   if (indicator == src)
      raise_err("%s: An invalid %s value: %s.",
         __func__, option_name, src);
   if (errno == ERANGE)
      raise_err("%s: An invalid %s value: %s.",
         __func__, option_name, src);
   if (options->factor < 0 || options->factor > MAX_FACTOR_VALUE)
      raise_err("%s: A %s value out of range: %s.",
         __func__, option_name, src);
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
   char *indicator;

   if (src == NULL)
      raise_err("%s: Failed to get data for this option: %s.\n",
         __func__, OP_SIZE);
   errno = 0;
   options->size = (int) strtol(src, &indicator, 10);
   if (indicator == src)
      raise_err("%s: An invalid %s value: %s.",
         __func__, OP_SIZE, src);
   if (errno == ERANGE)
      raise_err("%s: An invalid %s value: %s.",
         __func__, OP_SIZE, src);
   if (options->size < MIN_SIZE_VALUE || options->size > MAX_SIZE_VALUE)
      raise_err("%s: A %s value out of range: %s.",
         __func__, OP_SIZE, src);

   printf("size: %d\n", options->size);
   exit(EXIT_FAILURE);

}

static void handle_verbose_option(struct execution_options *options) {
   options->verbose = true;
}

static void handle_unknown_argument(char *src) {
   raise_err("%s: An unknown argument: %s.", __func__, src);
}