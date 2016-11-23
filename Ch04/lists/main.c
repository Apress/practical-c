/* main.c
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

//typedef struct item_t { int value; } item_t;

typedef struct item_t item_t;
struct item_t {
  item_t *next;
  int    value;
  };

void print_value(void *item);

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {

  //---------- Array-based stacks.
  printf("=== Array-based stacks\n");

  // Allocate the stack
	item_t **stack_a = NULL;
	LST_new_stack_a(stack_a, 7);
  LST_list_a(stack_a);

  printf("\n=== Reducing stack capacity to 3\n");
	LST_set_capacity_a(stack_a, 3);
  LST_list_a(stack_a);

	// Push two items onto the stack.
  item_t fi = { NULL, 111 };
  printf("pushed %d: %s\n", fi.value,
      (LST_push_a(stack_a, fi)) ? "ok": "stack full");
  item_t se = { NULL, 222 };
  printf("pushed %d: %s\n", se.value,
      (LST_push_a(stack_a, se)) ? "ok": "stack full");
  LST_list_a(stack_a);

  printf("\n=== Listing the stack with a call-back function\n");
  LST_list_a_plus(stack_a, print_value);
  printf("The stack is%s empty!\n", ((LST_is_empty_a(stack_a)) ? "" : " not"));
  printf("The stack is%s full!\n", ((LST_is_full_a(stack_a)) ? "" : " not"));

  // Pop them.
  item_t *it = NULL;
  do {
    it = LST_pop_a(stack_a);
    printf("popped %p", it);
    if (it) printf(": %d\n", it->value);
    else printf("\n");
    } while (it);
  printf("The stack is%s empty!\n", ((LST_is_empty_a(stack_a)) ? "" : " not"));

  // Fill up the stack to capacity.
  printf("\n=== Filling up the stack to capacity\n");
  for (int j = 0; j < LST_get_capacity_a(stack_a) + 1; j++) {
    printf("pushed %d: %s\n", fi.value,
        (LST_push_a(stack_a, fi)) ? "ok": "stack full");
    }
  printf("The stack is%s full!\n", ((LST_is_full_a(stack_a)) ? "" : " not"));
  LST_list_a(stack_a);

  // Change the stack maximum size.
  printf("\n=== Attempting to resize the stack with too small capacity\n");
  printf("Setting capacity to 2 %s\n",
      (LST_set_capacity_a(stack_a, 2)) ? "succeeded" : "failed");
  printf("Setting capacity to 4 %s\n",
      (LST_set_capacity_a(stack_a, 4)) ? "succeeded" : "failed");

  // Create a second stack.
  printf("\n=== Create a second stack\n");
  item_t **stack_b = NULL;
  LST_new_stack_a(stack_b, 2);
  printf("pushed %d to the second stack: %s\n", se.value,
      (LST_push_a(stack_b, se)) ? "ok": "stack full");
  printf("pushed %d to the second stack: %s\n", se.value,
      (LST_push_a(stack_b, se)) ? "ok": "stack full");

  // List both stacks.
  printf("stack_a: ");
  LST_list_a_plus(stack_a, print_value);
  printf("stack_b: ");
  LST_list_a_plus(stack_b, print_value);

	// Release the stacks.
  LST_free_stack_a(stack_a);
  LST_free_stack_a(stack_b);
  printf("\n\n");

  //---------- Linked-list stacks.
  printf("=== Linked-list stacks\n");
  item_t *stack = NULL;
  item_t item_1 = { NULL, 100 };
  LST_push(stack, item_1);
  item_t item_2 = { NULL, 200 };
  LST_push(stack, item_2);
  printf("Pushed two items\n");
  LST_list_stack(stack);

  printf("\nList stack with call-back\n");
  LST_list_stack_plus(stack, print_value);
  item_t *item = LST_top(stack);
  if (item) printf("Top of the stack: %d\n", LST_top(stack)->value);
  else printf("The stack is empty\n");
  printf("Stack size: %d\n", LST_stack_size(stack));

  printf("\nPopping %d\n", LST_pop(stack)->value);
  printf("Stack size: %d\n", LST_stack_size(stack));
  LST_list_stack_plus(stack, print_value);

  printf("\nPopping %d\n", LST_pop(stack)->value);
  printf("Stack size: %d\n", LST_stack_size(stack));
  LST_list_stack_plus(stack, print_value);
  item = LST_top(stack);
  if (item) printf("Top of the stack: %d\n", LST_top(stack)->value);
  else printf("Top of the stack: stack empty\n");

  //---------- Array-based queues.
  printf("\n\n=== Array-based queues\n");

  // Allocate the queue
  item_t **q_a = NULL;
  LST_new_queue_a(q_a, 3);
  printf("capacity: %d\n", LST_get_q_capacity_a(q_a));
  printf("size: %d\n", LST_get_q_size_a(q_a));
  printf("queue is%s empty\n", LST_q_is_empty_a(q_a) ? "" : " not");

  item_t it1 = { NULL, 1 };
  printf("\nenqueuing it1 %sdone\n", (LST_enqueue_a(q_a, it1)) ? "" : "not ");
  printf("size: %d\n", LST_get_q_size_a(q_a));
  printf("queue is%s full\n", LST_q_is_full_a(q_a) ? "" : " not");

  item_t it2 = { NULL, 2 };
  printf("\nenqueuing it2 %sdone\n", (LST_enqueue_a(q_a, it2)) ? "" : "not ");
  printf("size: %d\n", LST_get_q_size_a(q_a));
  printf("queue is%s full\n", LST_q_is_full_a(q_a) ? "" : " not");

  item_t it3 = { NULL, 3 };
  printf("\nenqueuing it3 %sdone\n", (LST_enqueue_a(q_a, it3)) ? "" : "not ");
  printf("size: %d\n", LST_get_q_size_a(q_a));
  printf("queue is%s full\n", LST_q_is_full_a(q_a) ? "" : " not");

  item_t it4 = { NULL, 4 };
  printf("\nenqueuing it4 %sdone\n", (LST_enqueue_a(q_a, it4)) ? "" : "not ");

  printf("\nLST_list_qa_plus: ");
  LST_list_qa_plus(q_a, print_value);

  item_t *it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  printf("\nenqueuing it4 %sdone\n", (LST_enqueue_a(q_a, it4)) ? "" : "not ");
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  item_t it5 = { NULL, 5 };
  printf("\nenqueuing it5 %sdone\n", (LST_enqueue_a(q_a, it5)) ? "" : "not ");
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  printf("\nenqueuing it1 %sdone\n", (LST_enqueue_a(q_a, it1)) ? "" : "not ");
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  printf("\nenqueuing it2 %sdone\n", (LST_enqueue_a(q_a, it2)) ? "" : "not ");
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  printf("\nenqueuing it3 %sdone\n", (LST_enqueue_a(q_a, it3)) ? "" : "not ");
  LST_list_qa_plus(q_a, print_value);

  printf("\nmaking a new queue of a larger size %s\n",
      (LST_set_q_capacity_a(q_a, 4)) ? "succeeded" : "failed");
  LST_list_qa_plus(q_a, print_value);
  //
  printf("\nmaking a new queue of the original size %s\n",
      (LST_set_q_capacity_a(q_a, 3)) ? "succeeded" : "failed");
  LST_list_qa_plus(q_a, print_value);
  //
  printf("\nmaking a new queue of a smaller size %s\n",
      (LST_set_q_capacity_a(q_a, 2)) ? "succeeded" : "failed");
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_first_a(q_a);
  printf("first item %p %d\n", it_p, (it_p) ? it_p->value : -1);

  printf("\nmaking a new queue of a size 1 %s\n",
      (LST_set_q_capacity_a(q_a, 1)) ? "succeeded" : "failed");
  LST_list_qa_plus(q_a, print_value);
  //
  printf("\nmaking a new queue of size 0 %s\n",
      (LST_set_q_capacity_a(q_a, 0)) ? "succeeded" : "failed");
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);
  LST_list_qa_plus(q_a, print_value);

  it_p = LST_dequeue_a(q_a);
  printf("\ndequeued item %p %d\n", it_p, (it_p) ? it_p->value : -1);

//  A useful statement that prints the two queue indices.
//  printf("%d %d\n", *((int *)q_a - 3), *((int *)q_a - 1));

  LST_free_queue_a(q_a);

  //---------- Linked-list queues.
  printf("\n\n=== Linked-list queues\n");
  lst_queue_t queue = {};

  item_t first = { NULL, 101 };
  item_t second = { NULL, 202 };
  item_t third = { NULL, 303 };
  printf("\nenqueuing first item\n");
  LST_enqueue(queue, first);
  printf("enqueuing second item\n");
  LST_enqueue(queue, second);
  printf("enqueuing third item\n");
  LST_enqueue(queue, third);
  LST_list_queue_plus(queue, print_value);
  printf("queue size: %d\n", LST_queue_size(queue));

  item_t *it_ptr = LST_dequeue(queue);
  printf("\ndequeued item %p %d\n", it_ptr, (it_ptr) ? it_ptr->value : -1);

  it_ptr = LST_dequeue(queue);
  printf("\ndequeued item %p %d\n", it_ptr, (it_ptr) ? it_ptr->value : -1);
  item_t fourth = { NULL, 404 };
  printf("enqueuing fourth item\n");
  LST_enqueue(queue, fourth);
  LST_list_queue_plus(queue, print_value);

  it_ptr = LST_dequeue(queue);
  printf("\ndequeued item %p %d\n", it_ptr, (it_ptr) ? it_ptr->value : -1);
  it_ptr = LST_first(queue);
  printf("first item: %p %d\n", it_ptr, it_ptr->value);

  it_ptr = LST_dequeue(queue);
  printf("\ndequeued item %p %d\n", it_ptr, (it_ptr) ? it_ptr->value : -1);
  it_ptr = LST_dequeue(queue);
  printf("\ndequeued item %p %d\n", it_ptr, (it_ptr) ? it_ptr->value : -1);
  printf("queue size: %d\n", LST_queue_size(queue));
  LST_list_queue_plus(queue, print_value);

	return EXIT_SUCCESS;
  } // main

//----------------------------------------------------------------- print_value
void print_value(void *item) {
  printf("%5d", ((item_t *)item)->value);
  } // print_value
