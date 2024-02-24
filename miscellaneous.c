#include <string.h>
#include <errno.h>
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

static void _handle_common_task(char **dest, char *src, int limit) {
   if (src == NULL) raise_err("Failed to get data for an option.");
   *dest = malloc(limit);
   if (*dest == NULL) raise_err("Failed to allocate memory dynamically.");
   strncpy(*dest, src, limit);
}

static void handle_src_option(char **dest, char *src, int *counter) {
   _handle_common_task(dest, src, FILENAME_MAX);
   (*counter)++;
}

static void handle_dest_option(char **dest, char *src, int *counter) {
   _handle_common_task(dest, src, FILENAME_MAX);
   (*counter)++;
}

static void handle_mode_option(struct execution_options *options, char *src, int *counter) {
   _handle_common_task(&options->__mode, src, 6);
   options->__mode[5] = '\0';
   if (strncmp(options->__mode, PITCH, 5) == 0)
      options->mode = 1;
   else if (strncmp(options->__mode, SPEED, 5) == 0)
      options->mode = 2;
   else
      raise_err("An invalid %s value: %s.", OP_MODE, options->__mode);
   (*counter)++;
}

static void handle_factor_option(struct execution_options *options, char *src, int *counter) {
   char *indicator;
   
   _handle_common_task(&options->__factor, src, 5);
   options->__factor[4] = '\0';
   errno = 0;
   options->factor = strtod(options->__factor, &indicator);
   if (indicator == options->__factor)
      raise_err("An invalid %s value: %s.", OP_FAC, options->__factor);
   if (errno == ERANGE)
      raise_err("An invalid %s value: %s.", OP_FAC, options->__factor);
   if (options->factor < 0 || options->factor > MAX_FACTOR_VALUE)
      raise_err("A %s value out of range: %s.", OP_FAC, options->__factor);
   (*counter)++;
}

static void handle_size_option(struct execution_options *options, char *src) {
   _handle_common_task(&options->__size, src, 5);
   options->__size[4] = '\0';
   options->size = atoi(options->__size);
   if (options->size == 0)
      raise_err("An invalid %s value: %s.", OP_SIZE, options->__size);
   if (options->size < MIN_SIZE_VALUE || options->size > MAX_SIZE_VALUE)
      raise_err("A %s value out of range: %s.", OP_SIZE, options->__size);
}

static void handle_verbose_option(struct execution_options *options) {
   options->verbose = true;
}

void validate_execution(int argc, char **argv, struct execution_options *options) {
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

void open_wav(struct execution_options *options, FILE **src, FILE **dest) {
   *src = fopen(options->src_name, "rb");
   if (*src == NULL) raise_err("Failed to open the file.");
   *dest = fopen(options->dest_name, "wb");
   if (dest == NULL) raise_err("Failed to open the file.");
}

void close_wav(FILE *src, FILE *dest) {
   int result;

   result = fclose(src);
   if (result == EOF) raise_err("Failed to close the file.");
   result = fclose(dest);
   if (result == EOF) raise_err("Failed to close the file.");
}

static void *_iterate_execution_options(struct execution_options *obj, int idx) {
   switch (idx) {
      case 0: return obj->src_name;
      case 1: return obj->dest_name;
      case 2: return obj->__mode;
      case 3: return obj->__factor;
      case 4: return obj->__size;
   }
   raise_err("Failed to access the field of struct execution_options.");
}

static void cleanup_execution_options(struct execution_options *obj) {
   int i;

   for (i = 0; i < obj->length; i++)
      free(obj->_iterate_execution_options(obj, i));
   free(obj);
}

struct execution_options *realize_execution_options(void) {
   struct execution_options *object_ptr;

   object_ptr = malloc(sizeof(struct execution_options));
   if (object_ptr == NULL)
      raise_err("Failed to create a new struct execution_options.");
   object_ptr->size = MIN_SIZE_VALUE;
   object_ptr->verbose = false;
   object_ptr->length = 5;
   object_ptr->self = object_ptr;
   object_ptr->_iterate_execution_options = _iterate_execution_options;
   object_ptr->cleanup_execution_options = cleanup_execution_options;

   return object_ptr;
};