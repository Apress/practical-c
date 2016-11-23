/* dar.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "dar.h"

#define Get_el_size(ar) (((Dar *)((void *)(ar) - sizeof(Dar)))->size)
#define Get_n_el(ar) (((Dar *)((void *)(ar) - sizeof(Dar)))->n)

//----------------------------------------------------------- dar_count_matches
int dar_count_matches(void *ar, int (*fun)(void *)) {
  int kount = 0;
  if (ar) {
    Dar *dar = (Dar *)(ar - sizeof(Dar));
    for (int k = 0; k < dar->n; k++) {
      kount += (*fun)(&ar[k * dar->size]);
      }
    }
  return kount;
  } // dar_count_matches

//------------------------------------------------------------------- dar_crash
void *dar_crash(char *e, char *file, int line) {
  fprintf(stderr, "DAR error \"%s\" (file %s; line %d)\n", e, file, line);
  fflush(stderr);
  abort();
  return NULL;
  } // dar_crash

//----------------------------------------------------------- dar_index_matches
int *dar_index_matches(void *ar, int (*fun)(void *)) {
  int *ind = NULL;
  if (ar) {
    Dar *dar_p = (Dar *)(ar - sizeof(Dar));
    int n_ind = dar_count_matches(ar, fun);
    ind = dar_new(dar_p->stack_p, sizeof((int)0), n_ind);
    int i = 0;
    for (int k = 0; k < dar_p->n; k++) {
      if ((*fun)(&ar[k * dar_p->size])) ind[i++] = k;
      }
    }
  return ind;
  } // dar_index_matches

//-------------------------------------------------------------------- dar_list
void dar_list(Dar *stk) {
  if (!stk) printf("Nothing to list\n");
  while (stk != NULL) {
    printf("%p %zu %zu\n", stk, stk->size, stk->n);
    stk = stk->down;
    }
  } // dar_list

//---------------------------------------------------------------- dar_list_dar
void dar_list_dar(void *ar) {
  if (ar) {
    Dar *dar_p = ar - sizeof(Dar);
    dar_list(*dar_p->stack_p);
    }
  else {
    printf("Nothing to list\n");
    }
  } // dar_list_dar

//--------------------------------------------------------------------- dar_new
void *dar_new(Dar **dar_stack_p, size_t el_size, size_t n_el) {
#if DAR_LOG
  printf("=== dar_new: %p %zu %zu",
      *dar_stack_p, el_size, n_el);
#endif

  // Calculate the size of the needed memory block and allocate it.
  size_t size = sizeof(Dar) + el_size * n_el;
  void *blk = calloc(size, 1);
  if (blk == NULL) {
    char mess[40];
    sprintf(mess, "dar_new: Failed to allocate %zu bytes", size);
    DAR_crash(mess);                                                     // -->
    }

  // Calculate the address to be returned.
  void *array = blk + sizeof(Dar);

  // Push the block address to the dar stack.
  Dar *new_dar = (Dar *)blk;
  new_dar->up = NULL;
  new_dar->down = *dar_stack_p;
  new_dar->size = el_size;
  new_dar->n = n_el;
  new_dar->stack_p = dar_stack_p;
  if (*dar_stack_p != NULL) (*dar_stack_p)->up = new_dar;
  *dar_stack_p = new_dar;

#if DAR_LOG
  printf(" --> %p (%p)\n", *dar_stack_p, array);
#endif
  return array;
  } // dar_new

//----------------------------------------------------------------- dar_new_dar
void *dar_new_dar(void *ar, size_t el_size, size_t n_el) {
#if DAR_LOG
  printf("=== dar_new_dar: %p %zu %zu\n", ar, el_size, n_el);
#endif
  void *array = NULL;
  if (ar) {
    Dar *dar_p = ar - sizeof(Dar);
    array = dar_new(dar_p->stack_p, el_size, n_el);
    }
  return array;
  } // dar_new_dar

//----------------------------------------------------------------- dar_release
void dar_release(void **array_p) {
  void *array = *array_p;
  if (array == NULL) return;                                              //-->
  Dar *dar = (Dar *)(array - sizeof(Dar));
#if DAR_LOG
  printf("=== dar_release: %p %p %zu %zu\n", *dar->stack_p, dar,
      dar->size, dar->n);
#endif

  Dar *up = dar->up;
  Dar *down = dar->down;
  if (up == NULL) { // dar is on top of the stack
    if (down == NULL) {
      *dar->stack_p = NULL; // no other arrays in the stack
      }
    else {
      *dar->stack_p = down;
      down->up = NULL;
      }
    }
  else if (down == NULL) { // dar is at the bottom of the stack
    up->down = NULL;
    }
  else { // dar is in the middle of the stack
    up->down = down;
    down->up = up;
    }
  free(dar);
  *array_p = NULL; // to possibly force a crash if it is used again
  } // dar_release

//------------------------------------------------------------- dar_release_all
void dar_release_all(Dar **stk_p) {
#if STR_LOG
  printf("=== dar_release_all\n");
#endif
  if (stk_p != NULL) {
    while (*stk_p != NULL) {
      Dar *p = *stk_p;
      *stk_p = (*stk_p)->down;
      memset(p, 0, sizeof(Dar) + p->size * p->n);
      free(p);
      }
    }
  } // dar_release_all

//------------------------------------------------------------------ dar_resize
void dar_resize(void **ar1_p, int i1, int n1,
    void **ar2_p, int i2, size_t n2) {
#if DAR_LOG
  printf("=== dar_resize: %p %d %d %d %zu\n", *ar1_p, i1, n1, i2, n2);
#endif
  if (!*ar1_p) {
    char mess[50];
    sprintf(mess, "dar_resize: input array pointer is null");
    DAR_crash(mess);                                                     // -->
    }
  if (i2 < 0 || (n2 > 0 && i2 >= n2)) {
    char mess[50];
    sprintf(mess, "dar_resize: output (%d..%zu) invalid", i2, n2);
    DAR_crash(mess);                                                     // -->
    }
  void *ar1 = *ar1_p;
  Dar *dar1_p = ar1 - sizeof(Dar);
  if (!n1) n1 = dar1_p->n - i1;
  if (i1 < 0 || i1 + n1 - 1 >= dar1_p->n) {
    char mess[50];
    sprintf(mess, "dar_resize: input (%d..%d) invalid", i1, i1 + n1 - 1);
    DAR_crash(mess);                                                     // -->
    }
  if (!n2) n2 = dar1_p->n;
  if (n1 > n2 - i2) {
    char mess[50];
    sprintf(mess, "dar_resize: from %d bytes to %zu", n1, n2 - i2);
    DAR_crash(mess);                                                     // -->
    }
  size_t size = dar1_p->size;
  void *ar2 = dar_new(dar1_p->stack_p, size, n2);
  (void)memcpy(ar2 + i2 * size, ar1 + i1 * size, n1 * size);
  if (ar2_p) {
    *ar2_p = ar2;
    }
  else {
    dar_release(ar1_p);
    *ar1_p = ar2;
    }
  } // dar_resize
