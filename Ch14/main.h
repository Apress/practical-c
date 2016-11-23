/* main.h
 *
 * General declarations
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef MAIN
#define MAIN

#define FALSE 0
#define TRUE  1

#define ROW 0
#define COL 1

#define ACROSS 0
#define DOWN   1
#define A_UP   2
#define A_DOWN 3

#define FORMAT 5

#if (FORMAT == 0)
#define N_NUM 6
#define NUM_OPS 2
#define MAX_RES 50
#define FACT 4  // not used

#elif (FORMAT == 1)
#define N_NUM 6
#define NUM_OPS 4
#define MAX_RES 50
#define FACT 4

#elif (FORMAT == 2)
#define N_NUM 8
#define NUM_OPS 2
#define MAX_RES 50
#define FACT 4  // not used

#elif (FORMAT == 3)
#define N_NUM 8
#define NUM_OPS 2
#define MAX_RES 75
#define FACT 4  // not used

#elif (FORMAT == 4)
#define N_NUM 8
#define NUM_OPS 4
#define MAX_RES 50
#define FACT 4

#elif (FORMAT == 5)
#define N_NUM 8
#define NUM_OPS 4
#define MAX_RES 75
#define FACT 4

#elif (FORMAT == 6)
#define N_NUM 10
#define NUM_OPS 2
#define MAX_RES 50
#define FACT 4  // not used

#elif (FORMAT == 7)
#define N_NUM 10
#define NUM_OPS 2
#define MAX_RES 75
#define FACT 4  // not used

#elif (FORMAT == 8)
#define N_NUM 10
#define NUM_OPS 2
#define MAX_RES 99
#define FACT 4  // not used

#elif (FORMAT == 9)
#define N_NUM 10
#define NUM_OPS 4
#define MAX_RES 50
#define FACT 4

#elif (FORMAT == 10)
#define N_NUM 10
#define NUM_OPS 4
#define MAX_RES 75
#define FACT 4

#elif (FORMAT == 11)
#define N_NUM 10
#define NUM_OPS 4
#define MAX_RES 99
#define FACT 4  // used to be 3, but it doesn't converge...
#endif

#define SIDE (N_NUM + N_NUM - 1)

// Operations codes
// The values must match the operation strings defined in ops[]
#define N_OPS 4
#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
//
#define EQU 4
#define INI 5

// All the variables declared here are defined in MathSearch.c
extern int grid[N_NUM][N_NUM];
extern int grid_op[N_NUM][N_NUM];
#define DIR 2
#define OP  3
extern int t_list[N_NUM * N_NUM / 3][4];  // ROW, COL, DIR, OP
extern int n_t;
extern int incr[][2];
extern int mat[SIDE][SIDE];
extern int z[SIDE][SIDE];

extern char ops[];

#endif
