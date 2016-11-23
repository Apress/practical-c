/* allocate.c
 */
#include <stdio.h>
#include <stdlib.h>
#include "exception.h"

char *allocate_exception = "Allocation failed";

void *allocate(unsigned n_bytes) {
  void *p = (n_bytes > 0) ? malloc(n_bytes): NULL;
  if (!p) THROW(allocate_exception);
  return p;
  }
