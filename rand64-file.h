#ifndef RAND64_FILE_H
#define RAND64_FILE_H

#include <stdbool.h>

/* Initialize file-based random input from given file */
void file_rand64_init(const char *filename);

/* Return a random value from file */
unsigned long long file_rand64(void);

/* Finalize input */
void file_rand64_fini(void);

#endif
