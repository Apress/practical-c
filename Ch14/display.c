/* display.c
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
#include "display.h"

void display() {

  // Initialize the output arrays
  for (int k = 0; k < N_NUM + N_NUM - 1; k++) {
    for (int j = 0; j < N_NUM + N_NUM - 1; j++) {
      mat[k][j] = -1;
      z[k][j] = -1;
      }
    }

  // Copy the numbers to the puzzle array
  for (int k = 0; k < N_NUM; k++) {
    for (int j = 0; j < N_NUM; j++) {
      mat[k + k][j + j] = grid[k][j];
      }
    }

  // Copy the operations to the puzzle array and
  // set the cell background array used for the HTML output
  for (int i = 0; i < n_t; i++) {
    int *t = t_list[i];
    int k0 = t[ROW];
    int j0 = t[COL];
    int dir = t[DIR];
    int k1 = k0 + incr[dir][ROW];
    int j1 = j0 + incr[dir][COL];
    int k2 = k1 + incr[dir][ROW];
    int j2 = j1 + incr[dir][COL];
    k0 += k0;
    k1 += k1;
    k2 += k2;
    j0 += j0;
    j1 += j1;
    j2 += j2;
    int kop = (k0 + k1) / 2;
    int jop = (j0 + j1) / 2;
    int kequ = (k1 + k2) / 2;
    int jequ = (j1 + j2) / 2;
    mat[kop][jop] = t[OP];
    mat[kequ][jequ] = EQU;

    // Set the cell background array
    int ii = dir * 10;
    z[k0][j0] = ii;
    z[k1][j1] = ii + 1;
    z[k2][j2] = ii + 2;
    if(dir == A_UP) {
      z[kop][jop]   = (z[kop][jop] < 0)   ? ii + 1 : 90;
      z[kequ][jequ] = (z[kequ][jequ] < 0) ? ii + 1 : 90;

      z[kop][jop - 1] = (z[kop][jop - 1] < 0) ? ii + 5 : ((z[kop][jop - 1] < 30) ? ii + 7 : 91);
      z[kop - 1][jop] = (z[kop - 1][jop] < 0) ? ii + 5 : ((z[kop - 1][jop] < 30) ? ii + 7 : 92);
      z[kop][jop + 1] = (z[kop][jop + 1] < 0) ? ii + 6 : ((z[kop][jop + 1] < 30) ? ii + 7 : 93);
      z[kop + 1][jop] = (z[kop + 1][jop] < 0) ? ii + 6 : ((z[kop + 1][jop] < 30) ? ii + 7 : 94);

      z[kequ][jequ - 1] = (z[kequ][jequ - 1] < 0) ? ii + 5 : ((z[kequ][jequ - 1] < 30) ? ii + 7 : 91);
      z[kequ - 1][jequ] = (z[kequ - 1][jequ] < 0) ? ii + 5 : ((z[kequ - 1][jequ] < 30) ? ii + 7 : 92);
      z[kequ][jequ + 1] = (z[kequ][jequ + 1] < 0) ? ii + 6 : ((z[kequ][jequ + 1] < 30) ? ii + 7 : 93);
      z[kequ + 1][jequ] = (z[kequ + 1][jequ] < 0) ? ii + 6 : ((z[kequ + 1][jequ] < 30) ? ii + 7 : 94);
      }
    else if(dir == A_DOWN) {
      z[kop][jop] = (z[kop][jop] < 0)   ? ii + 1 : 90;
      z[kequ][jequ] = (z[kequ][jequ] < 0)   ? ii + 1 : 90;

      z[kop][jop - 1] = (z[kop][jop - 1] < 0) ? ii + 6 : ((z[kop][jop - 1] > 30) ? ii + 7 : 91);
      z[kop - 1][jop] = (z[kop - 1][jop] < 0) ? ii + 5 : ((z[kop - 1][jop] > 30) ? ii + 7 : 92);
      z[kop][jop + 1] = (z[kop][jop + 1] < 0) ? ii + 5 : ((z[kop][jop + 1] > 30) ? ii + 7 : 93);
      z[kop + 1][jop] = (z[kop + 1][jop] < 0) ? ii + 6 : ((z[kop + 1][jop] > 30) ? ii + 7 : 94);

      z[kequ][jequ - 1] = (z[kequ][jequ - 1] < 0) ? ii + 6 : ((z[kequ][jequ - 1] > 30) ? ii + 7 : 91);
      z[kequ - 1][jequ] = (z[kequ - 1][jequ] < 0) ? ii + 5 : ((z[kequ - 1][jequ] > 30) ? ii + 7 : 92);
      z[kequ][jequ + 1] = (z[kequ][jequ + 1] < 0) ? ii + 5 : ((z[kequ][jequ + 1] > 30) ? ii + 7 : 93);
      z[kequ + 1][jequ] = (z[kequ + 1][jequ] < 0) ? ii + 6 : ((z[kequ + 1][jequ] > 30) ? ii + 7 : 94);
      }
    else {
      z[kop][jop] = ii + 1;
      z[kequ][jequ] = ii + 1;
      }

    } // for (int i..

  // Display the puzzle on the console
  for (int k = 0; k < N_NUM + N_NUM - 1; k++) {
    for (int j = 0; j < N_NUM + N_NUM - 1; j++) {
      if (mat[k][j] == -1) {
        printf("   ");
        }
      else if (k % 2 == 0  &&  j % 2 == 0){
        printf("%3d", mat[k][j]);
        }
      else {
        printf("  %c", ops[mat[k][j]]);
        }
      }
    printf("\n");
    }

  }
