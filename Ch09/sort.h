/* sort.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef SORT
#define SORT
#include <math.h>

#define SORT_DEBUG 0
#define SORT_LOG   0

#define Sort_list(ar) {                                  \
    int **a = (int **)ar;                                \
    int n = ((int *)a)[-1];                              \
    char fmt[5];                                         \
    sprintf(fmt, "%%%dd", (int)floor(log10(n - 1)) + 2); \
    printf("%5d:", n);                                   \
    for (int k = 0; k < n; k++) printf(fmt, k);          \
    printf("\n      ");                                  \
    for (int k = 0; k < n; k++) printf(fmt, *a[k]);      \
    printf("\n");                                        \
    }

#define Sort_list_int(a) {                               \
    int *aa = a;                                         \
    int n = ((int *)a)[-1];                              \
    char fmt[5];                                         \
    sprintf(fmt, "%%%dd", (int)floor(log10(n - 1)) + 2); \
    printf("%5d:", n);                                   \
    for (int k = 0; k < n; k++) printf(fmt, k);          \
    printf("\n      ");                                  \
    for (int k = 0; k < n; k++) printf(fmt, aa[k]);      \
    printf("\n");                                        \
    }

void sort_bubble(void **ar, int (*cmp)(void *, void *));
void sort_insertion(void **ar, int (*cmp)(void *, void *));
void sort_quick(void **ar, int (*cmp)(void *, void *));
void sort_quick_int(int *a);
void sort_shell(void **ar, int (*cmp)(void *, void *));
void sort_shell_step(void **ar, int (*cmp)(void *, void *), int dist);
#endif
