#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>

Options parse_options(int argc, char **argv) {
    /* Definition: 
    typedef struct {
    long long nbytes;
    InputType input_type;
    const char *input_file;
    OutputType output_type;
    int write_size;
    bool valid;
    } Options;
     */

    // default option
    Options opts = {
        .nbytes = 0,
        .input_type = INPUT_RDRAND,
        .input_file = NULL,
        .output_type = OUTPUT_STDOUT,
        .write_size = 0,
        .valid = false,
    };

    int option;
    while ((option = getopt(argc, argv, "i:o:")) != -1) {
        switch (option) {
            case 'i':
                if (strcmp(optarg, "rdrand") == 0) {
                    opts.input_type = INPUT_RDRAND;
                } else if (strcmp(optarg, "mrand48_r") == 0) {
                    opts.input_type = INPUT_MRAND48;
                } else {
                    opts.input_type = INPUT_FILE;
                    opts.input_file = optarg;
                } //else {
                //     fprintf(stderr, "Invalid input source: %s\n", optarg);
                //     return opts;
                // }
                break;

            case 'o':
                if (strcmp(optarg, "stdio") == 0) {
                    opts.output_type = OUTPUT_STDOUT;
                } else {
                    char *endptr;
                    long value = strtol(optarg, &endptr, 10);
                    if (*endptr == '\0' && value > 0) {
                        opts.output_type = OUTPUT_SIZE;
                        opts.write_size = value;
                    } else {
                        fprintf(stderr, "Invalid output option: %s\n", optarg);
                        return opts;
                    }
                }
                break;

            default:
                fprintf(stderr, "Usage: %s [-i input] [-o output] [number of random bytes]\n", argv[0]);
                return opts;
        }
    }

    if (optind < argc) {
        const char *nbytes_arg = argv[optind];
        char *endptr;

        errno = 0;  
        opts.nbytes = strtoll(nbytes_arg, &endptr, 10);
        int parse_error = errno;

        bool parsed_fully = (*endptr == '\0');
        bool is_valid_size = (opts.nbytes >= 0);

        if (parse_error == 0 && parsed_fully && is_valid_size) {
            opts.valid = true;
        } else {
            perror(nbytes_arg);  
        }
    } else {
        fprintf(stderr, "Missing required argument.\n");
    }

    return opts;

}
