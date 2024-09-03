#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "wave_file.h"
#include "miscellaneous.h"

#define RIFF 0x52494646    
#define WAVE 0x57415645
#define FMT  0x666D7420
#define DATA 0x64617461
#define LIST 0x4C495354

static void handle_fmt_subchunk(
   FILE *, struct wav_info *, bool, uint32_t);
static void handle_data_subchunk(struct wav_info *, uint32_t);
static void handle_list_chunk(FILE *, uint32_t, bool);

void observe_wav(
   FILE *src,
   struct wav_info *info,
   bool is_le,
   bool is_verbose
) {
   bool is_fmt_subchunk_found = false;
   bool is_data_subchunk_found = false;
   bool le = is_le;
   bool be = !le;
   int result;
   uint32_t chunk_id, chunk_size;

   result = fread(&info->chunk_id, 4, 1, src);
   if (result != 1) raise_err("%s: Failed to read RIFF.", __func__);
   if (le) endrev32(&info->chunk_id);
   
   result = fread(&info->chunk_size, 4, 1, src);
   if (result != 1) raise_err("%s: Failed to read the size of the RIFF chunk.", __func__);
   if (be) endrev32(&info->chunk_size);
   
   result = fread(&info->format, 4, 1, src);
   if (result != 1) raise_err("%s: Failed to read WAVE.", __func__);
   if (le) endrev32(&info->format);

   /*
    * Skip (possible) optional chunks and try to find
    * a fmt and data subchunk.
    */
   while (!is_data_subchunk_found) {
      result = fread(&chunk_id, 4, 1, src);
      if (result != 1) raise_err("%s: Failed to read ChunkID.", __func__);
      if (le) endrev32(&chunk_id);

      result = fread(&chunk_size, 4, 1, src);
      if (result != 1) raise_err("%s: Failed to read ChunkSize.", __func__);
      if (be) endrev32(&chunk_size);

      if (chunk_size > LONG_MAX)
            raise_err("%s: A file too big: I wouldn't like to process this file.", __func__);

      switch (chunk_id) {
         case FMT: {
            handle_fmt_subchunk(src, info, is_le, chunk_size);
            is_fmt_subchunk_found = true;
         }
         break;
         case DATA: {
            handle_data_subchunk(info, chunk_size);
            is_data_subchunk_found = true;
         }
         break;
         case LIST:
            handle_list_chunk(src, chunk_size, is_verbose);
         break;
         default: {
            result = fseek(src, chunk_size, SEEK_CUR);
            if (result != 0) raise_err("%s: Failed to seek the file position.", __func__);
         }
      }
   }

   if (!is_fmt_subchunk_found)
      raise_err("%s: An invalidly formatted .wav file.", __func__);
}

static void handle_fmt_subchunk(
   FILE *src,
   struct wav_info *info,
   bool is_le,
   uint32_t chunk_size
) {
   int result;
   bool le = is_le;
   bool be = !le;

   info->subchunk_1_id = FMT;
   info->subchunk_1_size = chunk_size;

   result = fread(&info->audio_format, 2, 1, src);
   if (result != 1) raise_err("%s: Failed to read AudioFormat.", __func__);
   if (be) endrev16(&info->audio_format);

   result = fread(&info->num_channels, 2, 1, src);
   if (result != 1) raise_err("%s: Failed to read NumChannels.", __func__);
   if (be) endrev16(&info->num_channels);

   result = fread(&info->sample_rate, 4, 1, src);
   if (result != 1) raise_err("%s: Failed to read SampleRate.", __func__);
   if (be) endrev32(&info->sample_rate);

   result = fread(&info->byte_rate, 4, 1, src);
   if (result != 1) raise_err("%s: Failed to read ByteRate.", __func__);
   if (be) endrev32(&info->byte_rate);

   result = fread(&info->block_align, 2, 1, src);
   if (result != 1) raise_err("%s: Failed to read BlockAlign.", __func__);
   if (be) endrev16(&info->block_align);

   result = fread(&info->bits_per_sample, 2, 1, src);
   if (result != 1) raise_err("%s: Failed to read BitsPerSample.", __func__);
   if (be) endrev16(&info->bits_per_sample);

   if (chunk_size == 18 || chunk_size == 40) {
      result = fseek(src, chunk_size - 16, SEEK_CUR);
      if (result != 0) raise_err("%s: Failed to seek the file position.", __func__);
   }
}

static void handle_data_subchunk(
   struct wav_info *info,
   uint32_t chunk_size
) {
   info->subchunk_2_id = DATA;
   info->subchunk_2_size = chunk_size;
}

static void handle_list_chunk(
   FILE *src,
   uint32_t chunk_size,
   bool is_verbose
) {
   int result;

   if (is_verbose)
      printf("A LIST chunk has been found but ignored.\n");

   result = fseek(src, chunk_size, SEEK_CUR);
   if (result != 0) raise_err("%s: Failed to seek the file position.", __func__);
}

/* Note: This function does this task: 0x6162 --> "ab" */
static void hex2fourCC(uint32_t hex, char *str) {
   int i;

   for (i = 3; i >= 0; i--)
      str[3 - i] = (hex >> (i * 8)) & 0x000000FF;
}

void show_wav_info(char *file_name, struct wav_info *info) {
   char fourCC[5] = {0};

   printf("Successful read of %s; its metadata:\n", file_name);
   hex2fourCC(info->chunk_id, fourCC);
   printf("  ChunkID = %s\n", fourCC);
   printf("  ChunkSize = %d (bytes)\n", info->chunk_size);
   hex2fourCC(info->format, fourCC);
   printf("  Format = %s\n", fourCC);
   hex2fourCC(info->subchunk_1_id, fourCC);
   printf("  SubChunk1ID = %s\n", fourCC);
   printf("  SubChunk1Size = %d (bytes)\n", info->subchunk_1_size);
   printf("  AudioFormat = %d %s",
          info->audio_format, info->audio_format == 1 ? "(PCM)\n" : "\n");
   printf("  NumChannels = %d\n", info->num_channels);
   printf("  SampleRate = %d\n", info->sample_rate);
   printf("  ByteRate = %d\n", info->byte_rate);
   printf("  BlockAlign = %d (bytes)\n", info->block_align);
   printf("  BitsPerSample = %d\n", info->bits_per_sample);
   hex2fourCC(info->subchunk_2_id, fourCC);
   printf("  SubChunk2ID = %s\n", fourCC);
   printf("  SubChunk2Size = %d\n", info->subchunk_2_size);
}

void assess_wav_info(struct wav_info *info) {
   if (info->chunk_id != RIFF)
      raise_err("%s: Need ChunkID = RIFF.", __func__);

   if (info->format != WAVE)
      raise_err("%s: Need Format = WAVE.", __func__);

   if (info->subchunk_1_id != FMT)
      raise_err("%s: Need SubChunk1ID = FMT_", __func__);

   if (info->subchunk_1_size != 16)
      raise_err("%s: Need SubChunk1Size = 16", __func__);

   if (info->audio_format != 1)
      raise_err("%s: Need AudioFormat = 1 (PCM).", __func__);

   if (info->num_channels > 2)
      raise_err("%s: Need NumChannels = 1 or 2.", __func__);

   if (info->sample_rate != 44100)
      raise_err("%s: Need SampleRate = 44100.", __func__);

   if (info->byte_rate != 88200 && info->byte_rate != 176400)
      raise_err("%s: Need ByteRate = 88200 or 176400.", __func__);

   if (info->block_align != 2 && info->block_align != 4)
      raise_err("%s: Need BlockAlign = 2 or 4.", __func__);

   if (info->bits_per_sample != 16)
      raise_err("%s: Need BitsPerSample = 16.", __func__);
}

void write_wav_header(
   FILE *dest,
   struct wav_info *info,
   uint32_t sample_number,
   bool is_le,
   char *dest_path
) {
   int result;
   bool le = is_le;
   bool be = !le;

   uint32_t chunk_size, subchunk_2_size;

   subchunk_2_size = sample_number
                     * info->num_channels
                     * (info->bits_per_sample / 8);
   chunk_size = 36 + subchunk_2_size;

   rewind(dest);

   if (le) endrev32(&info->chunk_id);
   result = fwrite(&info->chunk_id, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write ChunkID.", __func__);

   if (be) endrev32(&chunk_size);
   result = fwrite(&chunk_size, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write ChunkSize.", __func__);

   if (le) endrev32(&info->format);
   result = fwrite(&info->format, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write Format.", __func__);

   if (le) endrev32(&info->subchunk_1_id);
   result = fwrite(&info->subchunk_1_id, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write Subchunk1ID.", __func__);

   if (be) endrev32(&info->subchunk_1_size);
   result = fwrite(&info->subchunk_1_size, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write Subchunk1Size.", __func__);

   if (be) endrev16(&info->audio_format);
   result = fwrite(&info->audio_format, 2, 1, dest);
   if (result != 1) raise_err("%s: Failed to write AudioFormat.", __func__);

   if (be) endrev16(&info->num_channels);
   result = fwrite(&info->num_channels, 2, 1, dest);
   if (result != 1) raise_err("%s: Failed to write NumChannels.", __func__);

   if (be) endrev32(&info->sample_rate);
   result = fwrite(&info->sample_rate, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write SampleRate.", __func__);

   if (be) endrev32(&info->byte_rate);
   result = fwrite(&info->byte_rate, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write ByteRate.", __func__);

   if (be) endrev16(&info->block_align);
   result = fwrite(&info->block_align, 2, 1, dest);
   if (result != 1) raise_err("%s: Failed to write BlockAlign.", __func__);

   if (be) endrev16(&info->bits_per_sample);
   result = fwrite(&info->bits_per_sample, 2, 1, dest);
   if (result != 1) raise_err("%s: Failed to write BitsPerSample.", __func__);

   if (le) endrev32(&info->subchunk_2_id);
   result = fwrite(&info->subchunk_2_id, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write Subchunk2ID.", __func__);

   if (be) endrev32(&subchunk_2_size);
   result = fwrite(&subchunk_2_size, 4, 1, dest);
   if (result != 1) raise_err("%s: Failed to write Subchunk2Size.", __func__);

   printf("\a\nDone: %s, %" PRId32 " (bytes)\n",
      dest_path, 44 + subchunk_2_size);
}

char *open_wav(
   struct execution_options *options,
   struct env_data *env,
   FILE **src,
   FILE **dest
) {
   char *sp = env->src_path, *dp = env->dest_path;
   char *sn = options->src_name, *dn = options->dest_name;
   char *src_path_full, *dest_path_full;

   src_path_full = malloc(strlen(sp) + strlen(sn) + 1);
   if (src_path_full == NULL)
      raise_err("%s: Failed to allocate memory dynamically.", __func__);
   if (options->suppress_src_path) {
      strncpy(src_path_full, CURRENT_DIR, 3);
      strncat(src_path_full, sn, strlen(sn));
   }
   else {
      strncpy(src_path_full, sp, strlen(sp) + 1);
      strncat(src_path_full, sn, strlen(sn));   /* Note: strncat() always puts
                                                   \0 at the end. */
   }
   *src = fopen(src_path_full, "rb");
   if (*src == NULL)
      raise_err("%s: Failed to open the requested file from %s.",
         __func__, src_path_full);

   dest_path_full = malloc(strlen(dp) + strlen(dn) + 1);
   if (dest_path_full == NULL)
      raise_err("%s: Failed to allocate memory dynamically.", __func__);
   if (options->suppress_dest_path) {
      strncpy(dest_path_full, CURRENT_DIR, 3);
      strncat(dest_path_full, dn, strlen(dn));
   }
   else {  
      strncpy(dest_path_full, dp, strlen(dp) + 1);
      strncat(dest_path_full, dn, strlen(dn));
   }
   *dest = fopen(dest_path_full, "wb");
   if (*dest == NULL)
      raise_err("%s: Failed to open the requested file from %s.",
         __func__, dest_path_full);

   return dest_path_full;
}

void close_wav(FILE *src, FILE *dest) {
   int result;

   result = fclose(src);
   if (result == EOF)
      raise_err("%s: Failed to close the source wav file.", __func__);
   result = fclose(dest);
   if (result == EOF)
      raise_err("%s: Failed to close the destination wav file.", __func__);
}