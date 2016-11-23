/* compare.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compare.h"

#define Min3(a, b, c) ({                                        \
  unsigned int aa = a;                                          \
  unsigned int bb = b;                                          \
  unsigned int cc = c;                                          \
  (aa <= bb) ? ((aa <= cc) ? aa : cc) : ((bb <= cc) ? bb : cc); \
  })

void print_row(int *i, int max);

//------------------------------------------------------------ cmpr_levenshtein
int cmpr_levenshtein(char *s1, char *s2) {
  if (s1 == NULL || s2 == NULL) return -1;                                //-->
  int n1 = strlen(s1);
  int n2 = strlen(s2);
  if (n1 == 0) return n2;                                                 //-->
  if (n2 == 0) return n1;                                                 //-->
  if (n1 > n2) {
    char *s_tmp = s1;
    int n_tmp = n1;
    s1 = s2;
    n1 = n2;
    s2 = s_tmp;
    n2 = n_tmp;
    }
  int previous_row[n1 + 1];
  int row[n1 + 1];
  int *p = &previous_row[0];
  int *q = &row[0];
  for (int j = 0; j <= n1; j++) p[j] = j;
/*
  printf("      ");
  for (int j = 0; j < n1; j++) printf("%2c", s1[j]);
  printf("\n");
  printf("p  :");
  print_row(p, n1);
*/
  for (int k = 1; k <= n2; k++) {
    q[0] = k;
    for (int j = 1; j <= n1; j++) {
      int cost = (s1[j - 1] == s2[k - 1]) ? 0 : 1;
      q[j] = Min3(q[j - 1] + 1, p[j] + 1, p[j - 1] + cost);
      }
    int *d_tmp = p;
    p = q;
    q = d_tmp;
/*
    printf("p%2c:", s2[k-1]);
    print_row(p, n1);
*/
    }
  return p[n1];
  } // cmpr_levenshtein

//------------------------------------------------------- cmpr_levenshtein_wiki
int cmpr_levenshtein_wiki(char *s1, char *s2) {
  int l1 = strlen(s1);
  int l2 = strlen(s2);
  int mat[l2 + 1][l1 + 1];
  mat[0][0] = 0;
  for (int k = 1; k <= l2; k++) mat[k][0] = mat[k - 1][0] + 1;
  for (int j = 1; j <= l1; j++) mat[0][j] = mat[0][j - 1] + 1;
  for (int k = 1; k <= l2; k++) {
    for (int j = 1; j <= l1; j++) {
      mat[k][j] = Min3(
          mat[k - 1][j] + 1,
          mat[k][j - 1] + 1,
          mat[k - 1][j - 1] + ((s1[j - 1] == s2[k - 1]) ? 0 : 1)
          );
      }
    }
  return mat[l2][l1];
  } // cmpr_levenshtein_wiki

//---------------------------------------------------------- cmpr_transposition
// Returns 1 upon finding a transposition.
// It only works with strings that have the same length.
int cmpr_transposition(char *s1, char *s2) {
  if (s1 == NULL || s2 == NULL) return 0;                                 //-->
  int n = strlen(s1);
  if (strlen(s2) != n) return 0;                                          //-->
  for (int i = 1; i < n; i++) {
    if (s2[i] != s1[i] && s2[i] == s1[i - 1] && s2[i - 1] == s1[i]) {
      return 1;                                                           //-->
      }
    }
  return 0;
  } // cmpr_transposition

//------------------------------------------------------------------- print_row
void print_row(int *i, int max) {
  for (int j = 0; j <= max; j++) printf("%2d", i[j]);
  printf("\n");
  } // print_row
