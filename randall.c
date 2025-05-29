/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.

   Copyright 2015, 2017, 2020 Paul Eggert

   This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


/* Main program, which outputs N bytes of random data.  */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#include "rand64-hw.h"
#include "rand64-sw.h"
#include "output.h"
#include "options.h"
#include "rand64-file.h"

int main (int argc, char **argv){
  Options options = parse_options(argc, argv);
  if (!options.valid){
    return 1;
  }
  // bool valid = false;
  // long long nbytes;
  // if (argc == 2){
  //   char *endptr;
  //   errno = 0;
  //   nbytes = strtoll(argv[1], &endptr, 10);
  //   if (errno) 
  //     perror(argv[1]);
  //   else
	//     valid = !*endptr && 0 <= nbytes;
  // }
  // if (!valid){
  //   fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
  //   return 1;
  // }

  if (options.nbytes == 0)
    return 0;

  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);
  
  switch (options.input_type){
    case INPUT_RDRAND:
      if (!rdrand_supported()) {
          fprintf(stderr, "Error: RDRAND not supported.\n");
          return 1;
      }
      initialize = hardware_rand64_init;
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
      break;

    case INPUT_MRAND48:
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
      break;

    case INPUT_FILE:
      file_rand64_init(options.input_file);
      initialize = NULL;
      rand64 = file_rand64;
      finalize = file_rand64_fini;
      break;
  }

  if (initialize) {
    initialize();
  }

  // do {
  //     unsigned long long x = rand64 ();
  //     int outbytes = nbytes < wordsize ? nbytes : wordsize;
  //     if (!writebytes (x, outbytes)){
  //       output_errno = errno;
  //       break;
  //     }
  //     nbytes -= outbytes;
  //   }
  // while (0 < nbytes);
  int output_errno = 0;
  long long bytes_left = options.nbytes;
  int word_size = sizeof(unsigned long long);

  char *buffer = NULL;
  int buffer_index = 0;

  if (options.output_type == OUTPUT_SIZE) {
    buffer = malloc(options.write_size);
    if (!buffer) {
      perror("malloc");
      return 1;
    }
  }

  while (bytes_left > 0) {
    unsigned long long value = rand64();
    int output_bytes = (bytes_left < word_size) ? bytes_left : word_size;

    for (int i = 0; i < output_bytes; ++i) {
      char byte = value & 0xFF;
      value >>= 8;

      if (options.output_type == OUTPUT_STDOUT) {
        if (!writebytes(byte, 1)) {
          output_errno = errno;
          goto cleanup;
        }
      } else { // OUTPUT_SIZE
        buffer[buffer_index++] = byte;

        if (buffer_index == options.write_size) {
          int written = write(STDOUT_FILENO, buffer, options.write_size);
          if (written != options.write_size) {
            output_errno = errno;
            goto cleanup;
          }
          buffer_index = 0;
        }
      }
    }

    bytes_left -= output_bytes;
  }

  if (options.output_type == OUTPUT_SIZE && buffer_index > 0) {
    int written = write(STDOUT_FILENO, buffer, buffer_index);
    if (written != buffer_index) {
      output_errno = errno;
    }
  }

  cleanup:
  if (buffer)
    free(buffer);

  if (fclose(stdout) != 0)
    output_errno = errno;

  if (output_errno) {
    errno = output_errno;
    perror("output");
  }

  finalize();
  return !!output_errno;
}
