#include <math.h>
#include <limits.h>
#include <string.h>
#include "processing.h"

#define RIFF 0x52494646    
#define WAVE 0x57415645
#define FMT  0x666D7420
#define DATA 0x64617461
#define LIST 0x4C495354

/*
 * Note: These two functions reverse the byte
 * order, namely endianness.
 */
inline static void endrev16(uint16_t *p) {
   *p = *p << 8 | *p >> 8;
}

inline static void endrev32(uint32_t *p) {
   *p = *p << 24 | (*p & 0x0000FF00) << 8 |
        (*p & 0x00FF0000) >> 8 | (*p & 0xFF000000) >> 24;
}

static void _handle_fmt_subchunk(FILE *src,
                                 struct wav_info *info,
                                 bool is_le,
                                 uint32_t chunk_size) {
   int result;
   bool le = is_le;
   bool be = !le;

   info->subchunk_1_id = FMT;
   info->subchunk_1_size = chunk_size;

   result = fread(&info->audio_format, 2, 1, src);
   if (result != 1) raise_err("Failed to read AudioFormat.");
   if (be) endrev16(&info->audio_format);

   result = fread(&info->num_channels, 2, 1, src);
   if (result != 1) raise_err("Failed to read NumChannels.");
   if (be) endrev16(&info->num_channels);

   result = fread(&info->sample_rate, 4, 1, src);
   if (result != 1) raise_err("Failed to read SampleRate.");
   if (be) endrev32(&info->sample_rate);

   result = fread(&info->byte_rate, 4, 1, src);
   if (result != 1) raise_err("Failed to read ByteRate.");
   if (be) endrev32(&info->byte_rate);

   result = fread(&info->block_align, 2, 1, src);
   if (result != 1) raise_err("Failed to read BlockAlign.");
   if (be) endrev16(&info->block_align);

   result = fread(&info->bits_per_sample, 2, 1, src);
   if (result != 1) raise_err("Failed to read BitsPerSample.");
   if (be) endrev16(&info->bits_per_sample);

   if (chunk_size == 18 || chunk_size == 40) {
      result = fseek(src, chunk_size - 16, SEEK_CUR);
      if (result != 0) raise_err("Failed to seek the file position.");
   }
}

static void _handle_data_subchunk(struct wav_info *info,
                                  uint32_t chunk_size) {
   info->subchunk_2_id = DATA;
   info->subchunk_2_size = chunk_size;
}

static void _handle_list_chunk(FILE *src,
                               uint32_t chunk_size,
                               bool is_verbose) {
   int result;

   if (is_verbose)
      printf("A LIST chunk has been found but ignored.\n");

   result = fseek(src, chunk_size, SEEK_CUR);
   if (result != 0) raise_err("Failed to seek the file position.");
}

void observe_wav(FILE *src,
                 struct wav_info *info,
                 bool is_le,
                 bool is_verbose) {
   bool is_fmt_subchunk_found = false;
   bool is_data_subchunk_found = false;
   bool le = is_le;
   bool be = !le;
   int result;
   uint32_t chunk_id, chunk_size;

   result = fread(&info->chunk_id, 4, 1, src);
   if (result != 1) raise_err("Failed to read RIFF.");
   if (le) endrev32(&info->chunk_id);
   
   result = fread(&info->chunk_size, 4, 1, src);
   if (result != 1) raise_err("Failed to read the size of the RIFF chunk.");
   if (be) endrev32(&info->chunk_size);
   
   result = fread(&info->format, 4, 1, src);
   if (result != 1) raise_err("Failed to read WAVE.");
   if (le) endrev32(&info->format);

   /*
    * Skip (possible) optional chunks and try to find
    * a fmt and data subchunk.
    */
   while (!is_data_subchunk_found) {
      result = fread(&chunk_id, 4, 1, src);
      if (result != 1) raise_err("Failed to read ChunkID.");
      if (le) endrev32(&chunk_id);

      result = fread(&chunk_size, 4, 1, src);
      if (result != 1) raise_err("Failed to read ChunkSize.");
      if (be) endrev32(&chunk_size);

      if (chunk_size > LONG_MAX)
            raise_err("A file too big: I wouldn't like to process this file.");

      switch (chunk_id) {
         case FMT: {
            _handle_fmt_subchunk(src, info, is_le, chunk_size);
            is_fmt_subchunk_found = true;
         }
         break;
         case DATA: {
            _handle_data_subchunk(info, chunk_size);
            is_data_subchunk_found = true;
         }
         break;
         case LIST:
            _handle_list_chunk(src, chunk_size, is_verbose);
         break;
         default: {
            result = fseek(src, chunk_size, SEEK_CUR);
            if (result != 0) raise_err("Failed to seek the file position.");
         }
      }
   }

   if (!is_fmt_subchunk_found)
      raise_err("An invalidly formatted .wav file.");
}

/* Note: This function does this task: 0x6162 --> "ab" */
static void hex2fourCC(uint32_t hex, char *str) {
   int i;

   for (i = 3; i >= 0; i--)
      str[3 - i] = (hex >> (i * 8)) & 0x000000FF;
}

void show_wav_info(char *file_name, struct wav_info *info) {
   char fourCC[5] = {0};

   printf("Successful read of %s.\n", file_name);
   printf("The metadata of this file:\n");
   hex2fourCC(info->chunk_id, fourCC);
   printf("  ChunkID = %s\n", fourCC);
   printf("  ChunkSize = %d (byte)\n", info->chunk_size);
   hex2fourCC(info->format, fourCC);
   printf("  Format = %s\n", fourCC);
   hex2fourCC(info->subchunk_1_id, fourCC);
   printf("  SubChunk1ID = %s\n", fourCC);
   printf("  SubChunk1Size = %d (byte)\n", info->subchunk_1_size);
   printf("  AudioFormat = %d %s",
          info->audio_format, info->audio_format == 1 ? "(PCM)\n" : "\n");
   printf("  NumChannels = %d\n", info->num_channels);
   printf("  SampleRate = %d\n", info->sample_rate);
   printf("  ByteRate = %d\n", info->byte_rate);
   printf("  BlockAlign = %d (byte)\n", info->block_align);
   printf("  BitsPerSample = %d\n", info->bits_per_sample);
   hex2fourCC(info->subchunk_2_id, fourCC);
   printf("  SubChunk2ID = %s\n", fourCC);
   printf("  SubChunk2Size = %d\n", info->subchunk_2_size);
}

void assess_wav_info(struct wav_info *info) {
   if (info->chunk_id != RIFF)
      raise_err("Need ChunkID = RIFF.");

   if (info->format != WAVE)
      raise_err("Need Format = WAVE.");

   if (info->subchunk_1_id != FMT)
      raise_err("Need SubChunk1ID = FMT_");

   if (info->subchunk_1_size != 16)
      raise_err("Need SubChunk1Size = 16");

   if (info->audio_format != 1)
      raise_err("Need AudioFormat = 1 (PCM).");

   if (info->num_channels > 2)
      raise_err("Need NumChannels = 1 or 2.");

   if (info->sample_rate != 44100)
      raise_err("Need SampleRate = 44100.");

   if (info->byte_rate != 88200 && info->byte_rate != 176400)
      raise_err("Need ByteRate = 88200 or 176400.");

   if (info->block_align != 2 && info->block_align != 4)
      raise_err("Need BlockAlign = 2 or 4.");

   if (info->bits_per_sample != 16)
      raise_err("Need BitsPerSample = 16.");
}

/*
 * Note: This function is for removing 'click' sounds
 * through multiplying the return value of this function
 * by the audio data.
 */
inline static double window(int pos, int len) {
   if (pos < 10)
      return 0.1 * pos;
   else if (len - 10 <= pos)
      return 0.9 - 0.1 * (pos - (len - 10));
   else
      return 1;
}

static uint32_t _shift_pitch(FILE *src,
                             FILE *dest,
                             struct wav_info *info,
                             struct execution_options *options,
                             bool is_le) {
   int grain_size = options->size;
   double pitch_factor = options->factor;
   uint16_t num_channels = info->num_channels;
   uint32_t total_sample = info->subchunk_2_size
                           / (num_channels * (info->bits_per_sample / 8));
   uint32_t total_unit = total_sample / grain_size;
   int src_buf_len = grain_size * num_channels;
   int dest_buf_len = src_buf_len;

   int result;
   int unit, channel;
   int i;
   double j;
   int16_t *src_buf, *dest_buf;

   result = fseek(dest, 44L, SEEK_SET);
   if (result != 0) raise_err("Failed to seek the file position.");
   src_buf = malloc(src_buf_len * 2);
   if (src_buf == NULL) raise_err("Failed to allocate memory dynamically.");
   dest_buf = malloc(dest_buf_len * 2);
   if (dest_buf == NULL) raise_err("Failed to allocate memory dynamically.");

   for (unit = 0; unit < total_unit; unit++) {
      result = fread(src_buf, 2, src_buf_len, src);
      if (result != src_buf_len) break;

      for (channel = 0; channel < num_channels; channel++)
         for (i = 0, j = 0; i < grain_size; i++, j += pitch_factor) {
            if (j >= grain_size)
               j = 0;
            dest_buf[num_channels * i + channel]
               = src_buf[num_channels * (int) j + channel]
                 * window(i, grain_size);
         }
      if (!is_le)
            for (i = 0; i < dest_buf_len; i++)
               endrev16(&dest_buf[i]);

      result = fwrite(dest_buf, 2, dest_buf_len, dest);
      if (result != dest_buf_len) raise_err("Failed to write data.");
   }
   if (ferror(src)) raise_err("Failed to read audio data.");

   free(src_buf);
   free(dest_buf);

   /* the number of total samples. */
   return unit * grain_size;
}

static uint32_t _stretch_time(FILE *src,
                              FILE *dest,
                              struct wav_info *info,
                              struct execution_options *options,
                              bool is_le) {
   int grain_size = options->size;
   double speed_factor = options->factor;
   int part = grain_size / speed_factor;
   uint16_t num_channels = info->num_channels;
   uint32_t total_sample = info->subchunk_2_size
                           / (num_channels * (info->bits_per_sample / 8));
   uint32_t total_unit = total_sample / grain_size;
   int src_buf_len = grain_size * num_channels;
   int dest_buf_len = part * num_channels;

   int result;
   int unit, channel;
   int i, j;
   int16_t *src_buf, *dest_buf;

   result = fseek(dest, 44L, SEEK_SET);
   if (result != 0) raise_err("Failed to seek the file position.");
   src_buf = malloc(src_buf_len * 2);
   if (src_buf == NULL) raise_err("Failed to allocate memory dynamically.");
   dest_buf = malloc(dest_buf_len * 2);
   if (dest_buf == NULL) raise_err("Failed to allocate memory dynamically.");

   for (unit = 0; unit < total_unit; unit++) {
      result = fread(src_buf, 2, src_buf_len, src);
      if (result !=  src_buf_len) break;

      for (channel = 0; channel < num_channels; channel++)
         for (i = 0, j = 0; i < part; i++, j++) {
            if (j == grain_size)
               j = 0;
            dest_buf[num_channels * i + channel]
               = src_buf[num_channels * j + channel]
                 * window(i, part);
         }
      if (!is_le)
         for (i = 0; i < dest_buf_len; i++)
            endrev16(&dest_buf[i]);

      result = fwrite(dest_buf, 2, dest_buf_len, dest);
      if (result != dest_buf_len) raise_err("Failed to write data.");
   }
   if (ferror(src)) raise_err("Failed to read audio data.");

   free(src_buf);
   free(dest_buf);

   /* the number of total samples. */
   return unit * part;
}

uint32_t process_wav(FILE *src,
                     FILE *dest,
                     struct wav_info *info,
                     struct execution_options *options,
                     bool is_le) {
   uint32_t sample_number;

   if (options->mode == 1)
      sample_number
         = _shift_pitch(src, dest, info, options, is_le);
   else if (options->mode == 2)
      sample_number
         = _stretch_time(src, dest, info, options, is_le);

   return sample_number;
}

void write_wav_header(FILE *dest,
                      struct wav_info *info,
                      uint32_t sample_number,
                      bool is_le) {
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
   if (result != 1) raise_err("Failed to write ChunkID.");

   if (be) endrev32(&chunk_size);
   result = fwrite(&chunk_size, 4, 1, dest);
   if (result != 1) raise_err("Failed to write ChunkSize.");

   if (le) endrev32(&info->format);
   result = fwrite(&info->format, 4, 1, dest);
   if (result != 1) raise_err("Failed to write Format.");

   if (le) endrev32(&info->subchunk_1_id);
   result = fwrite(&info->subchunk_1_id, 4, 1, dest);
   if (result != 1) raise_err("Failed to write Subchunk1ID.");

   if (be) endrev32(&info->subchunk_1_size);
   result = fwrite(&info->subchunk_1_size, 4, 1, dest);
   if (result != 1) raise_err("Failed to write Subchunk1Size.");

   if (be) endrev16(&info->audio_format);
   result = fwrite(&info->audio_format, 2, 1, dest);
   if (result != 1) raise_err("Failed to write AudioFormat.");

   if (be) endrev16(&info->num_channels);
   result = fwrite(&info->num_channels, 2, 1, dest);
   if (result != 1) raise_err("Failed to write NumChannels.");

   if (be) endrev32(&info->sample_rate);
   result = fwrite(&info->sample_rate, 4, 1, dest);
   if (result != 1) raise_err("Failed to write SampleRate.");

   if (be) endrev32(&info->byte_rate);
   result = fwrite(&info->byte_rate, 4, 1, dest);
   if (result != 1) raise_err("Failed to write ByteRate.");

   if (be) endrev16(&info->block_align);
   result = fwrite(&info->block_align, 2, 1, dest);
   if (result != 1) raise_err("Failed to write BlockAlign.");

   if (be) endrev16(&info->bits_per_sample);
   result = fwrite(&info->bits_per_sample, 2, 1, dest);
   if (result != 1) raise_err("Failed to write BitsPerSample.");

   if (le) endrev32(&info->subchunk_2_id);
   result = fwrite(&info->subchunk_2_id, 4, 1, dest);
   if (result != 1) raise_err("Failed to write Subchunk2ID.");

   if (be) endrev32(&subchunk_2_size);
   result = fwrite(&subchunk_2_size, 4, 1, dest);
   if (result != 1) raise_err("Failed to write Subchunk2Size.");

   printf("\aDone: %" PRId32 " (byte)\n", 44 + subchunk_2_size);
}
