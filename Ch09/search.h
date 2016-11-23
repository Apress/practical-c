/* search.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef SRCH
#define SRCH

#define SRCH_DEBUG 1

int srch_int_add(int num, int **ar_p, int incr);
int *srch_int_alloc(int n, int *a);
int srch_int_del(int k, int *ar);
int srch_int_find(int num, int *ar);
void srch_int_free(int **ar_p);

int srch_lin_int(int num, int *ar, int n);

int srch_ptr_add(void *obj, void ***ar_p, int incr);
void **srch_ptr_alloc(int n, void *a);
int srch_ptr_del(int k, void **ar);
int srch_ptr_del_ord(int k, void **ar);
int srch_ptr_find(void *obj, void **ar, int (*cmp)(void *, void *));
int srch_ptr_find_ord(void *obj, void **ar, int (*cmp)(void *, void *));
void srch_ptr_free(void ***ar_p);
int srch_ptr_ins_ord(void *obj, void ***ar_p, int (*cmp)(void *, void *),
    int incr);
#endif
