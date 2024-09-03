#include "wave_file.h"
#include "miscellaneous.h"
#include "execution_options.h"
#include "env_data.h"
#include "command_line.h"
#include "envfile_reader.h"
#include "processing.h"

int main(int argc, char **argv) {
   FILE *src, *dest;
   struct wav_info info;
   struct execution_options *options;
   struct env_data *env;
   uint32_t sample_number;
   char *dest_path;
   bool is_le = get_endianness();

   options = realize_execution_options();
   env = realize_env_data();
   inspect_execution_options(argc, argv, options);
   read_env(env, options);
   dest_path = open_wav(options, env, &src, &dest);
   observe_wav(src, &info, is_le, options->verbose);
   if (options->verbose)
      show_wav_info(options->src_name, &info);
   assess_wav_info(&info);
   sample_number = process_audio_data(
      src, dest, &info, options, is_le);
   write_wav_header(
      dest, &info, sample_number, is_le, dest_path);
   close_wav(src, dest);
   options->unrealize(options->self);
   env->unrealize(env->self);

   return 0;
}