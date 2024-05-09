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
#define READLINE_READ_ERROR   -1
#define READLINE_EOF          -2
#define READLINE_EMPTY_LINE   -3
#define READLINE_LONG_LINE    -4
#define WRONG_ENVFILE   1

static int read_line(char * restrict, int, FILE * restrict);
static int handle_src_path_field(struct env_data *, char *, int, int *);
static int handle_dest_path_field(struct env_data *, char *, int, int *);
static int handle_unknown_field(char *, int);

void read_env(struct env_data *env, struct execution_options *options) {
   FILE *envfile;
   int result, error_flag;
   int line_count, applied_field_count;
   char *field_name, *field_value;
   char line[ENVFILE_LINE_MAX + 1];

   envfile = fopen(".env", "r");
   if (envfile == NULL) {
      if (options->verbose)
         printf(".env file not found; read skipped.\n");
      return;
   }

   error_flag = 0;
   line_count = 0;
   applied_field_count = 0;
   while (++line_count) {
      result = read_line(line, ENVFILE_LINE_MAX + 1, envfile);
      if (result == READLINE_READ_ERROR)
         raise_err(
            "%s: An error occured while reading from the .env file stream.", __func__);
      if (result == READLINE_EOF)
         break;
      if (result == READLINE_EMPTY_LINE)
         continue;

      field_name = strtok(line, " \t\0");   /* I think this call will always succeed. */
      if (field_name[0] == COMMENT)
         continue;
      if (result == READLINE_LONG_LINE) {
         error_flag |= WRONG_ENVFILE;
         fprintf(stderr, "Line %d is too long to process.\n", line_count);
         continue;
      }
      if (strlen(field_name) > ENVFILE_NAME_MAX) {
         error_flag |= WRONG_ENVFILE;
         fprintf(stderr,
            "The field name at line %d is too long (> %d).\n",
            line_count, ENVFILE_NAME_MAX);
         continue;
      }
      field_value = strtok(NULL, " \t\0");
      if (strncmp(field_name, SRC_PATH, strlen(SRC_PATH)) == 0)
         error_flag
            |= handle_src_path_field(env, field_value, line_count, &applied_field_count);
      else if (strncmp(field_name, DEST_PATH, strlen(DEST_PATH)) == 0)
         error_flag
            |= handle_dest_path_field(env, field_value, line_count, &applied_field_count);
      else
         error_flag |= handle_unknown_field(field_name, line_count);
   }
   if (error_flag & WRONG_ENVFILE)
      raise_err("%s: Unsuccessful read of .env file.", __func__);

   result = fclose(envfile);
   if (result == EOF)
      raise_err("%s: Failed to close the .env file stream.", __func__);
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
      raise_err("%s: The second argument = %d < 2.", __func__, n);
   if (n > ENVFILE_LINE_MAX + 1)
      raise_err(
         "%s: The second argument = %d > %d.",
         __func__, n, ENVFILE_LINE_MAX + 1);
   
   /* Skip (if any) white space character(s) at the front of the line. */
   while (isspace(ch = getc(stream)) && ch != '\n');
   if (ferror(stream))
      return READLINE_READ_ERROR;
   if (feof(stream))
      return READLINE_EOF;
   if (ch == '\n')
      return READLINE_EMPTY_LINE;
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
      return READLINE_READ_ERROR;

   if (i > n - 1)
      s[n - 1] = '\0';
   else
      s[i] = '\0';

   return i <= n - 1 ? 0 : READLINE_LONG_LINE;
}

static int handle_src_path_field(
   struct env_data *env,
   char *field_value,
   int line_count,
   int *applied_field_count)
{
   if (field_value == NULL) {
      fprintf(stderr,
         "The field " SRC_PATH " at line %d doesn't have a value.\n",
         line_count);
      return WRONG_ENVFILE;
   }
   if (strlen(field_value) > ENVFILE_VALUE_MAX) {
      fprintf(stderr,
         "The value of the field " SRC_PATH " at line %d is too long (> %d).\n",
         line_count, ENVFILE_VALUE_MAX);
      return WRONG_ENVFILE;
   }

   strncpy(env->src_path, field_value, strlen(field_value) + 1);
   (*applied_field_count)++;

   return !WRONG_ENVFILE;
}

static int handle_dest_path_field(
   struct env_data *env,
   char *field_value,
   int line_count,
   int *applied_field_count)
{
   if (field_value == NULL) {
      fprintf(stderr,
         "The field " DEST_PATH " at line %d doesn't have a value.\n",
         line_count);
      return WRONG_ENVFILE;
   }
   if (strlen(field_value) > ENVFILE_VALUE_MAX) {
      fprintf(stderr,
         "The value of the field " DEST_PATH " at line %d is too long (> %d).\n",
         line_count, ENVFILE_VALUE_MAX);
      return WRONG_ENVFILE;
   }
   
   strncpy(env->dest_path, field_value, strlen(field_value) + 1);
   (*applied_field_count)++;

   return !WRONG_ENVFILE;
}

static int handle_unknown_field(char *field_name, int line_count) {
   fprintf(stderr, "The field %s at line %d is unknown.\n",
      field_name, line_count);

   return WRONG_ENVFILE;
}