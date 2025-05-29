#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

typedef enum { INPUT_RDRAND, INPUT_MRAND48, INPUT_FILE } InputType;

typedef enum { OUTPUT_STDOUT, OUTPUT_SIZE } OutputType;

// options structure
typedef struct {
    long long nbytes;
    InputType input_type;
    const char *input_file;
    OutputType output_type;
    int write_size;
    bool valid;
} Options;

Options parse_options(int argc, char **argv);

#endif 

