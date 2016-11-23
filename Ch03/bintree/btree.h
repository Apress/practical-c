/* btree.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef BTREE
#define BTREE

typedef struct btr_node btr_node;
struct btr_node {
  int id;
  int value;
  btr_node *child[2];
  btr_node *next;
  };

btr_node *btr_blank_allocate(int n);
int       btr_calc_tree_max_depth_i(btr_node *root);
int       btr_calc_tree_max_depth_r(btr_node *root);
void      btr_free(void);
btr_node *btr_full_allocate(int max_depth);
btr_node *btr_get_node_address(int k_node);
int       btr_get_node_index(btr_node *node);
int       btr_get_node_value(int k_node);
int       btr_get_tree_depth(void);
void      btr_list_nodes(int with_addresses);
void      btr_list_tree(btr_node *node);
void      btr_make_inlevel_links(btr_node *root);
btr_node *btr_random_allocate(int seed, int n);
void      btr_set_node_value(int k_node, int value);
void      btr_set_ordered_ids(btr_node *node);
void      btr_traverse_tree(btr_node *node);

#endif
