/* count.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "count.h"

int count(int kk, int jj) {
  int nn = 0;
  for (int i2 = 0; i2 < 4; i2++) {  // in all directions
    int dk = incr[i2][ROW];
    int dj = incr[i2][COL];
    for (int i3 = 2; i3 >= 0; i3--) {  // in all positions
      int kz = kk - dk * i3;
      int jz = jj - dj * i3;
      int ko = kz + dk;
      int jo = jz + dj;
      int kt = ko + dk;
      int jt = jo + dj;
      if (kz >= 0  &&  kt >= 0  &&  kt < N_NUM  &&  jz >= 0  &&  jt >= 0  &&  jt < N_NUM) {
        int nn0 = grid[kz][jz];
        int nn1 = grid[ko][jo];
        int nn2 = grid[kt][jt];
        if (nn0 > 0  &&  nn1 > 0  &&  nn2 > 0) {
          for (int i4 = 0; i4 < N_OPS; i4++) {  // all operations
            switch (i4) {
              case ADD:
                if (nn0 + nn1 == nn2) nn++;
                break;
              case SUB:
                if (nn0 - nn1 == nn2) nn++;
                break;
              case MUL:
                if (nn0 * nn1 == nn2) nn++;
                break;
              default:
                if (nn2 * nn1 == nn0) nn++;
                break;
              }
            } // 4
          } // if
        } // if
      } // 3
    } // 2
  return nn;
  }
