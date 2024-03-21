#ifndef WAVE_FILE_H
#define WAVE_FILE_H

#include <stdio.h>
#include <inttypes.h>
#include "execution_options.h"

struct wav_info {
   uint32_t chunk_id;
   uint32_t chunk_size;
   uint32_t format;
   uint32_t subchunk_1_id;
   uint32_t subchunk_1_size;
   uint16_t audio_format;
   uint16_t num_channels;
   uint32_t sample_rate;
   uint32_t byte_rate;
   uint16_t block_align;
   uint16_t bits_per_sample;
   uint32_t subchunk_2_id;
   uint32_t subchunk_2_size;
};

/*
 * observe_wav: This function checks the metadata of the input
 * wav file. Also, it saves the acquired information to the
 * struct wav_info for later use.
 */
void observe_wav(FILE *, struct wav_info *, bool, bool);

/*
 * show_wav_info: This function displays the metadata of the
 * wav file to the stdout stream.
 */
void show_wav_info(char *, struct wav_info *);

/*
 * assess_wav_info: This function sees whether the input wav
 * file can be processed by this program.
 */
void assess_wav_info(struct wav_info *);

/*
 * write_wav_header: This function writes the metadata for the
 * output wav file.
 */
void write_wav_header(FILE *, struct wav_info *, uint32_t, bool);

/*
 * open_wav: This function opens two streams for
 * the input wav file and the output wave file
 * before processings are to take place.
 */
void open_wav(struct execution_options *, FILE **, FILE **);

/*
 * close_wav: This function closes two streams for
 * the input wav file and the output wave file
 * after all processings have been done.
 */
void close_wav(FILE *, FILE *);

#endif