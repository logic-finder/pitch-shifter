#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "wave_file.h"
#include "execution_options.h"

/*
 * process_audio_data: This function is the main part of this program.
 * It reads and processes audio data from the input wav file.
 * Also, it writes the processed results to the output wav file.
 */
uint32_t process_audio_data(
   FILE *src,
   FILE *dest,
   struct wav_info *info,
   struct execution_options *options,
   bool is_le
);

#endif