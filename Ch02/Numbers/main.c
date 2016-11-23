/* Numbers: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <ieee754.h>
#include "numbers.h"

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
/*
  printf("Two zeroes.\n");
  printf(" 0%3d\n", num_fltcmp(0, 0, 4));

  printf("\nCheck the sign against zeroes.\n");
  printf(" 1%3d\n", num_fltcmp(1.111, 0, 4));
  printf("-1%3d\n", num_fltcmp(-1.111, 0, 4));
  printf("-1%3d\n", num_fltcmp(0, 1.111, 4));
  printf(" 1%3d\n", num_fltcmp(0, -1.111, 4));

  printf("\nCheck the signs.\n");
  printf(" 1%3d\n", num_fltcmp(1, -1, 4));
  printf("-1%3d\n", num_fltcmp(-1, 1, 4));

  printf("\nCheck the positive and negative exponents.\n");
  printf("-1%3d\n", num_fltcmp(10, 100, 4));
  printf(" 1%3d\n", num_fltcmp(100, 10, 4));
  printf(" 1%3d\n", num_fltcmp(0.1, 0.01, 4));
  printf("-1%3d\n", num_fltcmp(0.01, 0.1, 4));
*/

/*
  float f1;
  union ieee754_float *ff1 = (union ieee754_float *)&f1;
  f1 = 1e-38;
  char exp = (char)ff1->ieee.exponent - 127;
  printf("%d\n", exp);
  f1 = 1;
  exp = (char)ff1->ieee.exponent - 127;
  printf("%d\n", exp);
  f1 = 3e+38;
  exp = (char)ff1->ieee.exponent - 127;
  printf("%d\n", exp);

  double d1;
  union ieee754_double *dd1 = (union ieee754_double *)&d1;
  d1 = 2e-308;
  int exp1 = (int)dd1->ieee.exponent - 1023;
  printf("%d\n", exp1);
  d1 = 1;
  exp1 = (int)dd1->ieee.exponent - 1023;
  printf("%d\n", exp1);
  d1 = 1e+308;
  exp1 = (int)dd1->ieee.exponent - 1023;
  printf("%d\n", exp1);

  long double ld1;
  union ieee854_long_double *lld1 = (union ieee854_long_double *)&ld1;
  ld1 = 3e-4932L;
  int exp2 = (int)lld1->ieee.exponent - 16383;
  printf("%d\n", exp2);
  ld1 = 1;
  exp2 = (int)lld1->ieee.exponent - 16383;
  printf("%d\n", exp2);
  ld1 = 1e+4932L;
  exp2 = (int)lld1->ieee.exponent - 16383;
  printf("%d\n", exp2);
*/

/*
  long double ld = 1;
  unsigned char *c = (unsigned char *)&ld;
  for (int i = 1; i <= 6; i++) c[sizeof(ld) - i] = 0xff;
  printf("%Lf\n", ld);
  for (int i = 0; i < sizeof(ld); i++) printf("%02x", c[i]);
  printf("\n");
  long double ld2;
  c = (unsigned char *)&ld2;
  for (int i = 1; i < sizeof(ld2); i++) c[i] = i;
  ld2 = ld;
  printf("%Lf\n", ld2);
  for (int i = 0; i < sizeof(ld2); i++) printf("%02x", c[i]);
  printf("\n");
*/

/*
  printf(" 0%3d\n", num_fltcmp(1., 1.1, 1));
  printf(" 0%3d\n", num_fltcmp(1.1, 1., 1));
  printf(" 1%3d\n", num_fltcmp(1., 0.9, 1));
  printf("-1%3d\n", num_fltcmp(0.9, 1., 1));

  printf("%3d\n", num_fltcmp(1.2221, 1.222, 4));
  printf("%3d\n", num_fltcmp(1.22221, 1.2222, 4));
  printf("%3d\n", num_fltcmp(1.2222, 1.22219, 4));

  printf("%3d\n", num_fltcmp(1.221, 1.22, 5));
  printf("%3d\n", num_fltcmp(1.2221, 1.222, 5));
  printf("%3d\n", num_fltcmp(1.22221, 1.2222, 5));
  printf("%3d\n", num_fltcmp(1.2222, 1.22219, 5));
*/

  int N = 3;
  srand(123456789);
/*
  float max_x = 10.0;
  float d[] = {
      -0.1, -0.01, -0.001, -0.0001, -0.00001, -0.000001, 0,
       0.000001,  0.00001,  0.0001,  0.001,  0.01,  0.1
      };
  int nd = sizeof(d) / sizeof(float);
  for (int k = 0; k < N; k++) {
    float x = (float)rand() / RAND_MAX * max_x;
    printf("\n%9.7f  ", x);
    for (int i = 1; i < FLT_MANT_DIG; i++) printf("%2d", i % 10);
    printf("\n");
    for (int j = 0; j < nd; j++) {
      printf("%10f:", d[j]);
      for (unsigned int i = 1; i < FLT_MANT_DIG; i++) {
        int res = num_fltcmp(x, x + d[j], i);
        if (res) printf(" %c", (res > 0) ? '+' : '-');
        else printf("  ");
        }
      printf("\n");
      }
    }
*/
/*
  double dmax_x = 10.0;
  double dd[(DBL_DIG << 1) + 1] = {-0.1};
  for (int k = 1; k < DBL_DIG; k++) {
    dd[k] = dd[k -1] / 10;
    dd[(DBL_DIG << 1) - k] = -dd[k];
    }
  dd[(DBL_DIG << 1)] = -dd[0];
  int dnd = (DBL_DIG << 1) + 1;
//  for (int k = 0; k < dnd; k++) printf("%g\n", dd[k]);

  int first = 1;
  for (int k = 0; k < N; k++) {
    double x = (double)rand() / RAND_MAX * dmax_x;
if (0 && first) {
  num_binprt(&x, 64, 1, 1);
  double x_big = 0;
  num_to_big_endian(&x, &x_big, 8);
  num_binprt(&x_big, 64, 0, 1);
  first = 0;
  }
    printf("\n%17.15f   ", x);
    for (int i = 1; i < DBL_MANT_DIG; i++) printf("%2d", i / 10);
    printf("\n                    ");
    for (int i = 1; i < DBL_MANT_DIG; i++) printf("%2d", i % 10);
    printf("\n");
    for (int j = 0; j < dnd; j++) {
      printf("%19.15f:", dd[j]);
      for (unsigned int i = 1; i < DBL_MANT_DIG; i++) {
        int res = num_dblcmp(x, x + dd[j], i);
        if (res) printf(" %c", (res > 0) ? '+' : '-');
        else printf("  ");
        }
      printf("\n");
      }
    }
*/

  long double ldmax_x = 10.0;
  long double ldd[(LDBL_DIG << 1) + 1] = {-0.1};
  for (int k = 1; k < LDBL_DIG; k++) {
    ldd[k] = ldd[k -1] / 10;
    ldd[(LDBL_DIG << 1) - k] = -ldd[k];
    }
  ldd[LDBL_DIG << 1] = -ldd[0];
  int ldnd = (LDBL_DIG << 1) + 1;
//  for (int k = 0; k < ldnd; k++) printf("%Lg\n", ldd[k]);

  for (int k = 0; k < N; k++) {
    long double lx = (long double)rand() / RAND_MAX * ldmax_x;
    printf("\n%20.18Lf  ", lx);
    for (int i = 1; i < LDBL_MANT_DIG; i++) printf("%2d", i / 10);
    printf("\n                      ");
    for (int i = 1; i < LDBL_MANT_DIG; i++) printf("%2d", i % 10);
    printf("\n");
    for (int j = 0; j < ldnd; j++) {
      printf("%21.18Lf:", ldd[j]);
      for (unsigned int i = 1; i < LDBL_MANT_DIG; i++) {
        int res = num_ldblcmp(lx, lx + ldd[j], i);
        if (res) printf(" %c", (res > 0) ? '+' : '-');
        else printf("  ");
        }
      printf("\n");
      }
    }

	return EXIT_SUCCESS;
  }
