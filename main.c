#include "processing.h"
#include "miscellaneous.h"

int main(int argc, char **argv) {
   FILE *src, *dest;
   struct wav_info info;
   struct execution_options *options;
   uint32_t sample_number;
   bool is_le = get_endianness();

   options = realize_execution_options();
   validate_execution(argc, argv, options);
   open_wav(options, &src, &dest);
   observe_wav(src, &info, is_le);
   if (options->verbose)
      show_wav_info(options->src_name, &info);
   assess_wav_info(&info);
   sample_number = process_wav(src, dest,
                               &info, options,
                               is_le);
   write_wav_header(dest,
                    &info, sample_number, is_le);
   close_wav(src, dest);
   options->cleanup_execution_options(options->self);
   
   return 0;
}