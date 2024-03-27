#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "execution_options.h"

/*
 * inspect_execution_options: This function checks the command line
 * arguments.
 */
void inspect_execution_options(
   int argc,
   char **argv,
   struct execution_options *options
);

#endif