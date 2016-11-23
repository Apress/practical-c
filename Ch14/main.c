/* main.c
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
#include "main.h"
#include "count.h"
#include "display.h"
#include "save_html.h"
#include "save_images.h"

// General parameters
#define SEED 22334455 // overwritten by argument 1
#define N_PUZ 1

// Global variables
// The coordinates of grid_op are always obtainable as the average between
// the coordinates of the surrounding cells with numbers (integer truncation)
int grid[N_NUM][N_NUM];
int grid_op[N_NUM][N_NUM];
int t_list[N_NUM * N_NUM / 3][4];  // ROW, COL, DIR, OP
int n_t;
int mat[SIDE][SIDE];
int z[SIDE][SIDE];

//                across   down     a_up   a_down
int incr[][2] = { {0, 1}, {1, 0}, {-1, 1}, {1, 1} };

// Operation strings.
// The order of operations must match the constants defined in main.h
// main.h:      ADD   SUB   MUL   DIV   EQU   INI
char ops[] = { '+',  '-',  'x',  '/',  '=',  ' ' };

//======================================================================== main
// Par 1: Pseudo-random seed; default is SEED
int seed = SEED;
int main(int argc, char *argv[]) {
  printf("*** MathSearch ***\n");

  // Process the first parameter if present and ignore the rest
  if (argc > 1) {
    seed = atoi(argv[1]);
    }
  srand(seed);
  printf("%d %d\n\n", N_NUM, seed);

  // Loop through the whole process for each puzzle you want to generate
  for (int k_puz = 0; k_puz < N_PUZ; k_puz++) {

    //---------------------------------------------------------- Initialization
    n_t = 0;
    for (int k = 0; k < N_NUM; k++) {
      for (int j = 0; j < N_NUM; j++) {
        grid[k][j] = 0;
        grid_op[k][j] = INI;
        }
      }

    //----------------------------------------------------- Define the triplets
    // Start from the approximate center
    int origin[2];
    origin[ROW] = N_NUM / 2 + rand() % (N_NUM / 3);
    origin[COL] = rand() % (N_NUM / 3);

    // Make a list of all cells roughly ordered by the distance from the origin
    int next[N_NUM * N_NUM][2] = {{0}};
    int n_next = 0;
    for (int i = 0; i < N_NUM  &&  n_next < N_NUM * N_NUM; i++) {
      int ku = origin[ROW] - i;
      if (ku < 0) ku = 0;
      int kd = origin[ROW] + i;
      if (kd >= N_NUM) kd = N_NUM - 1;
      int jl = origin[COL] - i;
      if (jl < 0) jl = 0;
      int jr = origin[COL] + i;
      if (jr >= N_NUM) jr = N_NUM - 1;
      for (int j = jl; j <= jr; j++) {
        if (grid[ku][j] == 0) {
          next[n_next][ROW] = ku;
          next[n_next][COL] = j;
          n_next++;
          grid[ku][j] = -1;
          }
        if (grid[kd][j] == 0) {
          next[n_next][ROW] = kd;
          next[n_next][COL] = j;
          n_next++;
          grid[kd][j] = -1;
          }
        }
      for (int k = ku; k <= kd; k++) {
        if (grid[k][jl] == 0) {
          next[n_next][ROW] = k;
          next[n_next][COL] = jl;
          n_next++;
          grid[k][jl] = -1;
          }
        if (grid[k][jr] == 0) {
          next[n_next][ROW] = k;
          next[n_next][COL] = jr;
          n_next++;
          grid[k][jr] = -1;
          }
        }
      }

/* Uncomment the following code to list next[][]
    {
      int ord[N_NUM][N_NUM];
      for (int i = 0; i < n_next; i++) {
        ord[next[i][ROW]][next[i][COL]] = i;
        }
      for (int k = 0; k < N_NUM; k++) {
        for (int j = 0; j < N_NUM; j++) {
          printf("%3d", ord[k][j]);
          }
        printf("\n");
        }
      }
*/

    // Place the triplets
    int i0 = 0;
    do {

      // Start from the available cell that is closest to the origin
      int k0 = next[i0][ROW];
      int j0 = next[i0][COL];

      // Determine which directions are within the grid
      int dirs0[4];
      int n_dirs0 = 0;
      for (int i = 0; i < 4; i++) {
        int kk = k0 + incr[i][ROW] + incr[i][ROW] ;
        if (kk >= 0  &&  kk < N_NUM) {
          int jj = j0 + incr[i][COL] + incr[i][COL];
          if (jj >= 0  &&  jj < N_NUM) {
            dirs0[n_dirs0] = i;
            n_dirs0++;
            }
          }
        }

      // Remove the directions that have one or more cells already used
      int dirs[4];
      int n_dirs = 0;
      for (int i = 0; i < n_dirs0; i++) {
        int k1 = k0 + incr[dirs0[i]][ROW];
        int j1 = j0 + incr[dirs0[i]][COL];
        int k2 = k1 + incr[dirs0[i]][ROW];
        int j2 = j1 + incr[dirs0[i]][COL];
        int kop = (k0 + k1) / 2;
        int jop = (j0 + j1) / 2;
        int kequ = (k1 + k2) / 2;
        int jequ = (j1 + j2) / 2;
        if (     grid[k0][j0] < 0  &&  grid[k1][j1] < 0  &&  grid[k2][j2] < 0
             &&  grid_op[kop][jop] != EQU
             &&  (grid_op[kequ][jequ] == INI  ||  grid_op[kequ][jequ] == EQU)
             ) {
          dirs[n_dirs] = dirs0[i];
          n_dirs++;
          }
        }

      // Select one of the directions and save the triplet.
      // In the process, also generate the opcode.
      if (n_dirs > 0) {
        int k_dir = rand() % n_dirs;
        t_list[n_t][ROW] = k0;
        t_list[n_t][COL] = j0;
        t_list[n_t][DIR] = dirs[k_dir];
        int dk = incr[dirs[k_dir]][ROW];
        int dj = incr[dirs[k_dir]][COL];
        int k1 = k0 + dk;
        int j1 = j0 + dj;
        int k2 = k1 + dk;
        int j2 = j1 + dj;
        grid[k0][j0] = n_t;
        grid[k1][j1] = n_t;
        grid[k2][j2] = n_t;
        int kop = (k0 + k1) / 2;
        int jop = (j0 + j1) / 2;
        int opcode;
        if (grid_op[kop][jop] == INI) {
          opcode = rand() % NUM_OPS;
          grid_op[kop][jop] = opcode;
          }
        else {
          opcode = grid_op[kop][jop];
          }
        int kequ = (k1 + k2) / 2;
        int jequ = (j1 + j2) / 2;
        grid_op[kequ][jequ] = EQU;
        t_list[n_t][OP] = opcode;
        n_t++;
        }

      // Either a triplet could be placed or not.
      // In either case, remove the starting cell from the list
      // so that you don't start from it again.
      i0++;
      } while (i0 < n_next);

    //---------------------------------------------------- Generate the numbers
    for (int k = 0; k < N_NUM; k++) {
      for (int j = 0; j < N_NUM; j++) {
        grid[k][j] = -1;
        }
      }

    for (int i = 0; i < n_t; i++) {
      int *trip = t_list[i];
      int k0 = trip[ROW];
      int j0 = trip[COL];
      int dir = trip[DIR];
      int k1 = k0 + incr[dir][ROW];
      int j1 = j0 + incr[dir][COL];
      int k2 = k1 + incr[dir][ROW];
      int j2 = j1 + incr[dir][COL];
      int n0;
      int n1;
      int n2;
      int nn;
      do {
        switch (trip[OP]) {
          case ADD: {
            n2 = rand() % (MAX_RES / 4 * 3) + MAX_RES / 4;
            n1 = rand() % (n2 / 2 - MAX_RES / 9) + MAX_RES / 9;
            n0 = n2 - n1;
            break;
            }

          case SUB: {
            n0 = rand() % (MAX_RES / 4 * 3) + MAX_RES / 4;
            n1 = rand() % (n0 / 2 - MAX_RES / 9) + MAX_RES / 9;
            n2 = n0 - n1;
            break;
            }

          case MUL: {
            n0 = rand() % (MAX_RES / FACT) + 3;
            do {
              n1 = rand() % (MAX_RES / FACT) + 3;
              n2 = n0 * n1;
              } while (n2 > MAX_RES);
            break;
            }

          default: {
            n1 = rand() % (MAX_RES / FACT) + 3;
            do {
              n2 = rand() % (MAX_RES / FACT) + 3;
              n0 = n1 * n2;
              } while (n0 > MAX_RES);
            break;
            }
          }
        grid[k0][j0] = n0;
        grid[k1][j1] = n1;
        grid[k2][j2] = n2;

        // Check whether the operation introduces ambiguities
        nn = count(k0, j0);
        if (nn == 1) nn = count(k1, j1);
        if (nn == 1) nn = count(k2, j2);
        } while (nn > 1);
      }

    //------------------------------------------------ Fill up the empty spaces
    for (int k = 0; k < N_NUM; k++) {
      for (int j = 0; j < N_NUM; j++) {
        if (grid[k][j] < 0) {
          do {
            grid[k][j] = rand() % 7 + 5;
            } while (count(k, j) > 0);
          }
        }
      }

    display();  // WARNING: Do not remove the display: it is needed for HTML


    //-------------------------- Save the puzzle and the solution to HTML files
    char name[32];  // certainly long enough
    sprintf(name, "%d_%d_%d_%d_%03d.html", N_NUM, MAX_RES, NUM_OPS, seed, k_puz);

    char id[32];  // certainly long enough
    sprintf(id, "%d %d %d %d %d", N_NUM, MAX_RES, NUM_OPS, seed, k_puz);

    int html_saved = save_html(name, id);
    printf("save_html %2d %s\n\n", k_puz, (html_saved) ? "successful" : "failed");

    sprintf(name, "%d_%d_%d_%d_%03d", N_NUM, MAX_RES, NUM_OPS, seed, k_puz);

    save_images(name);
    printf("save_images %2d done\n\n", k_puz);

    } // for (int k_puz..


	return EXIT_SUCCESS;
  }
