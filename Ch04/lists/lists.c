/* lists.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include "lists.h"

#define Set_size(st, val) {*((int *)(st) - 1) = val;}

//-------------------------------------------------------------- lst_do_nothing
void lst_do_nothing(void *nada) {
  } // lst_do_nothing

//--------------------------------------------------------------- lst_enqueue_a
int lst_enqueue_a(void **queue, void *item) {
  if (queue == NULL) {
    fprintf(stderr, "lst_enqueue_a: queue pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  int done = 0;
  if (!LST_q_is_full_a(queue)) {
    int *i_enq_p = (int *)queue - 3;
    int i_enq = *i_enq_p;
    queue[i_enq++] = item;
    if (i_enq > LST_get_q_capacity_a(queue)) i_enq = 0; // >, not >=
    *i_enq_p = i_enq;
    done = 1;
    }
  return done;
  } // lst_enqueue_a

//----------------------------------------------------------------- lst_first_a
void *lst_first_a(void **queue, int remove) {
  if (queue == NULL) {
    fprintf(stderr, "lst_first_a: queue pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  void *item = NULL;
  if (!LST_q_is_empty_a(queue)) {
    int *i_deq_p = (int *)queue - 1;
    int i_deq = *i_deq_p;
    item = queue[i_deq];
    if (remove) {
      queue[i_deq++] = NULL;
      if (i_deq > LST_get_q_capacity_a(queue)) i_deq = 0;
      *i_deq_p = i_deq;
      }
    }
  return item;
  } // lst_first_a

//------------------------------------------------------------------ lst_list_a
void lst_list_a(void **ptr, void (*fun)(void *)) {
  if (ptr == NULL) {
    fprintf(stderr, "lst_list_a: stack pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  int capacity = LST_get_capacity_a(ptr);
  int size = LST_get_size_a(ptr);
  printf("capacity=%d size=%d\n", capacity, size);
  for (int k = 0; k < capacity; k++) {
    printf("%d: %p", k, &ptr[k]);
    if (k < size) {
      printf(" %p", ptr[k]);
      if (fun) (*fun)(ptr[k]);
      }
    printf("\n");
    }
  } // lst_list_a

//----------------------------------------------------------------- lst_list_qa
void lst_list_qa(void **q, void (*fun)(void *)) {
  if (q == NULL) {
    fprintf(stderr, "lst_list_qa: queue pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  int *i_p = (int *)q;
  int i_deq = *--i_p;
  int capacity = *--i_p;
  int i_enq = *--i_p;
  int size = i_enq - i_deq;
  if (size < 0) size += capacity + 1;
  printf("capacity=%d size=%d\n", capacity, size);
  for (int k = 0; k < capacity + 1; k++) {
    printf("%d: %p", k, &q[k]);
    if (q[k]) {
      printf(" %p", q[k]);
      if (fun) (*fun)(q[k]);
      }
    printf("\n");
    }
  } // lst_list_qa

//-------------------------------------------------------------- lst_list_queue
void lst_list_queue(void **ptr, void (*fun)(void *)) {
  if (ptr == NULL) {
    fprintf(stderr, "lst_list_a: stack pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  int capacity = LST_get_capacity_a(ptr);
  int size = LST_get_size_a(ptr);
  printf("capacity=%d size=%d\n", capacity, size);
  for (int k = 0; k < capacity; k++) {
    printf("%d: %p", k, &ptr[k]);
    if (k < size) {
      printf(" %p", ptr[k]);
      if (fun) (*fun)(ptr[k]);
      }
    printf("\n");
    }
  } // lst_list_queue

//------------------------------------------------------------- lst_new_queue_a
// Together with the queue, allocate space to store its capacity (plus 1
// to distinguish between full and empty) and its enqueue and dequeue indices.
void lst_new_queue_a(void ***queue_ptr, int n) {
  if (n <= 0) {
    fprintf(stderr, "lst_new_queue_a: %d elements?\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }
  int *ptr = malloc((n + 1) * sizeof(void *) + 3 * sizeof(int));
  if (ptr == NULL) {
    fprintf(stderr, "lst_new_queue_a: failed to allocate %d-queue\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }
  int *i_enq_p = ptr++;
  *i_enq_p = 0;
  int *capacity_p = ptr++;
  *capacity_p = n;
  int *i_deq_p = ptr++;
  *i_deq_p = 0;
  void **queue = (void **)ptr;
  for (int k = 0; k <= n; k++) queue[k] = NULL; // <=, not <
  *queue_ptr = queue;
  } // lst_new_queue_a

//------------------------------------------------------------- lst_new_stack_a
// Together with the stack, allocate space to store its maximum
// size and its size (i.e., the number of items it currently contains).
void lst_new_stack_a(void ***stack_ptr, int n) {
  if (n <= 0) {
    fprintf(stderr, "lst_new_stack_a: %d elements?\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }
  int *ptr = malloc(n * sizeof(void *) + 2 * sizeof(int));
  if (ptr == NULL) {
    fprintf(stderr, "lst_new_stack_a: failed to allocate %d-stack\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }
  int *capacity = ptr++;
  *capacity = n;
  int *size = ptr++;
  *size = 0;
  void **stack = (void **)ptr;
  for (int k = 0; k < n; k++) stack[k] = NULL;
  *stack_ptr = stack;
  } // lst_new_stack_a

//------------------------------------------------------------------ lst_push_a
int lst_push_a(void **stack, void *item) {
  if (stack == NULL) {
    fprintf(stderr, "lst_push_a: stack pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  int capacity = LST_get_capacity_a(stack);
  int size = LST_get_size_a(stack);
  int done = 0;
  if (size < capacity) {
    stack[size++] = item;
    Set_size(stack, size);
    done = 1;
    }
  return done;
  } // lst_push_a

//---------------------------------------------------------- lst_set_capacity_a
int lst_set_capacity_a(void ***stack_ptr, int n) {
  int possible = 0;
  if (n > 0) {
    void **stack = *stack_ptr;
    if (stack == NULL) {
      fprintf(stderr, "lst_set_capacity: stack pointer is NULL\n");
      fflush(stderr);
      abort();                                                           // -->
      }
    int size = LST_get_size_a(stack);
    possible = size <= n;
    if (possible) {
      void **new_stack = NULL;
      lst_new_stack_a(&new_stack, n);
      Set_size(new_stack, size);
      if (size > 0) {
        for (int k = 0; k < size && stack[k]; k++) new_stack[k] = stack[k];
        }
      free((int *)*stack_ptr - 2);
      *stack_ptr = new_stack;
      }
    }
  return possible;
  } // lst_set_capacity_a

//-------------------------------------------------------- lst_set_q_capacity_a
// The function also fills up the new queue from element 0.
int lst_set_q_capacity_a(void ***q_ptr, int n) {
  int possible = 0;
  if (n > 0) {
    void **q = *q_ptr;
    if (q == NULL) {
      fprintf(stderr, "lst_set_q_capacity: queue pointer is NULL\n");
      fflush(stderr);
      abort();                                                           // -->
      }
    int *i_p = (int *)q;
    int *i_deq_p = --i_p;
    int capacity = *--i_p;
    int *i_enq_p = --i_p;
    int size = *i_enq_p - *i_deq_p;
    if (size < 0) size += capacity + 1;
    possible = size <= n;
    if (possible) {
      void **new_q = NULL;
      lst_new_queue_a(&new_q, n);
      if (size) {
        for (int k = 0, k_orig = *i_deq_p; k < size; k++) {
          new_q[k] = q[k_orig++];
          if (k_orig > capacity) k_orig = 0;
          }
        i_p = (int *)new_q;
        *(i_p - 3) = size;
        *(i_p - 1) = 0;
        }
      free((int *)*q_ptr - 3);
      *q_ptr = new_q;
      }
    }
  return possible;
  } // lst_set_q_capacity_a

//------------------------------------------------------------------- lst_top_a
void *lst_top_a(void **stack, int remove) {
  if (stack == NULL) {
    fprintf(stderr, "lst_pop_a: stack pointer is NULL\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  int size = LST_get_size_a(stack);
  void *item = NULL;
  if (size > 0) {
    size--;
    item = stack[size];
    if (remove) {
      stack[size] = NULL;
      Set_size(stack, size);
      }
    }
  return item;
  } // lst_top_a
