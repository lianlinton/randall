#ifndef RAND64_SW_H
#define RAND64_SW_H

#include <stdbool.h>

/* Initialize the software rand64 implementation.  */
void software_rand64_init(void);

/* Return a random value, using software operations.  */
unsigned long long software_rand64(void);

/* Finalize the software rand64 implementation.  */
void software_rand64_fini(void);

#endif 
