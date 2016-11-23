/* lists.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef LISTS
#define LISTS

//------- Array-based stacks
#define LST_free_stack_a(ptr) {   \
    int **_ptr = (int **)&(ptr);  \
    if (*_ptr != NULL) {          \
      free(*_ptr - 2);            \
      *_ptr = NULL;               \
      }                           \
    }
#define LST_get_capacity_a(st) (*((int *)(st) - 2))
#define LST_get_size_a(st) (*((int *)(st) - 1))
#define LST_is_empty_a(st) (LST_get_size_a(st) == 0)
#define LST_is_full_a(st) ({                         \
    int *_st = (int *)st;                            \
    LST_get_capacity_a(_st) == LST_get_size_a(_st);  \
    })
#define LST_list_a(st) lst_list_a((void **)(st), NULL)
#define LST_list_a_plus(st, fun) lst_list_a((void **)(st), fun)
#define LST_new_stack_a(stack, n) lst_new_stack_a((void ***)&(stack), n)
#define LST_pop_a(stack) lst_top_a((void **)(stack), 1)
#define LST_push_a(stack, item) lst_push_a((void **)(stack), (void *)&(item))
#define LST_set_capacity_a(stack, n) lst_set_capacity_a((void ***)&(stack), n)
#define LST_top_a(stack) lst_top_a((void **)(stack), 0)

void lst_list_a(void **ptr, void (*fun)(void *));
void lst_new_stack_a(void ***stack_ptr, int n);
int  lst_push_a(void **stack, void *item);
int  lst_set_capacity_a(void ***stack_ptr, int n);
void *lst_top_a(void **stack, int remove);

//------- Linked-list stacks
#define LST_is_empty(sta) (LST_stack_size(sta) == 0)
#define LST_list_stack(st) LST_list_stack_plus(st, lst_do_nothing)
#define LST_list_stack_plus(st, fun) {  \
    item_t *_next = st;                 \
    int k = 0;                          \
    if (_next) {                        \
      while (_next) {                   \
        printf("%3d: %p", k++, _next);  \
        fun(_next);                     \
        printf("\n");                   \
        _next = _next->next;            \
        }                               \
      }                                 \
    else {                              \
      printf("Stack empty\n");          \
      }                                 \
    }
#define LST_pop(st) ({                       \
    item_t **_stack_p = &(st);               \
    item_t *_item_p = *_stack_p;             \
    if (_item_p) *_stack_p = _item_p->next;  \
    _item_p;                                 \
    })
#define LST_push(st, it) {                     \
    item_t **_stack_p = &(st);                 \
    item_t *_item_p = &(it);                   \
    if (*_stack_p) _item_p->next = *_stack_p;  \
    else _item_p->next = NULL;                 \
    *_stack_p = _item_p;                       \
    }
#define LST_stack_size(st) ({  \
    item_t *_item_p = st;      \
    int _k = 0;                \
    while (_item_p) {          \
      _k++;                    \
      _item_p = _item_p->next; \
      }                        \
    _k;                        \
    })
#define LST_top(st) (st)

/*
//------- Alternative formulation of linked-list stack macros.
// Because of typeof(), replace the gcc option -std=c99 with -std=gnu99.
#define LST_is_empty(sta) (LST_stack_size(sta) == 0)
#define LST_list_stack(st) LST_list_stack_plus(st, lst_do_nothing)
#define LST_list_stack_plus(st, fun) {   \
    typeof(st) _next = st;               \
    int k = 0;                           \
    if (_next) {                         \
      while (_next) {                    \
        printf("%3d: %p", k++, _next);   \
        fun(_next);                      \
        printf("\n");                    \
        _next = _next->next;             \
        }                                \
      }                                  \
    else {                               \
      printf("Stack empty\n");           \
      }                                  \
    }
#define LST_pop(st) ({                       \
    typeof(st) *_stack_p = &(st);            \
    typeof(st) _item_p = *_stack_p;          \
    if (_item_p) *_stack_p = _item_p->next;  \
    _item_p;                                 \
    })
#define LST_push(st, it) {                    \
    typeof(it) **_stack_p = &(st);            \
    typeof(it)  *_item_p = &(it);             \
    if (*_stack_p) _item_p->next = *_stack_p; \
    else _item_p->next = NULL;                \
    *_stack_p = _item_p;                      \
    }
#define LST_stack_size(st) ({  \
    typeof(st) _item_p = st;   \
    int _k = 0;                \
    while (_item_p) {          \
      _k++;                    \
      _item_p = _item_p->next; \
      }                        \
    _k;                        \
    })
#define LST_top(st) (st)
*/
/*
//------- Yet another formulation of the linked-list macros.
// It relies on matching lst_item_t with the item structure definition of the
// calling program. As it uses typeof(), define the gcc option -std=gnu99
typedef struct lst_item_t lst_item_t;
struct lst_item_t {
  lst_item_t *nxt;
  };

#define LST_is_empty(sta) (LST_stack_size(sta) == 0)
#define LST_list_stack(st) LST_list_stack_plus(st, lst_do_nothing)
#define LST_list_stack_plus(st, fun) {       \
    lst_item_t *_next = (lst_item_t *)(st);  \
    int k = 0;                               \
    if (_next) {                             \
      while (_next) {                        \
        printf("%3d: %p", k++, _next);       \
        fun(_next);                          \
        printf("\n");                        \
        _next = _next->nxt;                  \
        }                                    \
      }                                      \
    else {                                   \
      printf("Stack empty\n");               \
      }                                      \
    }
#define LST_pop(st) ({                             \
    lst_item_t **_stack_p = (lst_item_t **)&(st);  \
    lst_item_t *_item_p = *_stack_p;               \
    if (_item_p) *_stack_p = _item_p->nxt;         \
    (typeof(st))_item_p;                           \
    })
#define LST_push(st, it) {                         \
    lst_item_t **_stack_p = (lst_item_t **)&(st);  \
    lst_item_t *_item_p = (lst_item_t *)&(it);     \
    if (*_stack_p) _item_p->nxt = *_stack_p;       \
    else _item_p->nxt = NULL;                      \
    *_stack_p = _item_p;                           \
    }
#define LST_stack_size(st) ({                  \
    lst_item_t *_item_p = (lst_item_t *)(st);  \
    int _k = 0;                                \
    while (_item_p) {                          \
      _k++;                                    \
      _item_p = _item_p->nxt;                  \
      }                                        \
    _k;                                        \
    })
#define LST_top(st) (st)
*/

//------- Array-based queues
#define LST_dequeue_a(qq) lst_first_a((void **)(qq), 1)
#define LST_enqueue_a(qq, item) lst_enqueue_a((void **)(qq), (void *)&(item))
#define LST_first_a(qq) lst_first_a((void **)(qq), 0)
#define LST_free_queue_a(ptr) {   \
    int **_ptr = (int **)&(ptr);  \
    if (*_ptr != NULL) {          \
      free(*_ptr - 3);            \
      *_ptr = NULL;               \
      }                           \
    }
#define LST_get_q_capacity_a(q) (*((int *)(q) - 2))
#define LST_get_q_size_a(q) ({                    \
    int *_q = (int *)(q);                         \
    int _i_deq = 0;                               \
    int _i_enq = 0;                               \
    if (_q) {                                     \
      _i_deq = (*--_q);                           \
      int _i_cap = (*--_q);                       \
      _i_enq = (*--_q);                           \
      if (_i_enq < _i_deq) _i_enq += _i_cap + 1;  \
      }                                           \
    _i_enq - _i_deq;                              \
    })
#define LST_list_qa(qq) lst_list_qa((void **)(qq), NULL)
#define LST_list_qa_plus(qq, fun) lst_list_qa((void **)(qq), fun)
#define LST_new_queue_a(qq, n) lst_new_queue_a((void ***)&(qq), n)
#define LST_q_is_empty_a(q) ({  \
    int *_q = (int *)(q);       \
    *(_q - 3) == *(_q - 1);     \
    })
#define LST_q_is_full_a(q) ({                            \
    int *_qq = (int *)q;                                 \
    LST_get_q_size_a(_qq) >= LST_get_q_capacity_a(_qq);  \
    })
#define LST_set_q_capacity_a(qq, n) lst_set_q_capacity_a((void ***)&(qq), n)

void lst_list_qa(void **ptr, void (*fun)(void *));
void lst_new_queue_a(void ***queue_ptr, int n);
int  lst_enqueue_a(void **queue, void *item);
int  lst_set_q_capacity_a(void ***queue_ptr, int n);
void *lst_first_a(void **queue, int remove);

//------- Linked-list queues
typedef struct lst_queue_t {
  void *first;
  void *last;
  } lst_queue_t;

#define LST_dequeue(q) ({                   \
    lst_queue_t *_q_p = &(q);               \
    item_t *_it_p = _q_p->first;            \
    if (_it_p) {                            \
      _q_p->first = _it_p->next;            \
      if (_it_p->next) _it_p->next = NULL;  \
      else _q_p->last = NULL;               \
      }                                     \
    _it_p;                                  \
    })
#define LST_enqueue(q, it) {                               \
    lst_queue_t *_q_p = &(q);                              \
    item_t *_it_p = &(it);                                 \
    if (_q_p->last) ((item_t *)_q_p->last)->next = _it_p;  \
    else _q_p->first = _it_p;                              \
    _q_p->last = _it_p;                                    \
    _it_p->next = NULL;                                    \
    }
#define LST_first(q) ((q).first)
#define LST_list_queue(q) LST_list_queue_plus(q, lst_do_nothing)
#define LST_list_queue_plus(q, fun) {   \
    item_t *_next = (q).first;          \
    int k = 0;                          \
    if (_next) {                        \
      while (_next) {                   \
        printf("%3d: %p", k++, _next);  \
        fun(_next);                     \
        printf("\n");                   \
        _next = _next->next;            \
        }                               \
      }                                 \
    else {                              \
      printf("Queue empty\n");          \
      }                                 \
    }
#define LST_q_is_empty(q) (LST_queue_size(q) == 0)
#define LST_queue_size(q) ({    \
    item_t *_it_p = (q).first;  \
    int _k = 0;                 \
    while (_it_p) {             \
      _k++;                     \
      _it_p = _it_p->next;      \
      }                         \
    _k;                         \
    })

//------- Generic
void lst_do_nothing(void *);

#endif
