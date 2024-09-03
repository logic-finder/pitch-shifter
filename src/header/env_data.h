#ifndef ENV_DATA_H
#define ENV_DATA_H

#define CURRENT_DIR "./"
#define ENVFILE_NAME_MAX   128
#define ENVFILE_VALUE_MAX  256
#define ENVFILE_LINE_MAX   (ENVFILE_NAME_MAX + ENVFILE_VALUE_MAX)

struct env_data {
   void (*unrealize)(struct env_data *);

   /* fields to be freed */
   char *src_path;
   char *dest_path;
   struct env_data *self;
};

/*
 * realize_env_data: This function creates
 * a new struct env_data.
 */
struct env_data *realize_env_data(void);

#endif