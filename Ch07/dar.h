/* dar.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef DAR
#define DAR
#include <stdlib.h>

#define DAR_LOG 1
#define DAR_DEBUG 1

// Structure to built a stack of allocated buffers.
typedef struct Dar Dar;
struct Dar {
  Dar    *up;   // one level higher in the stack
  Dar    *down; // one level deeper in the stack
  size_t size;  // size of an array element
  size_t n;     // number of elements in the array
  Dar    **stack_p;
  };

/*
 ******************************************** Allocation and release **********
 */
// Macro to report a catastrophic error and abort the program
#define DAR_crash(e) dar_crash(e, __FILE__, __LINE__)

// Macro to be executed first.
#define DAR_setup {       \
  Dar *dar_stack = NULL;

#define DAR_clone(ar1, ar2) dar_resize((void **)&(ar1), 0, 0, \
  (void **)&(ar2), 0, 0);
#define DAR_copy(ar1, i1, n1, ar2, i2, n2)                     \
  dar_resize((void **)&(ar1), i1, n1, (void **)&(ar2), i2, n2)
#define DAR_extend(ar, nn) dar_resize((void **)&(ar), 0, 0, NULL, 0, nn);
#define DAR_get_len(ar) ({                                   \
  void *_ar = ar;                                            \
  size_t _len = 0;                                           \
  if (_ar) _len = ((Dar *)((void *)(_ar) - sizeof(Dar)))->n; \
  _len;                                                      \
  })
#define DAR_list() dar_list(dar_stack)
#define DAR_list_dar(ar) dar_list_dar((void *)(ar))
#define DAR_new(var, ne) dar_new(&dar_stack, sizeof(var), ne)
#define DAR_new_a(ar, ne) dar_new(&dar_stack, sizeof(*(ar)), ne)
#define DAR_new_dar(ar, var, ne) dar_new_dar((void *)(ar), sizeof(var), ne)
#define DAR_release(array) dar_release((void **)&(array))
#define DAR_release_all() dar_release_all(&(dar_stack))
#define DAR_resize(ar, i1, n1, i2, n2)             \
  dar_resize((void **)&(ar), i1, n1, NULL, i2, n2)

// Macro to be executed last
#define DAR_dismantle           \
  dar_release_all(&dar_stack);  \
  }

int dar_count_matches(void *ar, int (*fun)(void *));
void *dar_crash(char *e, char *file, int line);
int *dar_index_matches(void *ar, int (*fun)(void *));
void dar_list(Dar *dar_stack);
void dar_list_dar(void *ar);
void *dar_new(Dar **dar_stack_p, size_t el_size, size_t n_el);
void *dar_new_dar(void *ar, size_t el_size, size_t n_el);
void dar_release(void **array_p);
void dar_release_all(Dar **dar_stack_p);
void dar_resize(void **ar1_p, int i1, int n1, void **ar2_p, int i2, size_t n2);

#endif
