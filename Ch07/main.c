/* Array: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "dar.h"

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {

{
  //------- Examples of allocation and release
  printf("------- Example of allocation and release\n");
  DAR_setup;

  int i;
  printf("ii:\t");
  int *ii = DAR_new(i, 20);

  float f;
  printf("ff:\t");
  float *ff = DAR_new(f, 10);

  printf("dd:\t");
  double *dd = DAR_new((double)0, 4);

  printf("ddd:\t");
  double *ddd = DAR_new_a(dd, 10);

  printf("iii:\t");
  int *iii = DAR_new_a(ii, 15);

  char c;
  printf("cc:\t");
  char *cc = DAR_new(c, 50);

  printf("iiii:\t");
  int **iiii = DAR_new(ii, 3);

  printf("\nDAR_list:\n");
  DAR_list();

  printf("\ndd:\t");
  DAR_release(dd);

  printf("ff:\t");
  DAR_release(ff);

  printf("ii:\t");
  DAR_release(ii);

  printf("cc:\t");
  DAR_release(cc);

  printf("iii:\t");
  DAR_release(iii);

  printf("\nDAR_list:\n");
  DAR_list();

  printf("\niiii:\t");
  DAR_release(iiii);

  printf("ddd:\t");
  DAR_release(ddd);

  printf("iii:\t");
  DAR_release(iii);

  printf("\n\nDAR_list:\n");
  DAR_list();

  DAR_dismantle;
  }

{
  //------- Example of multiple stacks
  printf("\n------- Example of multiple stacks\n");
  printf("=== Set up stack 1\n");
  DAR_setup; // first stack

  printf("ii_1 (stack 1):\t");
  int *ii_1 = DAR_new((int)0, 10);

  printf("dd_1 (stack 1):\t");
  double *dd_1 = DAR_new((double)0, 8);

  printf("\n=== Set up stack 2\n");
  DAR_setup; // second stack

  printf("ff_2 (stack 2):\t");
  float *ff_2 = DAR_new((float)0, 7);

  printf("ff_1 (stack 1):\t");
  float *ff_1 = DAR_new_dar(ii_1, (float)0, 5);

  printf("cc_2 (stack 2):\t");
  float *cc_2 = DAR_new_dar(ff_2, (char)0, 6);

  printf("\nDAR_list_dar (stack 1):\n");
  DAR_list_dar(dd_1);

  printf("dd_1 (stack 1):\t");
  DAR_release(dd_1);

  printf("\nDAR_list (stack 2):\n");
  DAR_list();

  printf("\n=== Dismantle stack 2\n");
  DAR_dismantle; // second stack

  printf("\nDAR_list (stack 1):\n");
  DAR_list();

  printf("\n=== Dismantle stack 1\n");
  DAR_dismantle; // first stack
  }

{
  //------- Examples of array resizing
  printf("\n------- Examples of array resizing\n");
  DAR_setup;

  int *int1 = DAR_new((int)0, 10);
  for (int k = 0; k < DAR_get_len(int1); k++) int1[k] = k + 100;
  for (int k = 0; k < DAR_get_len(int1); k++) printf("%4d", int1[k]);
  printf("\n\n");

  printf("Extend to 12\n");
  DAR_extend(int1, 12);
  for (int k = 0; k < DAR_get_len(int1); k++) printf("%4d", int1[k]);
  printf("\n\n");

  printf("Reduce to 9 and copy 5 elements from old [3] to new [4]\n");
  DAR_resize(int1, 3, 5, 4, 9);
  for (int k = 0; k < DAR_get_len(int1); k++) printf("%4d", int1[k]);
  printf("\n\n");

  printf("Reduce to 3 but keep the last element at the end\n");
  DAR_resize(int1, 8, 1, 2, 3);
  for (int k = 0; k < DAR_get_len(int1); k++) printf("%4d", int1[k]);
  printf("\n\n");

  char *c1 = DAR_new((char)0, 10);
  sprintf(c1, "abcdefghi");
  printf("%s\n\n", c1);

  printf("Reduce to 6 and copy the last 6 elements\n");
  DAR_resize(c1, strlen(c1) - 5, 6, 0, 6);
  printf("%s\n\n", c1);

  printf("DAR_list:\n");
  DAR_list();

  DAR_dismantle;
  }

{
  //------- Examples of array copying
  printf("\n------- Examples of array copying\n");
  DAR_setup;

  int *int1 = DAR_new((int)0, 10);
  for (int k = 0; k < DAR_get_len(int1); k++) int1[k] = k + 100;
  for (int k = 0; k < DAR_get_len(int1); k++) printf("%4d", int1[k]);
  printf("\n\n");

  printf("clone:\n");
  int *int2 = NULL;
  DAR_clone(int1, int2);
  for (int k = 0; k < DAR_get_len(int2); k++) printf("%4d", int2[k]);
  printf("\n\n");

  int *int3 = NULL;
  printf("Copy 5 elements from old [3] to new [4]\n");
  DAR_copy(int1, 3, 5, int3, 4, 0);
  for (int k = 0; k < DAR_get_len(int3); k++) printf("%4d", int3[k]);
  printf("\n\n");

  printf("DAR_list:\n");
  DAR_list();

  DAR_dismantle;
  }


{
  typedef struct a_t {
    int value;
    } a_t;

  a_t el = {};

  //------- Examples of element selecting
  printf("\n------- Examples of element selecting\n");
  DAR_setup;

  int match1(void *el_p) {
    a_t *e_p = el_p;
    return (e_p->value > 15 && e_p->value < 60);
    }
  a_t *ar1 = DAR_new(el, 8);
  for (int k = 0; k < DAR_get_len(ar1); k++) ar1[k].value = k * 11;
  for (int k = 0; k < DAR_get_len(ar1); k++) {
    printf("%d: %d\n", k, ar1[k].value);
    }
  printf("Looking for values > 15 and < 60\n");
  int *ind1 = dar_index_matches(ar1, match1);
  printf("#matches: %d\n", dar_count_matches(ar1, match1));
  for (int i = 0; i < DAR_get_len(ind1); i++) printf("%d\n", ind1[i]);
  printf("\n");

  int match2(void *el_p) {
    a_t **e_p = el_p;
    return (*e_p != NULL);
    }
  a_t **ar2 = DAR_new(&el, 10);
  for (int k = 2; k < DAR_get_len(ar2) / 2; k++) ar2[k] = &el;
  for (int k = 0; k < DAR_get_len(ar2); k++) {
    printf("%d: %p\n", k, ar2[k]);
    }
  printf("Looking for non-null elements\n");
  int *ind2 = dar_index_matches(ar2, match2);
  printf("#matches: %d\n", dar_count_matches(ar2, match2));
  for (int i = 0; i < DAR_get_len(ind2); i++) printf("%d\n", ind2[i]);
  printf("\n");

  printf("DAR_list:\n");
  DAR_list();

  DAR_dismantle;
  }

  return EXIT_SUCCESS;
  }
