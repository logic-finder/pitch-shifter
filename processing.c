#include <stdlib.h>
#include "processing.h"
#include "miscellaneous.h"

static uint32_t shift_pitch(
   FILE *, FILE *,
   struct wav_info *, struct execution_options *,
   bool);
static uint32_t stretch_time(
   FILE *, FILE *,
   struct wav_info *, struct execution_options *,
   bool);

uint32_t process_audio_data(
   FILE *src,
   FILE *dest,
   struct wav_info *info,
   struct execution_options *options,
   bool is_le
) {
   uint32_t sample_number;

   if (options->mode == 1)
      sample_number
         = shift_pitch(src, dest, info, options, is_le);
   else if (options->mode == 2)
      sample_number
         = stretch_time(src, dest, info, options, is_le);

   return sample_number;
}

/*
 * Note: the function 'window' is for removing 'click' sounds
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

static uint32_t shift_pitch(
   FILE *src,
   FILE *dest,
   struct wav_info *info,
   struct execution_options *options,
   bool is_le
) {
   int grain_size = options->size;
   double pitch_factor = options->factor;
   uint16_t num_channels = info->num_channels;
   uint32_t total_sample = info->subchunk_2_size
                           / (num_channels * (info->bits_per_sample / 8));
   uint32_t total_unit = total_sample / grain_size;
   int src_buf_len = grain_size * num_channels;
   int dest_buf_len = src_buf_len;
   int total_uint_digit = count_digit(total_unit);

   int result;
   int i;
   double j;
   int channel;
   uint32_t unit;
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

      print_progress_bar(unit + 1, total_unit, total_uint_digit);
   }
   if (ferror(src)) raise_err("Failed to read audio data.");

   free(src_buf);
   free(dest_buf);

   /* the number of total samples. */
   return unit * grain_size;
}

static uint32_t stretch_time(
   FILE *src,
   FILE *dest,
   struct wav_info *info,
   struct execution_options *options,
   bool is_le
) {
   int grain_size = options->size;
   double speed_factor = options->factor;
   int part = grain_size / speed_factor;
   uint16_t num_channels = info->num_channels;
   uint32_t total_sample = info->subchunk_2_size
                           / (num_channels * (info->bits_per_sample / 8));
   uint32_t total_unit = total_sample / grain_size;
   int src_buf_len = grain_size * num_channels;
   int dest_buf_len = part * num_channels;
   int total_uint_digit = count_digit(total_unit);

   int result;
   int i, j;
   int channel;
   uint32_t unit;
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

      print_progress_bar(unit + 1, total_unit, total_uint_digit);
   }
   if (ferror(src)) raise_err("Failed to read audio data.");

   free(src_buf);
   free(dest_buf);

   /* the number of total samples. */
   return unit * part;
}