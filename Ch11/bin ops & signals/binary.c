/* binary.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.h"

//----------------------------------------------------------------- bin_to_uint
unsigned int bin_to_uint(char *s) {
  if (!s || !*s) return 0;                                                //-->
  while (*s && *s != '0' && *s != '1') s++;
  if (strlen(s) >= 2 && *s == '0' && (*(s + 1) == 'b' || *(s + 1) == 'B')) {
    s += 2;
    }
  unsigned int res = 0;
  while (*s) {
    if (*s == '0' || *s == '1') res = (res << 1) | (*s & 1);
    s++;
    }
  return res;
  } // bin_to_uint
