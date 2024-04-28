#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "envfile_reader.h"
#include "execution_options.h"
#include "miscellaneous.h"
#include "env_data.h"

#define COMMENT   '#'
#define SRC_PATH  "SRC_PATH"
#define DEST_PATH "DEST_PATH"

static int read_line(char * restrict, int, FILE * restrict);
static int handle_src_path_field(struct env_data *, char *, int, int *);
static int handle_dest_path_field(struct env_data *, char *, int, int *);
static int handle_unknown_field(char *, int);

void read_env(struct env_data *env, struct execution_options *options) {
   FILE *envfile;
   int result, line_count, applied_field_count = 0;
   char *field_name, *field_value;
   char line[ENVFILE_LINE_MAX + 1];
   bool is_error = false;

   envfile = fopen(".env", "r");
   if (envfile == NULL) {
      if (options->verbose)
         printf(".env file not found; read skipped.\n");
      return;
   }

   for (line_count = 1; ; line_count++) {
      result = read_line(line, ENVFILE_LINE_MAX + 1, envfile);

      if (result == -1) {  /* read error */
         result = fclose(envfile);
         if (result == EOF)
            raise_err("Failed to close the .env file stream.");
         raise_err("An error occured while reading from the .env file stream.");
      }
      if (result == -2)    /* eof */
         break;
      if (result == -3)    /* empty line */
         continue;

      field_name = strtok(line, " \t\0");   /* I think this call will always succeed. */
      if (field_name[0] == COMMENT)
         continue;
      if (result == -4) {  /* long line */
         is_error = true;
         fprintf(stderr, "Line %d is too long to process.\n", line_count);
         continue;
      }
      if (strlen(field_name) > ENVFILE_NAME_MAX) {
         is_error = true;
         fprintf(stderr,
            "The field name at line %d is too long (> %d).\n",
            line_count, ENVFILE_NAME_MAX);
         continue;
      }

      field_value = strtok(NULL, " \t\0");

      if (strncmp(field_name, SRC_PATH, strlen(SRC_PATH)) == 0)
         is_error
            = handle_src_path_field(env, field_value, line_count, &applied_field_count);
      else if (strncmp(field_name, DEST_PATH, strlen(DEST_PATH)) == 0)
         is_error
            = handle_dest_path_field(env, field_value, line_count, &applied_field_count);
      else
         is_error = handle_unknown_field(field_name, line_count);
   }

   result = fclose(envfile);
   if (result == EOF)
      raise_err("Failed to close the .env file stream.");

   if (is_error) {
      printf("test\n");
      exit(EXIT_FAILURE);
   }

   if (options->verbose)
      printf("Successful read of .env file: total %d fields applied.\n",
         applied_field_count);
}

static int read_line(
   char * restrict s,
   int n,
   FILE * restrict stream
) {
   int i, result, ch;

   if (n < 2)
      raise_err("The second argument = %d < 2.");
   if (n > ENVFILE_LINE_MAX + 1)
      raise_err("The second argument = n > %d.", ENVFILE_LINE_MAX + 1);
   
   /* Skip (if any) white space character(s) at the front of the line. */
   while (isspace(ch = getc(stream)) && ch != '\n');
   if (ferror(stream))
      return -1;
   if (feof(stream))
      return -2;
   if (ch == '\n')
      return -3;
   ungetc(ch, stream);  /* This call is guaranteed to succeed. */

   /* Save N-1 characters at most and \0 at the last
      and discard the rest characters (if any). */
   i = 0;
   while ((ch = getc(stream)) != '\n' && ch != EOF) {
      if (i < n - 1)
         s[i] = ch;
      i++;
   }
   if (ferror(stream))
      return -3;

   if (i > n - 1)
      s[n - 1] = '\0';
   else
      s[i] = '\0';

   return i <= n - 1 ? 0 : -4;
}

static int handle_src_path_field(
   struct env_data *env,
   char *field_value,
   int line_count,
   int *applied_field_count)
{
   bool is_error;

   if (field_value == NULL) {
      fprintf(stderr,
         "The field " SRC_PATH " at line %d doesn't have a value.\n",
         line_count);
      return (is_error = true);
   }
   if (strlen(field_value) > ENVFILE_VALUE_MAX) {
      fprintf(stderr,
         "The value of the field " SRC_PATH " at line %d is too long (> %d).\n",
         line_count, ENVFILE_VALUE_MAX);
      return (is_error = true);
   }

   strncpy(env->src_path, field_value, strlen(field_value) + 1);
   (*applied_field_count)++;

   return (is_error = false);
}

static int handle_dest_path_field(
   struct env_data *env,
   char *field_value,
   int line_count,
   int *applied_field_count)
{
   bool is_error;

   if (field_value == NULL) {
      fprintf(stderr,
         "The field " DEST_PATH " at line %d doesn't have a value.\n",
         line_count);
      return (is_error = true);
   }
   if (strlen(field_value) > ENVFILE_VALUE_MAX) {
      fprintf(stderr,
         "The value of the field " DEST_PATH " at line %d is too long (> %d).\n",
         line_count, ENVFILE_VALUE_MAX);
      return (is_error = true);
   }
   
   strncpy(env->dest_path, field_value, strlen(field_value) + 1);
   (*applied_field_count)++;

   return (is_error = false);
}

static int handle_unknown_field(char *field_name, int line_count) {
   bool is_error;

   fprintf(stderr, "The field %s at line %d is unknown.\n",
      field_name, line_count);

   return (is_error = true);
}