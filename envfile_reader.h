#ifndef ENVFILE_READER_H
#define ENVFILE_READER_H

#include "env_data.h"
#include "execution_options.h"

/*
 * read_env: This function reads the .env file,
 * get data from it, and fill the struct env_data
 * with the data.
 */
void read_env(struct env_data *env, struct execution_options *options);

#endif