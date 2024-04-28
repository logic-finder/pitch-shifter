#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include "env_data.h"
#include "miscellaneous.h"

#define LEN_EXECUTION_OPTIONS 2  /* except self */

static void unrealize(struct env_data *);

struct env_data *realize_env_data(void) {
   struct env_data *objptr;
   char *src_path, *dest_path;
   char *current_dir = CURRENT_DIR;

   objptr = malloc(sizeof(struct env_data));
   if (objptr == NULL)
      raise_err("%s: Failed to create a new struct env_data.", __func__);
   src_path = malloc(ENVFILE_VALUE_MAX + 1);
   if (src_path == NULL)
      raise_err("%s: Failed to allocate memory dynamically.", __func__);
   dest_path = malloc(ENVFILE_VALUE_MAX + 1);
   if (dest_path == NULL)
      raise_err("%s: Failed to allocate memory dynamically.", __func__);
   objptr->self = objptr;
   objptr->unrealize = unrealize;
   objptr->src_path = src_path;
   strncpy(src_path, current_dir, 3);  /* '.', '\', and '\0' */
   objptr->dest_path = dest_path;
   strncpy(dest_path, current_dir, 3);

   return objptr;
}

static void *iterate(struct env_data *objptr, int idx) {
   switch (idx) {
      case 0: return objptr->src_path;
      case 1: return objptr->dest_path;
      default:
         raise_err("%s: Failed to access the field of struct env_data.", __func__);
   }
}

static void unrealize(struct env_data *objptr) {
   int i;

   for (i = 0; i < LEN_EXECUTION_OPTIONS; i++)
      free(iterate(objptr, i));
   free(objptr->self);
}