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
#include "btree.h"

#define SEED 10002 // overwritten by argument 1
#define N 25

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
/*
#include <sys/resource.h>
   struct rlimit rl;
   int result = getrlimit(RLIMIT_STACK, &rl);
   if (result == 0) printf("*** stack limit = %ld\n", rl.rlim_cur);
   result = getrlimit(RLIMIT_DATA, &rl);
   if (result == 0) printf("*** data limit = %ld\n", rl.rlim_cur);
*/

  int seed = SEED;
  if (argc > 1) seed = atoi(argv[1]);
  srand(seed);
  printf("*** Bintree %d\n", seed);


  btr_node *root = btr_random_allocate(seed, N);
//  printf("root: %d %p\n", btr_get_node_index(root), root);
//  printf("calculated max depth (r): %d\n", btr_calc_tree_max_depth_r(root));
//  printf("calculated max depth (i): %d\n", btr_calc_tree_max_depth_i(root));
  puts("");
  btr_list_nodes(1);
  puts("");
  btr_list_tree(root);
  btr_free();

  root = btr_full_allocate(5);
  puts("");
  btr_list_nodes(1);
  puts("");
  btr_make_inlevel_links(root);
  btr_list_tree(root);
  puts("");
//  btr_traverse_tree(root);
  btr_set_ordered_ids(root);
//  btr_list_nodes(1);
  btr_list_tree(root);

  btr_free();
	return EXIT_SUCCESS;
  }
