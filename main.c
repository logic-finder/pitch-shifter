#include "wave_file.h"
#include "execution_options.h"
#include "command_line.h"
#include "processing.h"
#include "miscellaneous.h"

int main(int argc, char **argv) {
   FILE *src, *dest;
   struct wav_info info;
   struct execution_options *options;
   uint32_t sample_number;
   bool is_le = get_endianness();

   options = realize_execution_options();
   inspect_execution_options(argc, argv, options);
   open_wav(options, &src, &dest);
   observe_wav(src, &info, is_le, options->verbose);
   if (options->verbose)
      show_wav_info(options->src_name, &info);
   assess_wav_info(&info);
   sample_number = process_audio_data(
      src, dest, &info, options, is_le);
   write_wav_header(dest, &info, sample_number, is_le);
   close_wav(src, dest);
   options->unrealize(options->self);

   return 0;
}