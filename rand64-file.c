#include "rand64-file.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *input_file = NULL;

void file_rand64_init(const char *filename) {
    input_file = fopen(filename, "rb");
    if (!input_file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }
}

unsigned long long file_rand64(void) {
    unsigned long long random_value;
    if (fread(&random_value, sizeof(random_value), 1, input_file) != 1) {
        perror("file read");
        exit(EXIT_FAILURE);
    }
    return random_value;
}

void file_rand64_fini(void) {
    if (input_file) {
        fclose(input_file);
        input_file = NULL;
    }
}
