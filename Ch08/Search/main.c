/* Search: main.c
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
#include "search_tree.h"

void arrays(void);
void binary_trees(void);

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
//  arrays();
//  binary_trees();

  // Balancing binary trees.
  int n_nodes = 31;
  srcht_tree tree = {};
  printf("Inserting %d nodes in sequence.\n", n_nodes);
  for (int k = 0; k < n_nodes; k++) {
    if (!srcht_ins(k, &tree)) printf("Failed to insert node %d\n", k);
    }
  if (n_nodes <= 20) srcht_list_tree(&tree);
  printf("Balancing %ssuccessful\n", (srcht_balance_tree(&tree)) ? "" : "un");
  srcht_list_tree(&tree);
  int n = srcht_free_tree(&tree);
  printf("\nFreed %d nodes from the tree\n", n);

	return EXIT_SUCCESS;
  }

//---------------------------------------------------------------------- arrays
void arrays(void) {
/*
  { // Linear search of an unordered array.
    int ar[] = {7, -3, 0, 25, -2, 6};
    int n = sizeof(ar) / sizeof(int);
    for (int k = 0; k < n; k++) {
      printf("%2d%3d\n", k, srch_lin_int(ar[k], ar, n - 1));
      }
    } // Linear search..
*/

/*
  { // Dynamic arrays of integers.

  #define Elements(ar) {                               \
      int *a = ar;                                     \
      int n = a[-1];                                   \
      printf("%2d:", n);                               \
      for (int k = 0; k < n; k++) printf("%3d", a[k]); \
      printf("\n");                                    \
      }

    // Allocate an array with initialization and then release it.
    int a_init[] = {7, -3, 0, 25, -2, 6};
    int *arr = srch_int_alloc(sizeof(a_init) / sizeof(int), a_init);
    Elements(arr);
    printf("\n");
    srch_int_free(&arr);

    // Add elements with automatic array extension.
    int *ar = srch_int_alloc(5, NULL);
    int n = 0;
    for (int k = 0; k < 13; k++) {
      n = srch_int_add(k, &ar, k % 5);
      printf("%d: n=%d\n", k, n);
      }
    printf("\n");

    // Search elements at random and then delete them.
    srand(10002);
    Elements(ar);
    int n_left = n;
    int n_search = 0;
    while (n_left > 0) {
      int num = (rand() % (n + 2)) - 1;
      n_search++;
      int res = srch_int_find(num, ar);
      if (res >= 0) {
        n_left = srch_int_del(res, ar);
        printf("%d: %d removed\n", n_search, num);
        Elements(ar);
        }
      else {
  //      printf("%d: %d not found\n", n_search, num);
        }
      }
    srch_int_free(&ar);
    printf("%p\n", ar);
    } // Dynamic arrays of integers..
*/


  { // Dynamic arrays of pointers.

  #define Elements(ar) {                                \
      int **a = (int **)ar;                             \
      int n = ((int *)a)[-1];                           \
      printf("%2d:", n);                                \
      for (int k = 0; k < n; k++) printf("%3d", *a[k]); \
      printf("\n");                                     \
      }

    int cmp_i(void *el_p, void *val_p) {
      int el = *(int *)el_p;
      int val = *(int *)val_p;
      return (val == el) ? 0 : (val > el) ? 1 : -1;
      }

/*
    { // Allocate an array with initialization and then release it.
      int i_init[] = {7, -3, 0, 25, -2, 6};
      int n_i = sizeof(i_init) / sizeof(int);
      void *a_init[n_i];
      for (int k = 0; k < n_i; k++) a_init[k] = &i_init[k];
      void **arr = srch_ptr_alloc(sizeof(a_init) / sizeof(void *), a_init);
      Elements(arr);
      printf("\n");
      srch_ptr_free(&arr);
      } // Allocate an array..
*/

/*
    { // Add elements with automatic array extension and then delete them.
      int i_init[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
      int n_i = sizeof(i_init) / sizeof(int);
      void **ar = srch_ptr_alloc(5, NULL);
      int n = 0;
      for (int k = 0; k < n_i; k++) {
        n = srch_ptr_add(&i_init[k], &ar, k % 5);
        printf("%d: n=%d\n", k, n);
        }
      printf("\n");

      // Search elements at random and delete them.
      srand(10002);
      Elements(ar);
      int n_left = n;
      int n_search = 0;
      while (n_left > 0) {
        int num = (rand() % (n + 2)) - 1;
        n_search++;
        int res = srch_ptr_find(&i_init[num], ar, &cmp_i);
        if (res >= 0) {
          n_left = srch_ptr_del(res, ar);
          printf("%d: %d removed\n", n_search, num);
          Elements(ar);
          }
        else {
    //      printf("%d: %d not found\n", n_search, num);
          }
        }

      srch_ptr_free(&ar);
      printf("%p\n", ar);
      } // Add elements..
*/

    { // Binary search.
      int i_init[] = {1, 3, 5, 7, 9};
      int n_i = sizeof(i_init) / sizeof(int);
      void **ar = srch_ptr_alloc(n_i, NULL);
      for (int k = 0; k < n_i; k++) srch_ptr_add(&i_init[k], &ar, 5);
      Elements(ar);
      int k_max = i_init[n_i - 1] + 1;
      printf("Binary search\n");
      for (int k = 0; k <= k_max; k++) {
        int i = srch_ptr_find_ord(&k, ar, &cmp_i);
        if (i < 0) {
          i = -i - 1;
          if (i >= n_i) printf("%2d: %2d (past the last element)\n", k, i);
          else printf("%2d: %2d (currently holding %d)\n", k, i, *(int *)ar[i]);
          }
        else {
          printf("%2d: %2d (%d found)\n", k, i, *(int *)ar[i]);
          }
        }
      srch_ptr_free(&ar);
      } // Binary search..

    { // Search elements at random and delete them.
      printf("\n");
      int i_init[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
      int n_i = sizeof(i_init) / sizeof(int);
      void **ar = srch_ptr_alloc(n_i, NULL);
      for (int k = 0; k < n_i; k++) srch_ptr_add(&i_init[k], &ar, 5);

      srand(12345);
      Elements(ar);
      int n_left = n_i;
      int n_search = 0;
      while (n_left > 0) {
        int num = (rand() % (n_i + 2)) - 1;
        n_search++;
        int res = srch_ptr_find_ord(&i_init[num], ar, &cmp_i);
        if (res >= 0) {
          n_left = srch_ptr_del_ord(res, ar);
          printf("%d: %d removed\n", n_search, num);
          Elements(ar);
          }
        }

      srch_ptr_free(&ar);
      printf("%p\n", ar);
      } // Search elements..

    { // Insert elements at random.
      printf("\n");
      int i_init[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
      int n_i = sizeof(i_init) / sizeof(int);
      void **ar = srch_ptr_alloc(5, NULL);

      srand(12345);
      Elements(ar);
      int n_left = n_i;
      while (n_left > 0) {
        int num = rand() % n_i;
        int res = srch_ptr_ins_ord(&i_init[num], &ar, &cmp_i, 5);
        if (res >= 0) {
          n_left--;
          printf("%d inserted\n", num);
          Elements(ar);
          }
        }

      srch_ptr_free(&ar);
      printf("%p\n", ar);
      } // Insert elements..

    } // Dynamic arrays of pointers..

  } // arrays

//---------------------------------------------------------------- binary_trees
void binary_trees(void) {
#define AT_RANDOM
  srcht_tree tree = {};
#ifdef AT_RANDOM
//  int i_init[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25};
  int i_init[50];
  for (int k = 0; k < 50; k++) i_init[k] = k;
#else
  int i_init[] = {1, 3, 5, 7, 9, 11, 13};
#endif
  int n_i = sizeof(i_init) / sizeof(int);
  srcht_node *node = NULL;
#ifdef AT_RANDOM
  srand(12356);
  printf("Inserting %d nodes from [ ", n_i);
  for (int k = 0; k < n_i; k++) printf("%d ", i_init[k]);
  printf("] at random.\n");
#else
  printf("Inserting the %d nodes [ ", n_i);
  for (int k = 0; k < n_i; k++) printf("%d ", i_init[k]);
  printf("] in sequence.\n");
#endif
  for (int k = 0; k < n_i; k++) {
#ifdef AT_RANDOM
    int id = i_init[rand() % n_i];
#else
    int id = i_init[k];
#endif
    node = srcht_ins(id, &tree);
    if (node) {
      printf("Inserted node %d\n", node->id);
//      srcht_display_node(node);
//      printf("root=%p n_nodes=%d\n", tree.root, tree.n_nodes);
      }
    else {
      printf("=== Failed to insert node %d\n", id);
      }
    } // for (int k..
  printf("\n");
  srcht_list_tree(&tree);
  printf("\n");
//  srcht_traverse_tree(tree.root, 1);

  int n_del = tree.n_nodes;
  printf("deleting %d of %d nodes at random.\n", n_del, tree.n_nodes);
  for (int k = 0; k < n_del; k++) {
    int id;
    int res;
    do {
      id = i_init[rand() % n_i];
      res = srcht_del(id, &tree);
      } while (res != 1);
    printf("%2d: Deleted %2d\n", k, id);
    srcht_list_tree(&tree);
    }
  srcht_traverse_tree(tree.root, 1);
  printf("%p %d\n", tree.root, tree.n_nodes);

  int n = srcht_free_tree(&tree);
  printf("Freed %d nodes from the tree\n", n);
  } // binary_trees
