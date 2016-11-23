/* allocate.h
 */
#ifndef ALLOCATE
#define ALLOCATE
#include "exception.h"

extern char *allocate_exception;
void *allocate(unsigned n_bytes);
#endif
