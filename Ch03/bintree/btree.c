/* btree.c
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
#include "btree.h"

static btr_node *nodes;
static int n_nodes = 0;
static btr_node **avail = NULL;
static int n_avail = 0;
static int max_depth = 0;

static int calc_depth_r(btr_node *node, int level);
static btr_node *pick_avail(void);
static int set_node_coords(btr_node *node, int offs, int depth);
static void write_level_in_nodes(btr_node *node, int level);
void write_level_pointers(btr_node *node, int level,
    btr_node **first, btr_node **last);

//---------------------------------------------------------- btr_blank_allocate
btr_node *btr_blank_allocate(int n) {
  n_nodes = n;
  if (n_nodes < 2 || n_nodes > 99) {
    fprintf(stderr, "More than 99 or less than 2 nodes\n");
    fflush(stderr);
    abort();                                                             // -->
    }

  // Allocate the array of nodes
  nodes = malloc(n * sizeof(btr_node));
  if (nodes == NULL) {
    fprintf(stderr, "btr_blank_allocate 1: failed to allocate %d-tree\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }

  // Initialize the list.
  for (int k = 0; k < n; k++) {
    nodes[k].id = k;
    nodes[k].value = 0;
    nodes[k].child[0] = nodes[k].child[1] = nodes[k].next = NULL;
    }
  return &nodes[0];
  } // btr_blank_allocate

//--------------------------------------------------- btr_calc_tree_max_depth_i
int btr_calc_tree_max_depth_i(btr_node *root) {

  // Set the value of nodes to the highest value of the children + 1.
  // Repeat the step until no node value changes.
  // We can use a do loop because we know that there are at least two levels.
  for (int k = 0; k < n_nodes; k++) nodes[k].value = 0;
  int n_mod = 0;
  do {
    n_mod = 0;
    for (int k = 0; k < n_nodes; k++) {
      btr_node  *node = &nodes[k];
      int val_max = 0;
      for (int j = 0; j < 2; j++) {
        btr_node *child = node->child[j];
        if (child != NULL && child->value > val_max) val_max = child->value;
        }
      if (node->value < val_max + 1) {
        node->value = val_max + 1;
        n_mod++;
        }
      }
    } while (n_mod > 0);

  return root->value - 1;
  } // btr_calc_tree_max_depth_i

//--------------------------------------------------- btr_calc_tree_max_depth_r
int btr_calc_tree_max_depth_r(btr_node *root) {
  return calc_depth_r(root, 0);
  } // btr_calc_tree_max_depth_r

//-------------------------------------------------------------------- btr_free
void btr_free(void) {
  if (nodes) {
    free(nodes);
    nodes = NULL;
    }
  } // btr_free

//----------------------------------------------------------- btr_full_allocate
btr_node *btr_full_allocate(int max_depth) {
#define MAX_MAX_DEPTH 20
  if (max_depth < 1 || max_depth > MAX_MAX_DEPTH) {
    fprintf(stderr,
        "btr_full_allocate: max_depth=%d is outside the range 1..%d\n",
        max_depth, MAX_MAX_DEPTH);
    fflush(stderr);
    abort();                                                             // -->
    }

  // Allocate the nodes.
  n_nodes = (1 << (max_depth + 1)) - 1;
  (void)btr_blank_allocate(n_nodes);

  // Link the nodes to their children, level by level.
  int next_free = 1;
  for (int level = 0; level < max_depth; level++) {
    int nn = 1 << level;
    int k_first = next_free - nn;
    for (int k = 0; k < nn; k++) {
      btr_node *parent = &nodes[k_first + k];
      parent->child[0] = &nodes[next_free++];
      parent->child[1] = &nodes[next_free++];
      }
    }

  // Chain the nodes like in make_inlevel_links().
  // It is easier to do it here than when setting the children links.
  btr_node *node1 = &nodes[0];
  for (int k = 1; k < n_nodes; k++) {
    btr_node *node2 = &nodes[k];
    node1->next = node2;
    node1 = node2;
    }

  return &nodes[0];
  } // btr_full_allocate

//-------------------------------------------------------- btr_get_node_address
btr_node *btr_get_node_address(int k_node) {
  return &nodes[k_node];
  } // btr_get_node_address

//---------------------------------------------------------- btr_get_node_index
int btr_get_node_index(btr_node *node) {
  int k_node = 0;
  while (&nodes[k_node] != node) k_node++;
  return k_node;
  } // btr_get_node_index

//---------------------------------------------------------- btr_get_node_value
int btr_get_node_value(int k_node) {
  return nodes[k_node].value;
  } // btr_get_node_value

//-------------------------------------------------------------- btr_list_nodes
void btr_list_nodes(int with_addresses) {
  printf("  #   id val idL idR  ..     Address         Left       Right\n");
  for (int k = 0; k < n_nodes; k++) {
    btr_node *node = &nodes[k];
    printf("%3d:%4d%4d%4d%4d%4d", k, node->id, node->value,
        (node->child[0]) ? node->child[0]->id : -1,
        (node->child[1]) ? node->child[1]->id : -1,
        (node->next) ? node->next->id : 0
        );
    if (with_addresses) {
      printf("%12p:%12p%12p", node, node->child[0], node->child[1]);
      }
    puts("");
    }
  } // btr_list_nodes

//--------------------------------------------------------------- btr_list_tree
void btr_list_tree(btr_node *root) {
#define DEPTH_LIMIT 20

  if (root->child[0] == NULL && root->child[1] == NULL) {
    fprintf(stderr, "btr_list_tree: the root node has no children\n");
    fflush(stderr);
    abort();                                                             // -->
    }

  // Calculate the maximum depth of the tree
  max_depth = btr_calc_tree_max_depth_i(root);
  if (max_depth > DEPTH_LIMIT) {
    fprintf(stderr,
        "btr_list_tree: max_depth=%d exceeds the limit of %d\n",
        max_depth, DEPTH_LIMIT);
    fflush(stderr);
    abort();                                                             // -->
    }

  // Set the value of each node to the node's coordinates expressed as
  // column * 100 + depth.
  int min_col = set_node_coords(root, 1 << (max_depth + 1), 0);

  // Calculate the maximum number of characters needed to display the
  // the bottom level of the tree: 1 << max_depth nodes and
  // 3 digits + 1 space for each node.
  int bottom_w = 1 << (max_depth + 2);

  // Allocate the strings to compose the tree for printing.
  int len = bottom_w - min_col + 1;
  char (*lines)[len + 1] = malloc((len + 1) * 2 * (max_depth + 1));
  if (lines == NULL) {
    fprintf(stderr,
        "btr_list_tree: failed to allocate %d characters\n",
        (len + 1) * 2 * (max_depth + 1));
    fflush(stderr);
    abort();                                                             // -->
    }
  for (int level = 0; level <= max_depth; level++) {
    char *line = lines[level * 2];
    char *sep = lines[level * 2 + 1];
    for (int i = 0; i < len; i++) line[i] = sep[i] = ' ';
    line[len] = sep[len] = '\0';
    }

  // Compose the tree.
  for (int k = 0; k < n_nodes; k++) {
    btr_node *node = &nodes[k];
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
      btr_node *parent = &nodes[k];
      if (parent->value % 100 == level) {
        for (int k_dir = 0; k_dir < 2; k_dir ++) {
          btr_node *node = parent->child[k_dir];
          if (node) {
            btr_node *child = node->child[1 - k_dir];
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
//    btr_get_node_index(node), btr_get_node_index(child));

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
  if (!root->child[0] || !root->child[1]) {
    char *line = lines[0];
    int col_p = root->value / 100 - min_col;
    int child_right = (root->child[0]) ? 0 : 1;
    btr_node *child = root->child[child_right];
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
    btr_node *node = &nodes[k];
    if (!node->child[0] && !node->child[1]) {

      // A childless node.  Find the parent.
      btr_node *parent = NULL;
      for (int j = 0; j < n_nodes && parent == NULL; j++) {
        btr_node *p = &nodes[j];
        if (p->child[0] == node || p->child[1] == node) parent = p;
        }
      int level = node->value % 100;
      int col = node->value / 100 - min_col;
      int col_p = parent->value / 100 - min_col;
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
  } // btr_list_tree

//------------------------------------------------------ btr_make_inlevel_links
void btr_make_inlevel_links(btr_node *root) {

  // Allocate and initialize the lists to store the addresses of the
  // first and last nodes in each level.
  int depth = calc_depth_r(root, 0);
  btr_node **first = malloc((depth + 1) * sizeof(btr_node *) * 2);
  if (first == NULL) {
    fprintf(stderr, "btr_make_inlevel_links: failed to allocate depth lists\n");
    fflush(stderr);
    abort();                                                             // -->
    }
  btr_node **last = &first[depth + 1];
  for (int i = 0; i <= depth; i++) first[i] = NULL;

  // Write the pointers within each level.
  write_level_pointers(root, 0, first, last);

/*
  for (int i = 0; i <= depth; i++) {
    printf("%d:", i);
    btr_node *node = first[i];
    do {
      printf(" %03d", node->id);
      node = node->next;
      } while (node);
    printf("\n");
    }
*/

  // Chain the levels.
  for (int i = 0; i < depth; i++) last[i]->next = first[i + 1];

  // Free the lists.
  free(first);

/*
  btr_node *node = root;
  do {
    printf(" %03d", node->id);
    node = node->next;
    } while (node);
  printf("\n");
*/
  } // btr_make_inlevel_links

//--------------------------------------------------------- btr_random_allocate
btr_node *btr_random_allocate(int seed, int n) {
  n_nodes = n;
  if (n_nodes < 2 || n_nodes > 99) {
    fprintf(stderr, "More than 99 or less than 2 nodes\n");
    fflush(stderr);
    abort();                                                             // -->
    }

  // Allocate the arrays
  nodes = malloc(n * sizeof(btr_node));
  if (nodes == NULL) {
    fprintf(stderr, "btr_random_allocate 1: failed to allocate %d-tree\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }
  size_t avail_size = n * sizeof(btr_node *);
  avail = malloc(avail_size + n * sizeof(int));
  if (avail == NULL) {
    fprintf(stderr, "btr_random_allocate 2: failed to allocate %d-tree\n", n);
    fflush(stderr);
    abort();                                                             // -->
    }
  int *widths = (int *)(avail + avail_size);
  for (int k = 0; k < n; k++) widths[k] = 0;

  // Initialize the lists.
  widths[0] = 1;
  int depth = 1;
  n_avail = n;
  for (int k = 0, max_w = 1, left = n - 1; k < n; k++) {
    nodes[k].id = k;
    nodes[k].value = 0;
    nodes[k].child[0] = nodes[k].child[1] = nodes[k].next = NULL;
    avail[k] = &nodes[k];
    if (k > 0 && left > 0) {
      max_w = widths[k - 1] * 2;
      widths[k] = rand() % ((max_w < left) ? max_w : left) + 1;
      left -= widths[k];
      depth = k;
      }
    }

/* Splitting the loop into two:
  for (int k = 0; k < n; k++) {
    nodes[k].value = 0;
    nodes[k].child[0] = NULL;
    nodes[k].child[1] = NULL;
    avail[k] = &nodes[k];
    }
  for (int k = 1, max_w = 1, left = n - 1; left > 0 && k < n; k++) {
    max_w = widths[k - 1] * 2;
    widths[k] = rand() % ((max_w < left) ? max_w : left) + 1;
    left -= widths[k];
    depth = k;
    }
*/

//for (int i = 0; i <= depth; i++) printf(" %d", widths[i]); puts("");
//for (int i = 0; i < n; i++) printf(" %p", avail[i]); puts("");

  // Pick the root node and save it at the end of avail[].
  btr_node *tree = pick_avail();
  int k_pos_up = n_avail;
  avail[k_pos_up] = tree;

  // Fill the "lower" levels of nodes one by one.
  for (int k = 1, k_pos = k_pos_up; k <= depth; k++) {
    for (int j = 0; j < widths[k]; j++) {
      btr_node *a_node = pick_avail();
      avail[--k_pos] = a_node;

      // Choose one of the available child pointers of a node one level above.
      btr_node **child_p = NULL;
      do {
        int k_parent = k_pos_up + rand() % widths[k - 1];
        btr_node *parent = avail[k_parent];
        child_p = &parent->child[rand() % 2];
        } while (*child_p != NULL);
      *child_p = a_node;
      }
    k_pos_up = k_pos;
    }

  free(avail);
  btr_make_inlevel_links(tree);
  return tree;
  } // btr_random_allocate

//---------------------------------------------------------- btr_set_node_value
void btr_set_node_value(int k_node, int value) {
  nodes[k_node].value = value;
  } // btr_set_node_value

//--------------------------------------------------------- btr_set_ordered_ids
void btr_set_ordered_ids(btr_node *node) {
  static int k_node = 0;
  if (node != NULL) {
    btr_set_ordered_ids(node->child[0]);
    node->id = k_node++;
    btr_set_ordered_ids(node->child[1]);
    }
  } // btr_set_ordered_ids

//----------------------------------------------------------- btr_traverse_tree
void btr_traverse_tree(btr_node *node) {
  if (node != NULL) {
    btr_traverse_tree(node->child[0]);
    printf("%03d\n", node->id);
    btr_traverse_tree(node->child[1]);
    }
  } // btr_traverse_tree

//---------------------------------------------------------------- calc_depth_r
int calc_depth_r(btr_node *node, int level) {
  int depth[2] = {0};
  for (int k = 0; k < 2; k++) {
    btr_node *child = node->child[k];
    depth[k] = (child) ? calc_depth_r(child, level + 1) : level;
    }
  return (depth[0] >= depth[1]) ? depth[0] : depth[1];
  } // calc_depth_r

//------------------------------------------------------------------ pick_avail
// Warning: avail[] is allocated and released in btr_random_allocate().
btr_node *pick_avail(void) {
  int k_picked = rand() % n_avail;
  btr_node *node = avail[k_picked];
  n_avail--;
  for (int j = k_picked; j < n_avail; j++) avail[j] = avail[j + 1];
  return node;
  } // pick_avail

//------------------------------------------------------------- set_node_coords
int set_node_coords(btr_node *node, int offs, int depth) {
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
    btr_node *child = node->child[k];
    if (child != NULL) set_node_coords(child, child_offs[k], depth + 1);
    }
  return min_col;
  } // set_node_coords

//-------------------------------------------------------- write_level_in_nodes
void write_level_in_nodes(btr_node *node, int level) {
  if (node) {
    node->value = level;
    write_level_in_nodes(node->child[0], level + 1);
    write_level_in_nodes(node->child[1], level + 1);
    }
  } // write_level_in_nodes

//-------------------------------------------------------- write_level_pointers
void write_level_pointers(btr_node *node, int level,
    btr_node **first, btr_node **last) {
  if (node != NULL) {
    write_level_pointers(node->child[0], level + 1, first, last);

    if (first[level] == NULL) first[level] = node;
    else last[level]->next = node;
    last[level] = node;

    write_level_pointers(node->child[1], level + 1, first, last);
    }
  } // write_level_pointers
