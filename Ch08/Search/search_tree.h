/* search_tree.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef SRCHT
#define SRCHT

typedef struct srcht_node srcht_node;
struct srcht_node {
  int id;
  int value;
  srcht_node *child[2];
  srcht_node *parent;
  };
typedef struct srcht_tree {
  srcht_node *root;
  int n_nodes;
  } srcht_tree;

int        srcht_balance_tree(srcht_tree *tree);
void       srcht_build_node_array(srcht_node *node, srcht_node **nodes);
int        srcht_calc_tree_max_depth_r(srcht_node *root);
int        srcht_del(int id, srcht_tree *tree);
int        srcht_del_node(srcht_node *node, srcht_tree *tree);
void       srcht_display_node(srcht_node *node);
int        srcht_find(int id, srcht_tree *tree, srcht_node **res);
int        srcht_free_tree(srcht_tree *tree);
int        srcht_get_tree_depth(void);
srcht_node *srcht_ins(int id, srcht_tree *tree);
void       srcht_list_tree(srcht_tree *tree);
void       srcht_traverse_tree(srcht_node *node, int with_addresses);

#endif
