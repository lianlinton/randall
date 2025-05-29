#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>

/* Write the lowest nbytes bytes of x to stdout in little-endian order.  */
bool writebytes(unsigned long long x, int nbytes);

#endif 