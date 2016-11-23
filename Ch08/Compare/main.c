/* Compare: main.c
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

// Comparing structures;
typedef struct {
  unsigned int number;
  unsigned int version;
  char a_string[10];
  } part_t;
//
typedef struct {
  unsigned int number;
  unsigned int version;
  void *a_void_ptr;
  } part_new_t;
//
int part_cmp(part_new_t *p1, part_new_t *p2) {
  unsigned long l1 = (((unsigned long)(*p1).number) << 32) | (*p1).version;
  unsigned long l2 = (((unsigned long)(*p2).number) << 32) | (*p2).version;
  return (l1 == l2) ? 0 : (l1 > l2) ? 1 : -1;
  }

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {

/*
{ // Structures with holes.
  struct {
    char cc[2];
    union {
      int i;
      char c;
      } u;
    } st;
  char *ar = (char *)&st;
  for (int i = 0; i < sizeof(st); i++) ar[i] = -(i + 1);
  for (int i = 0; i < sizeof(st); i++) printf("%3d", ar[i]);
  printf("\n");
  st.cc[0] = 1;
  st.cc[1] = 2;
  st.u.c = 3;
  for (int i = 0; i < sizeof(st); i++) printf("%3d", ar[i]);
  printf("\n");
  st.u.i = 0x12345432;
  st.u.c = 3;
  for (int i = 0; i < sizeof(st); i++) printf("%3d", ar[i]);
  printf("\n");
  }
*/

/*
{ // Comparing structures.
  part_t old_part;
  part_new_t new_part;
  char c[3] = {'<', '=', '>'};
  for (int k1 = 0; k1 < 2; k1++) {
    new_part.number = k1;
    for (int k2 = 0; k2 < 2; k2++) {
      new_part.version = k2;
      for (int j1 = 0; j1 < 2; j1++) {
        old_part.number = j1;
        for (int j2 = 0; j2 < 2; j2++) {
          old_part.version = j2;
          printf("(%d %d) %c (%d %d)\n",
              new_part.number, new_part.version,
              c[part_cmp(&new_part, (part_new_t *)&old_part) + 1],
              old_part.number, old_part.version
              );
          }
        }
      }
    }
  }

*/

/*
{ // Levenshtein distance.
  printf("%d \"post\" \"must\"\n", cmpr_levenshtein("post", "must"));
  printf("%d \"post\" \"post\"\n", cmpr_levenshtein("post", "post"));
  printf("%d \"pst\" \"post\"\n", cmpr_levenshtein("pst", "post"));
  printf("%d \"copbok\" \"copybook\"\n",
      cmpr_levenshtein("copbok", "copybook"));
  }
*/

{ // Character transposition.
  char *ss[] = {"copybook", "ocpybook", "copyboko", "copbyook", "copy"};
  char *s1 = ss[0];
  int ns = sizeof(ss) / sizeof(char *);
  for (int ks = 0; ks < ns; ks++) {
    char *s2 = ss[ks];
    printf("\"%s\" \"%s\": %d\n", s1, s2, cmpr_transposition(s1, s2));
    }
  }

  return EXIT_SUCCESS;
  }
