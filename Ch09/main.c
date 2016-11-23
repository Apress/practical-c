/* Sort: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include "search.h"
#include "sort.h"

#define N 100000

long n_comp;

int cmp_i(void *a_p, void *b_p);
int cmp_i_qsort(void *a_p, void *b_p);
void fill_random(void **ar, int *iar);
void fill_random_int(int *a);

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
/*
{
  // Define an array of integers and fill it up.
  int i_init[N];
  for (int k = 0; k < N; k++) i_init[k] = k;

  // Initialize an array of pointers with elements of i_init picked at random.
  void **ar = srch_ptr_alloc(N, NULL);
  srand(12345);
  fill_random(ar, i_init);
  Sort_list(ar);

  // Insertion and Shell sorts.
  n_comp = 0;
//  sort_insertion(ar, &cmp_i);
  sort_shell(ar, &cmp_i);
  Sort_list(ar);
  printf("#comparisons: %d\n", n_comp);
  srch_ptr_free(&ar);
  }
*/

/*
{ // Shell sort: Statistics on number of comparisons.
#define N_REP 100
  int dist[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
  int n_dist = sizeof(dist) / sizeof(int);
  int i_a[N];
  for (int k = 0; k < N; k++) i_a[k] = k;
  void **p_a = srch_ptr_alloc(N, NULL);
  srand(1);
  for (int k_dist = -1; k_dist < n_dist; k_dist++) {
    printf("%2d", (k_dist >= 0) ? dist[k_dist] : 1);
    for (int k_rep = 0; k_rep < N_REP; k_rep++) {
      fill_random(p_a, i_a);
      n_comp = 0;
      if (k_dist >= 0) {
        sort_shell_step(p_a, &cmp_i, dist[k_dist]);
//        sort_shell_step(p_a, &cmp_i, 3);
//        sort_shell_step(p_a, &cmp_i, 3);
        }
      sort_shell_step(p_a, &cmp_i, 1);
      printf("\t%d", n_comp);
      }
    printf("\n");
    }
 srch_ptr_free(&p_a);
  }
*/

/*
{ // Bubble sort.
#define N_REP 100
  srand(1234);
  n_comp = 0;
  int i_a[N];
  for (int k = 0; k < N; k++) i_a[k] = k;
  void **p_a = srch_ptr_alloc(N, NULL);
  for (int k_rep = 0; k_rep < N_REP; k_rep++) {
    fill_random(p_a, i_a);
    sort_bubble(p_a, &cmp_i);
    }
  printf("Bubble sort: %d\n", (int)((n_comp + 0.5) / N_REP));
  srch_ptr_free(&p_a);
  }
*/

/*
  { // Quicksort (pointer arrays).
  #define N_REP 100
    srand(12);
    n_comp = 0;
    int i_a[N];
    for (int k = 0; k < N; k++) i_a[k] = k;
    void **p_a = srch_ptr_alloc(N, NULL);
    for (int k_rep = 0; k_rep < N_REP; k_rep++) {
      fill_random(p_a, i_a);
      Sort_list(p_a);
      sort_quick(p_a, &cmp_i);
      }
    printf("Quicksort: %d\n", (int)((n_comp + 0.5) / N_REP));
  #if !SORT_DEBUG
    Sort_list(p_a);
  #endif
    srch_ptr_free(&p_a);
    }
*/
/*
  { // Quicksort (int arrays).
  #define N_REP 1
    int *a = srch_int_alloc(N, NULL);
    srand(12);
    n_comp = 0;
    for (int k_rep = 0; k_rep < N_REP; k_rep++) {
      fill_random_int(a);
      Sort_list_int(a);
      sort_quick_int(a);
      }
#if !SORT_DEBUG
    printf("Quicksort (int)\n");
    Sort_list_int(a);
#endif
    srch_int_free(&a);
    }
*/

  { // Comparing sort_quick() with qsort()
#include <time.h>
#define USE_QSORT 1
#if USE_QSORT
#  define Sort(ar) qsort(ar, *((int *)ar - 1), sizeof(int), cmp_i_qsort)
#else
#  define Sort(ar) sort_quick_int(ar)
#  endif
#define N_REP 1000
    srand(12);
    double total = 0;
    int *a = srch_int_alloc(N, NULL);
    for (int k_rep = 0; k_rep < N_REP; k_rep++) {
      fill_random_int(a);
      clock_t t0 = clock();
      Sort(a);
      clock_t t1 = clock();
      total += (double)(t1 - t0) / CLOCKS_PER_SEC * 1.E6;
      }
    printf("clock: %3.0f us\n", total / N_REP);
//    Sort_list_int(a);
    srch_int_free(&a);
    }
  return EXIT_SUCCESS;
  }

//----------------------------------------------------------------------- cmp_i
int cmp_i(void *a_p, void *b_p) {
#if SORT_LOG
  n_comp++;
#endif
  int a = *(int *)a_p;
  int b = *(int *)b_p;
  return (b == a) ? 0 : (b > a) ? 1 : -1;
  } // cmp_i

//----------------------------------------------------------------- cmp_i_qsort
int cmp_i_qsort(void *a_p, void *b_p) {
  int a = *(int *)a_p;
  int b = *(int *)b_p;
  return (b == a) ? 0 : (a > b) ? 1 : -1;
  } // cmp_i_qsort

//----------------------------------------------------------------- fill_random
void fill_random(void **ar, int *iar) {
  for (int k = 0; k < N; k++) ar[k] = NULL;
  for (int k = 0; k < N; k++) {
    unsigned int kk;
    do {
      kk = rand() % N;
      } while (ar[kk]);
    ar[kk] = iar + k;
    }
  *((int *)ar - 1) = N;
  } // fill_random

//------------------------------------------------------------- fill_random_int
void fill_random_int(int *a) {
  for (int k = 0; k < N; k++) a[k] = -1;
  for (int k = 0; k < N; k++) {
    unsigned int kk;
    do {
      kk = rand() % N;
      } while (a[kk] >= 0);
    a[kk] = k;
    }
  *(a - 1) = N;
  } // fill_random_int
