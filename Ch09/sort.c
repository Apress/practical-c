/* sort.c
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
#include "sort.h"

int  partition(void **ar, int (*cmp)(void *, void *), int k1, int k2);
int  partition_int(int *a, int k1, int k2);
void quick_step(void **ar, int (*cmp)(void *, void *), int k1, int k2);
void quick_step_int(int *a, int k1, int k2);

//------------------------------------------------------------------- partition
// Returns the position of the pivot.  Only works with unique keys.
int partition(void **ar, int (*cmp)(void *, void *), int k1, int k2) {
  int k_pivot = (k1 + k2) >> 1;
  void *pivot = ar[k_pivot];
  {
    if ((*cmp)(ar[k1], pivot) > 0) {
      if ((*cmp)(ar[k2], pivot) > 0) { // pivot has the largest key
        k_pivot = ((*cmp)(ar[k1], ar[k2]) > 0) ? k2 : k1;
        pivot = ar[k_pivot];
        }
      }
    else if ((*cmp)(pivot, ar[k2]) > 0) { // pivot has the smallest key
      k_pivot = ((*cmp)(ar[k1], ar[k2]) > 0) ? k1 : k2;
      pivot = ar[k_pivot];
      }
    }
  int j1 = k1 - 1;
  int j2 = k2 + 1;
  int i1;
  int i2;
  int swaps;
  void *tmp;
#if SORT_DEBUG
  printf("partition:\n    k_pivot=%d pivot=%d\n", k_pivot, *(int *)pivot);
#endif
  do {
    swaps = 0;
    do { i1 = (*cmp)(pivot, ar[++j1]); } while (i1 < 0);
    do { i2 = (*cmp)(pivot, ar[--j2]); } while (i2 > 0);
#if SORT_DEBUG
    printf("    j1=%d (%d) i1=%d j2=%d (%d) i2=%d\n",
        j1, *(int *)ar[j1], i1, j2, *(int *)ar[j2], i2);
#endif
    if (i1 > 0) { // something before pivot?
      if (i2 < 0) { // something after pivot?  Do the swap.
        tmp = ar[j1];
        ar[j1] = ar[j2];
        ar[j2] = tmp;
#if SORT_DEBUG
        printf("    swap %d and %d:\n", j1, j2);
        Sort_list(ar);
#endif
        swaps = 1;
        } // if (i2 < 0..
      else { // nothing after the pivot? Move the pivot left.
        int kp = k_pivot;
        do {
          while (kp > k1 && i1 > 0) {
            kp--;
            i1 = (*cmp)(pivot, ar[kp]);
            if (i1 > 0) {
#if SORT_DEBUG
              printf("    move pivot (%d) from %d to %d:\n",
                  *(int *)pivot, k_pivot, k_pivot - 1);
#endif
              ar[k_pivot] = ar[kp];
              ar[kp] = pivot;
              k_pivot = kp;
#if SORT_DEBUG
              Sort_list(ar);
#endif
              }
            }
          if (kp > j1) {
#if SORT_DEBUG
            printf("    three-way swap: %d (%d) to %d,"
                "%d (%d) to %d, and pivot in %d (%d) to %d:\n",
                j1, *(int *)ar[j1], k_pivot, kp, *(int *)ar[kp], j1,
                k_pivot, *(int *)pivot, kp);
#endif
            ar[k_pivot] = ar[j1];
            ar[j1] = ar[kp];
            ar[kp] = pivot;
            k_pivot = kp;
#if SORT_DEBUG
            Sort_list(ar);
#endif
            // There can still be elements between j1 and kp (both
            // excluded) with a key greater than that of the pivot and
            // we already know that there are no elements on the right
            // of the pivot to be moved to its left.
            do { i1 = (*cmp)(pivot, ar[++j1]); } while (i1 < 0);
            }
          else {
            i1 = -100;
            }
          } while (kp > k1 && i1 > 0);
        } // if (i2 < 0.. else..
      } // if (i1 > 0..
    else { // nothing before the pivot.
      if (i2 < 0) { // something after pivot?  Move the pivot right.
        int kp = k_pivot;
        do {
          while (kp < k2 && i2 < 0) {
            kp++;
            i2 = (*cmp)(pivot, ar[kp]);
            if (i2 < 0) {
#if SORT_DEBUG
              printf("    move pivot (%d) from %d to %d:\n",
                  *(int *)pivot, k_pivot, k_pivot + 1);
#endif
              ar[k_pivot] = ar[kp];
              ar[kp] = pivot;
              k_pivot = kp;
#if SORT_DEBUG
              Sort_list(ar);
#endif
              }
            }
          if (kp < j2) {
#if SORT_DEBUG
            printf("    three-way swap: %d (%d) to %d,"
                " %d (%d) to %d, and pivot in %d (%d) to %d\n",
                j2, *(int *)ar[j2], k_pivot, kp, *(int *)ar[kp], j2,
                k_pivot, *(int *)pivot, kp);
#endif
            ar[k_pivot] = ar[j2];
            ar[j2] = ar[kp];
            ar[kp] = pivot;
            k_pivot = kp;
#if SORT_DEBUG
            Sort_list(ar);
#endif
            // There can still be elements between kp and j2 (both
            // excluded) with a key less than that of the pivot and
            // we already know that there are no elements on the left
            // of the pivot to be moved to its right.
            do { i2 = (*cmp)(pivot, ar[--j2]); } while (i2 > 0);
            }
          else {
            i2 = 100;
            }
          } while (kp < k2 && i2 < 0);
        } // if (i2 < 0
      } // if (i1 > 0.. else..
    } while (swaps);
  return k_pivot;
  } // partition

//--------------------------------------------------------------- partition_int
// Returns the position of the pivot.  Only works with unique keys.
int partition_int(int *a, int k1, int k2) {
  int k_pivot = (k1 + k2) >> 1;
  int pivot = a[k_pivot];
  {
    if (pivot > a[k1]) {
      if (pivot > a[k2]) { // pivot is largest
        k_pivot = (a[k2] > a[k1]) ? k2 : k1;
        pivot = a[k_pivot];
        }
      }
    else if (a[k2] > pivot) { // pivot is smallest
      k_pivot = (a[k2] > a[k1]) ? k1 : k2;
      pivot = a[k_pivot];
      }
    }
  int j1 = k1 - 1;
  int j2 = k2 + 1;
  int i1;
  int i2;
  int swaps;
  int tmp;
#if SORT_DEBUG
  printf("partition:\n    k_pivot=%d pivot=%d\n", k_pivot, pivot);
#endif
  do {
    swaps = 0;
    do { i1 = a[++j1] - pivot; } while (i1 < 0);
    do { i2 = a[--j2] - pivot; } while (i2 > 0);
#if SORT_DEBUG
    printf("    j1=%d (%d) i1=%d j2=%d (%d) i2=%d\n",
        j1, a[j1], i1, j2, a[j2], i2);
#endif
    if (i1 > 0) { // something before pivot?
      if (i2 < 0) { // something after pivot?  Do the swap.
        tmp = a[j1];
        a[j1] = a[j2];
        a[j2] = tmp;
#if SORT_DEBUG
        printf("    swap %d and %d:\n", j1, j2);
        Sort_list_int(a);
#endif
        swaps = 1;
        } // if (i2 < 0..
      else { // nothing after the pivot? Move the pivot left.
        int kp = k_pivot;
        do {
          while (kp > k1 && i1 > 0) {
            kp--;
            i1 = a[kp] - pivot;
            if (i1 > 0) {
#if SORT_DEBUG
              printf("    move pivot (%d) from %d to %d:\n",
                  pivot, k_pivot, k_pivot - 1);
#endif
              a[k_pivot] = a[kp];
              a[kp] = pivot;
              k_pivot = kp;
#if SORT_DEBUG
              Sort_list_int(a);
#endif
              }
            }
          if (kp > j1) {
#if SORT_DEBUG
            printf("    three-way swap: %d (%d) to %d,"
                "%d (%d) to %d, and pivot in %d (%d) to %d:\n",
                j1, a[j1], k_pivot, kp, a[kp], j1, k_pivot, pivot, kp);
#endif
            a[k_pivot] = a[j1];
            a[j1] = a[kp];
            a[kp] = pivot;
            k_pivot = kp;
#if SORT_DEBUG
            Sort_list_int(a);
#endif
            do { i1 = a[++j1] - pivot; } while (i1 < 0);
            }
          else {
            i1 = -100;
            }
          } while (kp > k1 && i1 > 0);
        } // if (i2 < 0.. else..
      } // if (i1 > 0..
    else { // nothing before the pivot.
      if (i2 < 0) { // something after pivot?  Move the pivot right.
        int kp = k_pivot;
        do {
          while (kp < k2 && i2 < 0) {
            kp++;
            i2 = a[kp] - pivot;
            if (i2 < 0) {
#if SORT_DEBUG
              printf("    move pivot (%d) from %d to %d:\n",
                  pivot, k_pivot, k_pivot + 1);
#endif
              a[k_pivot] = a[kp];
              a[kp] = pivot;
              k_pivot = kp;
#if SORT_DEBUG
              Sort_list_int(a);
#endif
              }
            }
          if (kp < j2) {
#if SORT_DEBUG
            printf("    three-way swap: %d (%d) to %d,"
                " %d (%d) to %d, and pivot in %d (%d) to %d\n",
                j2, a[j2], k_pivot, kp, a[kp], j2, k_pivot, pivot, kp);
#endif
            a[k_pivot] = a[j2];
            a[j2] = a[kp];
            a[kp] = pivot;
            k_pivot = kp;
#if SORT_DEBUG
            Sort_list_int(a);
#endif
            do { i2 = a[--j2] - pivot; } while (i2 > 0);
            }
          else {
            i2 = 100;
            }
          } while (kp < k2 && i2 < 0);
        } // if (i2 < 0
      } // if (i1 > 0.. else..
    } while (swaps);
  return k_pivot;
  } // partition_int

//--------------------------------------------------------- partition_int_macro
// Returns the position of the pivot.  Only works with unique keys.
int partition_int_macro(int *a, int k1, int k2) {

#define Move_pivot_int(kk, ii, jj, k_dir) {                    \
  int sort_debug = SORT_DEBUG;                                 \
  int kp = k_pivot;                                            \
  do {                                                         \
    while (kp * k_dir < kk * k_dir && ii * k_dir < 0) {        \
      kp += k_dir;                                             \
      ii = a[kp] - pivot;                                      \
      if (ii * k_dir < 0) {                                    \
if (sort_debug) printf("    move pivot (%d) from %d to %d:\n", \
    pivot, k_pivot, k_pivot - 1);                              \
        a[k_pivot] = a[kp];                                    \
        a[kp] = pivot;                                         \
        k_pivot = kp;                                          \
if (sort_debug) Sort_list_int(a);                              \
        }                                                      \
      }                                                        \
    if (kp * k_dir < jj * k_dir) {                             \
if (sort_debug) printf("    three-way swap: %d (%d) to %d,"    \
    "%d (%d) to %d, and pivot in %d (%d) to %d:\n",            \
    j1, a[j1], k_pivot, kp, a[kp], j1, k_pivot, pivot, kp);    \
      a[k_pivot] = a[jj];                                      \
      a[jj] = a[kp];                                           \
      a[kp] = pivot;                                           \
      k_pivot = kp;                                            \
if (sort_debug) Sort_list_int(a);                              \
      do {                                                     \
        jj -= k_dir;                                           \
        ii = a[jj] - pivot;                                    \
        } while (ii * k_dir > 0);                              \
      }                                                        \
    else {                                                     \
      ii = k_dir;                                              \
      }                                                        \
    } while (kp * k_dir < kk * k_dir && ii * k_dir < 0);       \
  }

//------ Function begins -------
  int k_pivot = (k1 + k2) >> 1;
  int pivot = a[k_pivot];
  {
    if (pivot > a[k1]) {
      if (pivot > a[k2]) { // pivot is largest
        k_pivot = (a[k2] > a[k1]) ? k2 : k1;
        pivot = a[k_pivot];
        }
      }
    else if (a[k2] > pivot) { // pivot is smallest
      k_pivot = (a[k2] > a[k1]) ? k1 : k2;
      pivot = a[k_pivot];
      }
    }
  int j1 = k1 - 1;
  int j2 = k2 + 1;
  int i1;
  int i2;
  int swaps;
  int tmp;
#if SORT_DEBUG
  printf("partition:\n    k_pivot=%d pivot=%d\n", k_pivot, pivot);
#endif
  do {
    swaps = 0;
    do { i1 = a[++j1] - pivot; } while (i1 < 0);
    do { i2 = a[--j2] - pivot; } while (i2 > 0);
#if SORT_DEBUG
    printf("    j1=%d (%d) i1=%d j2=%d (%d) i2=%d\n",
        j1, a[j1], i1, j2, a[j2], i2);
#endif
    if (i1 > 0) { // something before pivot?
      if (i2 < 0) { // something after pivot?  Do the swap.
        tmp = a[j1];
        a[j1] = a[j2];
        a[j2] = tmp;
#if SORT_DEBUG
        printf("    swap %d and %d:\n", j1, j2);
        Sort_list_int(a);
#endif
        swaps = 1;
        } // if (i2 < 0..
      else { // nothing after the pivot? Move the pivot left.
        Move_pivot_int(k1, i1, j1, -1);
        } // if (i2 < 0.. else..
      } // if (i1 > 0..
    else { // nothing before the pivot.
      if (i2 < 0) { // something after pivot?  Move the pivot right.
        Move_pivot_int(k2, i2, j2, 1);
        } // if (i2 < 0
      } // if (i1 > 0.. else..
    } while (swaps);
  return k_pivot;
  } // partition_int_macro

//------------------------------------------------------------------ quick_step
void quick_step(void **ar, int (*cmp)(void *, void *), int k1, int k2) {
#if SORT_DEBUG
  printf("\nquick_step: k1=%d k2=%d\n", k1, k2);
#endif
  int k_pivot = partition(ar, cmp, k1, k2);
  if (k_pivot - 1 - k1 > 0) quick_step(ar, cmp, k1, k_pivot - 1);
  if (k2 - k_pivot - 1 > 0) quick_step(ar, cmp, k_pivot + 1, k2);
  } // quick_step

//-------------------------------------------------------------- quick_step_int
void quick_step_int(int *a, int k1, int k2) {
#if SORT_DEBUG
  printf("\nquick_step_int: k1=%d k2=%d\n", k1, k2);
#endif
  int k_pivot = partition_int(a, k1, k2);
  if (k_pivot - 1 - k1 > 0) quick_step_int(a, k1, k_pivot - 1);
  if (k2 - k_pivot - 1 > 0) quick_step_int(a, k_pivot + 1, k2);
  } // quick_step_int

//----------------------------------------------------------------- sort_bubble
void sort_bubble(void **ar, int (*cmp)(void *, void *)) {
  int n = *((int *)ar - 1);
  if (n <= 1) return;                                                     //-->
  int swaps;
  do {
    swaps = 0;
    for (int k = 0; k < n - 1; k++) {
      void *e1 = ar[k];
      void *e2 = ar[k + 1];
      if ((*cmp)(e1, e2) < 0) {
        ar[k] = e2;
        ar[k + 1] = e1;
        swaps = 1;
        }
      }
    } while (swaps);
  } // sort_bubble

//-------------------------------------------------------------- sort_insertion
void sort_insertion(void **ar, int (*cmp)(void *, void *)) {
  sort_shell_step(ar, cmp, 1);
  } // sort_insertion

//------------------------------------------------------------------ sort_quick
void sort_quick(void **ar, int (*cmp)(void *, void *)) {
  int n = *((int *)ar - 1);
  if (n <= 1) return;                                                     //-->
  quick_step(ar, cmp, 0, n - 1);
  } // sort_quick

//-------------------------------------------------------------- sort_quick_int
void sort_quick_int(int *a) {
  int n = *(a - 1);
  if (n <= 1) return;                                                     //-->
  quick_step_int(a, 0, n - 1);
  } // sort_quick_int

//------------------------------------------------------------------ sort_shell
void sort_shell(void **ar, int (*cmp)(void *, void *)) {
  sort_shell_step(ar, cmp, 6);
  sort_shell_step(ar, cmp, 3);
  sort_shell_step(ar, cmp, 1);
  } // sort_shell

//------------------------------------------------------------- sort_shell_step
void sort_shell_step(void **ar, int (*cmp)(void *, void *), int dist) {
  int n = *((int *)ar - 1);
  if (n <= 1) return;                                                     //-->
  for (int k = 1; k < n; k++) {
    void *el = ar[k];
    int j = k - 1;
    while (j >= 0 && (*cmp)(el, ar[j]) >= 0) j -= dist;
    j = (j < 0) ? 0 : j + 1;
    if (j < k) {
      (void)memmove(&ar[j + 1], &ar[j], (k - j)*sizeof(void *));
      ar[j] = el;
      }
    }
  } // sort_shell_step
