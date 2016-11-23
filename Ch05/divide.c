/* Try_catch: divide.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <stdio.h>
#include "exception.h"

char *divide_exception = "Division by zero";

int divide(int n1, int n2) {
  if (n2 == 0) THROW(divide_exception);
  return n1 / n2;
  }
