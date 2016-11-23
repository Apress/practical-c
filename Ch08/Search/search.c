/* search.c
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
#include "search.h"

//---------------------------------------------------------------- srch_int_add
// Returns the number of array elements occupied after the addition
// or -1 if unsuccessful.
int srch_int_add(int num, int **ar_p, int incr) {
  int *a = *ar_p;
  int *buf = a - 2;
  int max_n = buf[0];
  int n = buf[1];
  if (n >= max_n) {

    // The array is full.  Extend it.
    if (incr < 1) incr = 1;
    int new_max_n = max_n + incr;
    int *new_buf = realloc(buf, (new_max_n + 2) * sizeof(int));
    if (new_buf) {

      // The extension was successful. Check whether the block has moved,
      // clean up the new elements, and update the array capacity.
      if (new_buf != buf) {
        buf = new_buf;
        a = buf + 2;
        *ar_p = a;
        }
      (void)memset(&a[max_n], 0, (new_max_n - max_n) * sizeof(int));
      buf[0] = new_max_n;
#if SRCH_DEBUG
      printf("Buffer extended from %d to %d\n", max_n, new_max_n);
#endif
      }
    else {

      // The memory block couldn't be extended.
      // Try to allocate an expanded array from scratch.
      int *new_a = srch_int_alloc(new_max_n, NULL);
      if (new_a) {

        // The new allocation was successful. Copy the elements across,
        // free the old memory block, redefine the local variables,
        // set the array utilization, and update the array pointer
        // in the calling program.
#if SRCH_DEBUG
      printf("New buffer for %d elements allocated\n", new_max_n);
#endif
        (void)memcpy(new_a, a, max_n * sizeof(int));
        free(buf);
        a = new_a;
        buf = a - 2;
        buf[1] = n; // buf[0] already set within srch_int_alloc()
        *ar_p = a;
        } // if (new_a..
      else {

        // There is not enough space in the heap. No can do.
        return -1;                                                        //-->
        }
      } // if (new_buf.. else..
    } // if (n >= max_n..

  // Let's make the addition.
  a[n++] = num;
  buf[1] = n;
  return n;
  } // srch_int_add

//-------------------------------------------------------------- srch_int_alloc
// Returns the address of the array or NULL if unsuccessful.
int *srch_int_alloc(int n, int *a) {
  int *buf = NULL;
  if (a) {
    buf = malloc((n + 2) * sizeof(int));
    if (buf == NULL) return NULL;                                         //-->
    (void)memcpy(buf + 2, a, n * sizeof(int));
    buf[1] = n;
    }
  else {
    buf = calloc(n + 2, sizeof(int));
    if (buf == NULL) return NULL;                                         //-->
    }
  buf[0] = n;
  return buf + 2;
  } // srch_int_alloc

//---------------------------------------------------------------- srch_int_del
// Returns the number of array elements still occupied after the
// deletion or -1 if unsuccessful.
int srch_int_del(int k, int *ar) {
  int n = ar[-1];
  if (k < 0 || k >= n) return -1;                                         //-->
  ar[k] = ar[n - 1];
  ar[-1]--;
  return ar[-1];
  } // srch_int_del

//--------------------------------------------------------------- srch_int_find
// Returns the first position where the requested number is found
// or -1 if unsuccessful.
int srch_int_find(int num, int *ar) {
  int n = ar[-1];
  int kk = -1;
  for (int k = 0; k < n && kk < 0; k++) if (ar[k] == num) kk = k;
  return kk;
  } // srch_int_find

//--------------------------------------------------------------- srch_int_free
void srch_int_free(int **ar_p) {
  free(*ar_p - 2);
  *ar_p = NULL;
  } // srch_int_free

//---------------------------------------------------------------- srch_ptr_add
// Returns the number of array elements occupied after the addition
// or -1 if unsuccessful.
int srch_ptr_add(void *obj, void ***ar_p, int incr) {
  void **a = *ar_p;
  int *buf = (int *)a - 2;
  int max_n = buf[0];
  int n = buf[1];
  if (n >= max_n) {

    // The array is full.  Extend it.
    if (incr < 1) incr = 1;
    int new_max_n = max_n + incr;
    int *new_buf = realloc(buf, 2 * sizeof(int) + new_max_n * sizeof(void *));
    if (new_buf) {

      // The extension was successful. Check whether the block has moved,
      // clean up the new elements, and update the array capacity.
      if (new_buf != buf) {
        buf = new_buf;
        a = (void **)(buf + 2);
        *ar_p = a;
        }
      (void)memset(*ar_p + max_n, 0, (new_max_n - max_n) * sizeof(void *));
      buf[0] = new_max_n;
#if SRCH_DEBUG
      printf("Buffer extended from %d to %d\n", max_n, new_max_n);
#endif
      }
    else {

      // The memory block couldn't be extended.
      // Try to allocate an expanded array from scratch.
      void **new_a = srch_ptr_alloc(new_max_n, NULL);
      if (new_a) {

        // The new allocation was successful. Copy the elements across,
        // free the old memory block, redefine the local variables,
        // set the array utilization, and update the array pointer
        // in the calling program.
#if SRCH_DEBUG
      printf("New buffer for %d elements allocated\n", new_max_n);
#endif
        (void)memcpy(new_a, a, max_n * sizeof(void *));
        free(buf);
        a = new_a;
        buf = (int *)a - 2;
        buf[1] = n; // buf[0] already set within srch_ptr_alloc()
        *ar_p = a;
        } // if (new_a..
      else {

        // There is not enough space in the heap. No can do.
        return -1;                                                        //-->
        }
      } // if (new_buf.. else..
    } // if (n >= max_n..

  // Let's make the addition.
  a[n++] = obj;
  buf[1] = n;
  return n;
  } // srch_ptr_add

//-------------------------------------------------------------- srch_ptr_alloc
// Returns the address of the array or NULL if unsuccessful.
void **srch_ptr_alloc(int n, void *a) {
  int *buf = NULL;
  size_t n_bytes = 2 * sizeof(int) + n * sizeof(void *);
  buf = malloc(n_bytes);
  if (buf == NULL) return NULL;                                           //-->
  if (a) {
    (void)memcpy(buf + 2, a, n * sizeof(void *));
    buf[1] = n;
    }
  else {
    (void)memset(buf, 0, n_bytes);
    }
  buf[0] = n;
  return (void **)(buf + 2);
  } // srch_ptr_alloc

//---------------------------------------------------------------- srch_ptr_del
// Returns the number of array elements still occupied after the
// deletion or -1 if unsuccessful.
int srch_ptr_del(int k, void **ar) {
  int *n_p = (int *)ar - 1;
  int n = *n_p;
  if (k < 0 || k >= n) return -1;                                         //-->
  ar[k] = ar[n - 1];
  n--;
  *n_p = n;
  return n;
  } // srch_ptr_del

//------------------------------------------------------------ srch_ptr_del_ord
// Returns the number of array elements still occupied after the
// deletion or -1 if unsuccessful.
int srch_ptr_del_ord(int k, void **ar) {
  int *n_p = (int *)ar - 1;
  int n = *n_p;
  if (k < 0 || k >= n) return -1;                                         //-->
  (void)memmove(&ar[k], &ar[k + 1], (n - k - 1)*sizeof(void *));
//  for (int j = k + 1; j < n; j++) ar[j - 1] = ar[j];
  n--;
  *n_p = n;
  return n;
  } // srch_ptr_del_ord

//--------------------------------------------------------------- srch_ptr_find
// Returns the first position where the requested element is found
// or -1 if unsuccessful.
int srch_ptr_find(void *obj, void **ar, int (*cmp)(void *, void *)) {
  int *n_p = (int *)ar - 1;
  int kk = -1;
  for (int k = 0; k < *n_p && kk < 0; k++) if (!(*cmp)(ar[k], obj)) kk = k;
  return kk;
  } // srch_ptr_find

//----------------------------------------------------------- srch_ptr_find_ord
// Returns the position where the requested element is found or the
// position where that element should have been, but after changing
// its sign and decrementing it by 1.
int srch_ptr_find_ord(void *obj, void **ar, int (*cmp)(void *, void *)) {
  int n = *((int *)ar - 1);
  if (n <= 0) return -1;                                                  //-->
  int k0 = 0;
  int k1 = n - 1;
  int k = (k0 + k1) / 2;
  int cmp_res;
  do {
    cmp_res = (*cmp)(ar[k], obj);
    if (cmp_res) {
      if (cmp_res > 0) k0 = k + 1;
      else k1 = k - 1;
      k = (k0 + k1) / 2;
      }
    } while (cmp_res && k1 >= k0);
  return (cmp_res) ? -k0 - 1 : k;
  } // srch_ptr_find_ord

//--------------------------------------------------------------- srch_ptr_free
void srch_ptr_free(void ***ar_p) {
  free((int *)*ar_p - 2);
  *ar_p = NULL;
  } // srch_ptr_free

//------------------------------------------------------------ srch_ptr_ins_ord
// Returns the number of array elements occupied after the insertion,
// -1 if the value already exists, or -2 if unsuccessful for other reasons.
int srch_ptr_ins_ord(void *obj, void ***ar_p, int (*cmp)(void *, void *),
    int incr) {
  int k = srch_ptr_find_ord(obj, *ar_p, cmp);
  if (k >= 0) return -1;                                                  //-->
  k = -k - 1;
  int n = srch_ptr_add(obj, ar_p, incr);
  if (n <= 1) return (n == 1) ? 1 : -2;                                   //-->
  if (k == n - 1) return n;                                               //-->
  void **ar = *ar_p;
  (void)memmove(&ar[k + 1], &ar[k], (n - k - 1)*sizeof(void *));
//  for (int j = n - 1; j > k; j--) ar[j] = ar[j - 1];
  ar[k] = obj;
  return n;
  } // srch_ptr_ins_ord

//---------------------------------------------------------------- srch_lin_int
// Returns the position if found or -1 if not found.
int srch_lin_int(int num, int *ar, int n) {
  int kk = -1;
  for (int k = 0; k < n && kk < 0; k++) if (ar[k] == num) kk = k;
  return kk;
  } // srch_lin_int
