/* search_tree.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "search_tree.h"

static srcht_node *balance(int k1, int k2, srcht_node **nodes);
static void build_node_array(srcht_node *node, srcht_node **nodes, int init_k);
static int calc_depth_r(srcht_node *node, int level);
static int find(int id, srcht_node *from, srcht_node **res);
static int free_branch(srcht_node *node);
static int set_node_coords(srcht_node *node, int offs, int depth,
    int max_depth);
static void traverse_tree(srcht_node *node, int with_addresses, int do_init);

//--------------------------------------------------------------------- balance
srcht_node *balance(int k1, int k2, srcht_node **nodes) {
  int k = (k1 + k2) / 2;
  srcht_node *node = nodes[k];
  if (node) {
    nodes[k] = NULL;
    srcht_node *child = balance(k1, k, nodes);
    node->child[0] = child;
    if (child) child->parent = node;
    child = balance(k, k2, nodes);
    node->child[1] = child;
    if (child) child->parent = node;
    }
  return node;
  } // balance

//------------------------------------------------------------ build_node_array
void build_node_array(srcht_node *node, srcht_node **nodes, int init_k) {
  static int k = 0;
  if (init_k) k = 0;
  if (node != NULL) {
    build_node_array(node->child[0], nodes, 0);
    nodes[k++] = node;
    build_node_array(node->child[1], nodes, 0);
    }
  } // build_node_array

//---------------------------------------------------------------- calc_depth_r
int calc_depth_r(srcht_node *node, int level) {
  int depth[2] = {0};
  for (int k = 0; k < 2; k++) {
    srcht_node *child = node->child[k];
    depth[k] = (child) ? calc_depth_r(child, level + 1) : level;
    }
  return (depth[0] >= depth[1]) ? depth[0] : depth[1];
  } // calc_depth_r

//------------------------------------------------------------------------ find
// Returns 1 when successful. It always sets res.
int find(int id, srcht_node *node, srcht_node **res) {
  if (node->id == id) {
    *res = node;
    return 1;                                                             //-->
    }
  srcht_node *child = node->child[(id < node->id) ? 0 : 1];
  if (!child) {
    *res = node;
    return 0;                                                             //-->
    }
  return find(id, child, res);
  } // find

//----------------------------------------------------------------- free_branch
// Returns the number of nodes removed.
int free_branch(srcht_node *node) {
  int n = 0;
  if (node->child[0]) n = free_branch(node->child[0]);
  if (node->child[1]) n += free_branch(node->child[1]);
  free(node);
  return n + 1;
  } // free_branch

//------------------------------------------------------------- set_node_coords
int set_node_coords(srcht_node *node, int offs, int depth, int max_depth) {
  static int min_col = INT_MAX;
  if (offs < min_col) min_col = offs;

  // The offset of a child with respect to the parent is given by
  // (1 << (max_depth - depth)), negative for child[0] and positive for
  // child[1] (where depth is the depth of the parent).
  node->value = offs * 100 + depth;
  int delta = 1 << (max_depth - depth);
  int child_offs[2] = {0};
  child_offs[0] = offs - delta;
  child_offs[1] = offs + delta;
  for (int k = 0; k < 2; k++) {
    srcht_node *child = node->child[k];
    if (child != NULL) {
      set_node_coords(child, child_offs[k], depth + 1, max_depth);
      }
    }
  return min_col;
  } // set_node_coords

//---------------------------------------------------------- srcht_balance_tree
// Returns 1 if successful.
int srcht_balance_tree(srcht_tree *tree) {
  if (!tree || !tree->root) return 0;                                     //-->
  int n_nodes = tree->n_nodes;
  if (n_nodes < 3) return 1;                                              //-->

  // Build the list of nodes.
  srcht_node **nodes = malloc(n_nodes * sizeof(srcht_node *));
  if (!nodes) return 0;                                                   //-->
  build_node_array(tree->root, nodes, 1);

  // Balance the tree.
  int k_new_root = n_nodes / 2;
  tree->root = nodes[k_new_root];
  tree->root->parent = NULL;
  nodes[k_new_root] = NULL;
  srcht_node *child = balance(0, k_new_root, nodes);
  tree->root->child[0] = child;
  if (child) child->parent = tree->root;
  child = balance(k_new_root + 1, n_nodes, nodes);
  tree->root->child[1] = child;
  if (child) child->parent = tree->root;

  free(nodes);
  return 1;
  } // srcht_balance_tree

//------------------------------------------------------ srcht_build_node_array
void srcht_build_node_array(srcht_node *node, srcht_node **nodes) {
  build_node_array(node, nodes, 1);
  } // srcht_build_node_array

//------------------------------------------------- srcht_calc_tree_max_depth_r
int srcht_calc_tree_max_depth_r(srcht_node *root) {
  return calc_depth_r(root, 0);
  } // srcht_calc_tree_max_depth_r

//------------------------------------------------------------------- srcht_del
// It returns 1 when successful and 0 when unsuccessful.
int srcht_del(int id, srcht_tree *tree) {
  srcht_node *node = NULL;
  if (srcht_find(id, tree, &node) <= 0) return 0;                         //-->
  return srcht_del_node(node, tree);
  } // srcht_del

//-------------------------------------------------------------- srcht_del_node
// Returns 1 if successful.
int srcht_del_node(srcht_node *node, srcht_tree *tree) {
  if (!tree || !node) return 0;                                           //-->
  srcht_node **hook = NULL;
  srcht_node *parent = node->parent;
  if (parent) for (int k = 0; k < 2; k++) {
    if (parent->child[0] == node) hook = &parent->child[0];
    else hook = &parent->child[1];
    }
  else { // it is the root
    hook = &tree->root;
    }
  srcht_node *c0 = node->child[0];
  srcht_node *c1 = node->child[1];

  // Now that we have saved the pointers contained in the node structure,
  // we can release it.
  free(node);
  tree->n_nodes--;

  if (!c0 && !c1) { // Simplest case: the node has no children.
    *hook = NULL;
    return 1;                                                             //-->
    }

  if (!c0 || !c1) { // The node has one child.
    if (c0) {
      *hook = c0;
      c0->parent = parent;
      }
    else {
      *hook = c1;
      c1->parent = parent;
      }
    return 1;                                                             //-->
    }

  if (!c0->child[1]) { // The node has two children. Try the simpler way.
    *hook = c0;
    c0->parent = parent;
    c0->child[1] = c1;
    c1->parent = c0;
    return 1;                                                             //-->
    }
  if (!c1->child[0]) {
    *hook = c1;
    c1->parent = parent;
    c1->child[0] = c0;
    c0->parent = c1;
    return 1;                                                             //-->
    }

  // No can do. Let's follow the left branches below c1 until we hit
  // a NULL (i.e., until we find a node with a free left child).
  srcht_node *old_parent = c1;
  node = c1->child[0];
  while (node->child[0]) {
    old_parent = node;
    node = node->child[0];
    }
  old_parent->child[0] = node->child[1];
  if (node->child[1]) node->child[1]->parent = old_parent;
  *hook = node;
  node->parent = parent;
  node->child[0] = c0;
  c0->parent = node;
  node->child[1] = c1;
  c1->parent = node;
  return 1;
  } // srcht_del_node

//---------------------------------------------------------- srcht_display_node
void srcht_display_node(srcht_node *node) {
  printf("%p", node);
  if (node) {
    printf("(%d) parent=%p", node->id, node->parent);
    if (node->parent) printf("(%d)", node->parent->id);
    for (int k = 0; k < 2; k++) {
      srcht_node *child = node->child[k];
      printf(" child%d=%p", k, child);
      if (child) printf("(%d)", child->id);
      }
    }
  printf("\n");
  } // srcht_display_node

//------------------------------------------------------------------ srcht_find
// It returns 1 when successful, 0 when unsuccessful, and -2 when the
// search couldn't be carried out, in which case res remains unchanged.
// It sets res to the address of the requested node if its id is the
// requested id or the address of the parent node of which the node
// should have been a child if it existed. A NULL res means that there
// are no nodes in the tree.
int srcht_find(int id, srcht_tree *tree, srcht_node **res) {
  if (!tree) return -2;                                                   //-->
  srcht_node *root = tree->root;
  if (root == NULL) {
    *res = NULL;
    return 0;                                                             //-->
    }
  return find(id, root, res);
  } // srcht_find

//------------------------------------------------------------- srcht_free_tree
// Returns the number of nodes removed.
int srcht_free_tree(srcht_tree *tree) {
  int n = 0;
  if (tree && tree->root) {
    n = free_branch(tree->root);
    tree->root = NULL;
    tree->n_nodes -= n;
    }
  return n;
  } // srcht_free_tree

//------------------------------------------------------------------- srcht_ins
// Returns the address of the node or NULL if unsuccessful.
srcht_node *srcht_ins(int id, srcht_tree *tree) {
  if (!tree) return NULL;                                                 //-->
  srcht_node *node = NULL;
  if (srcht_find(id, tree, &node)) return NULL;                           //-->
  srcht_node *parent = node;
  node = calloc(1, sizeof(srcht_node));
  if (node == NULL) return NULL;                                          //-->
  node->id = id;
  node->parent = parent;
  if (!parent) { // first node in the tree
    tree->root = node;
    tree->n_nodes = 1;
    return node;                                                          //-->
    }
  srcht_node **child_p = &parent->child[(id < parent->id) ? 0 : 1];

  // If the target address is already occupied, attach that node to
  // the new one as the appropriate child before attaching the new
  // node to the tree.
  if (!child_p) node->child[((*child_p)->id < id) ? 0 : 1] = *child_p;
  *child_p = node;
  tree->n_nodes++;
  return node;
  } // srcht_ins

//------------------------------------------------------------- srcht_list_tree
void srcht_list_tree(srcht_tree *tree) {
#define DEPTH_LIMIT 20
  if (!tree || !tree->root) {
    printf("srcht_list_tree: tree empty or non-existent\n");
    return;                                                               //-->
    }
  if (tree->root->child[0] == NULL && tree->root->child[1] == NULL) {
    printf("%03d\n", tree->root->id);
    return;                                                               //-->
    }
  int max_depth = srcht_calc_tree_max_depth_r(tree->root);
  if (max_depth > DEPTH_LIMIT) {
    printf("srcht_list_tree: max_depth=%d exceeds the limit of %d\n",
        max_depth, DEPTH_LIMIT);
    return;                                                               //-->
    }

  // Set the value of each node to the node's coordinates expressed as
  // column * 100 + depth.
  int min_col = set_node_coords(tree->root, 1 << (max_depth + 1), 0, max_depth);

  // Calculate the maximum number of characters needed to display the
  // the bottom level of the tree: 1 << max_depth nodes and
  // 3 digits + 1 space for each node.
  int bottom_w = 1 << (max_depth + 2);

  // Allocate the strings to compose the tree for printing.
  int len = bottom_w - min_col + 1;
  char (*lines)[len + 1] = malloc((len + 1) * 2 * (max_depth + 1));
  if (lines == NULL) {
    fprintf(stderr,
        "srcht_list_tree: failed to allocate %d characters\n",
        (len + 1) * 2 * (max_depth + 1));
    fflush(stderr);
    abort();                                                              //-->
    }
  for (int level = 0; level <= max_depth; level++) {
    char *line = lines[level * 2];
    char *sep = lines[level * 2 + 1];
    for (int i = 0; i < len; i++) line[i] = sep[i] = ' ';
    line[len] = sep[len] = '\0';
    }

  // Build the list of nodes.
  int n_nodes = tree->n_nodes;
  srcht_node **nodes = malloc(n_nodes * sizeof(srcht_node *));
  build_node_array(tree->root, nodes, 1);

  // Compose the tree.
  for (int k = 0; k < n_nodes; k++) {
    srcht_node *node = nodes[k];
    int level = node->value % 100;
    char *line = lines[level * 2];
    char *sep = lines[level * 2 + 1];

    // First print the node identifier.
    char num[4] = "";
    sprintf(num, "%03d", node->id);
    int col = node->value / 100 - min_col;
    char *where = line + col;
    for (int i = 0; i < 3; i++) where[i] = num[i];

    // Then, if appropriate, print the separation line below it.
    col++;
    where = sep + col;
    if (node->child[0] && node->child[1]) *where = '^';
    else if (node->child[0] || node->child[1]) *where = '\'';
    //
    int n_dash = (1 << (max_depth - level)) - 1;
    if (node->child[0]) {
      where = sep + col - n_dash;
      for (int i = 0; i < n_dash; i++) *where++ = '-';
      where = sep + col - n_dash - 1;
      *where = '.';
      }
    if (node->child[1]) {
      where = sep + col + 1;
      for (int i = 0; i < n_dash; i++) *where++ = '-';
      where = sep + col + n_dash + 1;
      *where = '.';
      }
    } // for (int k..

  // "Hide" the trailing spaces.
  for (int level = 0; level <= max_depth * 2; level++) {
    int i = len - 1;
    while (lines[level][i] == ' ') lines[level][i--] = '\0';
    }

  //******* Optimize the output by removing empty space.
  //------- Removing "loops".
  // We stop at
  // max_depth - 2 because we need to look at grandchildren and the bottom
  // level is max_depth.
  for (int level = 0; level < max_depth - 1; level++) {
    for (int k = 0; k < n_nodes; k++) {
      srcht_node *parent = nodes[k];
      if (parent->value % 100 == level) {
        for (int k_dir = 0; k_dir < 2; k_dir ++) {
          srcht_node *node = parent->child[k_dir];
          if (node) {
            srcht_node *child = node->child[1 - k_dir];
            if (child && !node->child[k_dir]) {
              int incr = - k_dir * 2 + 1; // 'incr' calculated for the node
              int new_col = parent->value / 100 - min_col - incr * 2;
              int new_col_c = child->value / 100 - min_col - incr * 2;
              if (new_col_c * incr < new_col * incr) new_col = new_col_c;
              char *line_u = lines[level * 2 + 1];
              char *line = lines[level * 2 + 2];
              char *line_d = lines[level * 2 + 3];
              int col = node->value / 100 - min_col;
              if (col != new_col) {

//printf("%d: %03d(%d) %03d %03d\n", level + 1, k, k_dir,
//    srcht_get_node_index(node), srcht_get_node_index(child));

                char num[3];
                for (int i = 0; i < 3; i++) {
                  num[i] = line[col + i];
                  line[col + i] = ' ';
                  }

                if (incr > 0) { // moving to the right
                  int i = col + 1;
                  while (i < new_col) {
                    line_u[i] = ' ';
                    line[i] = ' ';
                    line_d[i++] = ' ';
                    }
                  line[i] = num[0];
                  line_u[i] = ' ';
                  line_d[i] = ' ';
                  line[++i] = num[1];
                  line_u[i] = '.';
                  line_d[i] = '\'';
                  line[++i] = num[2];
                  }
                else { // moving to the left
                  int i = new_col;
                  line[i] = num[0];
                  line[++i] = num[1];
                  line_u[i] = '.';
                  line_d[i] = '\'';
                  line[++i] = num[2];
                  if (line[col + 3] == '\0') {
                    line_u[i] = '\0';
                    line_d[i++] = '\0';
                    line[i] = '\0';
                    }
                  else {
                    while (i <= col + 1) {
                      line_u[i] = ' ';
                      line_d[i++] = ' ';
                      }
                    }
                  }

                node->value = (new_col + min_col) * 100 + level + 1;
                } // if (col..
              } // if (child..
            } // if (node..
          } // for (k_dir..

        } // if (parent->value..
      } // for (int k..
    } // for (int level..

  //------- If the root has only one child, you can move the root's number to
  // above its only child.
  if (!tree->root->child[0] || !tree->root->child[1]) {
    char *line = lines[0];
    int col_p = tree->root->value / 100 - min_col;
    int child_right = (tree->root->child[0]) ? 0 : 1;
    srcht_node *child = tree->root->child[child_right];
    int col = child->value / 100 - min_col;

    // Move the root's node identifier.
    char num[3];
    for (int i = 0; i < 3; i++) {
      num[i] = line[col_p + i];
      line[col_p + i] = ' ';
      }
    char *line0 = lines[1];
    if (child_right) {

      // If the child is on the right side, the root number is followed by
      // '\0's, which have to be replaced with spaces. Otherwise, the root
      // number will not be seen.
      for (int i = 3; i < col; i++) line[i] = ' ';

      int i = col_p + 1;
      while (i < col - 2) line0[i++] = ' ';
      line[i] = num[0];
      line0[i] = ' ';
      line[++i] = num[1];
      line0[i] = '\'';
      line[++i] = num[2];
      }
    else {
      int i = col + 2;
      line[i] = num[0];
      line[++i] = num[1];
      line0[i] = '\'';
      line[++i] = num[2];
      line0[i] = '\0';
      line[++i] = '\0';
      }
    }

  //------- Regardless of the depth, you can move all leaves to be close to
  // their parents.  The order in which you find them doesn't matter.
  // Remember that at this point, column and level are stored in value.
  for (int k = 0; k < n_nodes; k++) {
    srcht_node *node = nodes[k];
    if (!node->child[0] && !node->child[1]) {

      // A childless node.
      int level = node->value % 100;
      int col = node->value / 100 - min_col;
      int col_p = node->parent->value / 100 - min_col;
      int distance = col_p - col;

      // Move the node identifier, but only if it needs moving.
      if (distance < -2 || distance > 2) {
        int incr = (distance > 0) ? 1 : -1;
        char *line = lines[level * 2];
        char *line0 = lines[level * 2 - 1];
        char num[3];
        for (int i = 0; i < 3; i++) {
          num[i] = line[col + i];
          line[col + i] = ' ';
          }

        // Write the node identifier in its new position.
        if (incr > 0) {

          // If the node is the last of the line and it is a left child,
          // you need to replace the '\0's that follow its original
          // position with spaces.  Otherwise it will not be seen.
          if (line[col + 3] == '\0') {
            for (int i = col + 3; i < col_p - 1; i++) line[i] = ' ';
            }
          int i = col + 1;
          while (i < col_p - 2) line0[i++] = ' ';
          line[i] = num[0];
          line0[i] = ' ';
          line[++i] = num[1];
          line0[i] = '.';
          line[++i] = num[2];
          }
        else {
          int i = col_p + 2;
          line[i] = num[0];
          line[++i] = num[1];
          line0[i] = '.';
          line[++i] = num[2];
          if (line[col + 3] != '\0') {
            while (i <= col + 1) line0[i++] = ' ';
            }
          else {

            // If a right child is at the end of a line, you have to
            // hide the trailing spaces.
            line0[i] = '\0';
            line[++i] = '\0';
            }
          }
        } // if (distance..
      } // if (!node->child[0]..
    } // for (int k..

  //------- Build a line with the 'or' of all lines containing numbers. The
  // groups of columns with empty spaces can certainly be reduced to single
  // columns.

  char *l = lines[max_depth * 2 + 1];  // not used
  for (int j = 0; j < len; j++) l[j] = '.';
  int llen = 0;
  for (int k = 0; k <= max_depth; k++) {
    char *line = lines[k * 2];
    for (int j = 0; line[j] != '\0'; j++) {
      if (line[j] != ' ') {
        l[j] = line[j];
        if (j > llen) llen = j;
        }
      }
    }
  llen++;
  l[llen] = '\0';
  int count = 0;
  for (int j = llen - 3; j > 1; j--) {
    if (l[j] == '.') {
      count++;
      }
    else if (count < 2) { // ignore single dots
      count = 0;
      }
    else {

      // j + 1 is the leftmost dot; j + count is the rightmost dot.
      // Overwrite in all lines the characters from j + 2 to j + count.
      for (int k = 0; k <= max_depth * 2 + 1; k++) {
        for (int i = j + 2; i <= strlen(lines[k]); i++) {
          lines[k][i] = lines[k][i + count - 1];
          }
        }
      count = 0;
      }
    }

  //------- We still have to check whether there are spaces beginning from
  // column zero. If yes, we have to remove them.
  int i_space = 0;
  while (l[i_space] == '.') i_space++;
  for (int k = 0; k <= max_depth * 2 + 1; k++) {
    int limit = strlen(lines[k]) - i_space;
    for (int i = 0; i <= limit; i++) {
      lines[k][i] = lines[k][i + i_space];
      }
    }

  //******* Print the tree.
  for (int k = 0; k < max_depth; k++) {
    printf("%s\n%s\n", lines[k * 2], lines[k * 2 + 1]);
    }
  printf("%s\n", lines[max_depth * 2]);

  free(lines);
  free(nodes);
  } // srcht_list_tree

//--------------------------------------------------------- srcht_traverse_tree
void srcht_traverse_tree(srcht_node *node, int with_addresses) {
  traverse_tree(node, with_addresses, 1);
  } // srcht_traverse_tree

//--------------------------------------------------------------- traverse_tree
void traverse_tree(srcht_node *node, int with_addresses, int do_init) {
  static int k = -1;
  if (do_init) {
    k = -1;
    printf("  #   id idL idR  ..%s\n",
        (with_addresses) ? "     Address         Left       Right" : ""
        );
    }

  if (node != NULL) {
    traverse_tree(node->child[0], with_addresses, 0);
    k++;
    printf("%3d:%4d%4d%4d%4d", k, node->id,
         (node->child[0]) ? node->child[0]->id : -1,
         (node->child[1]) ? node->child[1]->id : -1,
         (node->parent) ? node->parent->id : 0
         );
     if (with_addresses) {
       printf("%12p:%12p%12p", node, node->child[0], node->child[1]);
       }
     puts("");
     traverse_tree(node->child[1], with_addresses, 0);
    }
  } // traverse_tree
