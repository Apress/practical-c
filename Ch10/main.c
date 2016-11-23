/* Integral: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "itg.h"
#include "f1.h"

#include <limits.h>
#include "dar.h"

#define N 11

double f2(double x) {
  double ret = exp(-0.1*log(x));
  return (ret == INFINITY) ? 0 : ret;
  }

double g1(double x, double y) {
  return 0.2 * cos(0.24 * x) + 0.2 * cos(0.16 * x) + sin(x * 0.04 + y);
  }

#define X0 (0.)
#define X1 (50.)
#define Y0 (0.)
#define Y1 (1.)
int in_rect(double x, double y) {
  return x >= X0 && x <= X1 && y >= Y0 && y <= Y1;
  }
int in_triang(double x, double y) {
//  return x >= X0 && x <= X1 && y >= Y0 && y <= (x - X0)/(X1 - X0);
  return y >= Y0 && y <= Y1 && x >= (X1 - X0)*(y - Y0) + X0 && x <= X1;
  }
int in_left_half(double x, double y) {
  return x >= X0 && x <= X1*0.5 && y >= Y0 && y <= Y1;
  }

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
/*
  //------ Calculate the values of f1(x) for x = 0..50 with steps of of 5.
  printf("----- Calculate 50 values of f1\n");
  double x[N] = {};
  double y[N] = {};
  for (int k = 0; k < N; k++) x[k] = k * 5;
  if (itg_fill(x, y, N, f1)) for (int k = 0; k < N; k++){
    printf("%4.1f %14.9f\n", x[k], y[k]);
    }

  //------ Calculate the integral of f1(x) known in tabular form.
  printf("\n----- Calculate integral of f1 in tabular form\n");
  double value;
  if (itg_trapezoid_xy(x, y, N, &value)) printf("%.8f\n", value);

  //------ Calculate the integral of f1(x) with the trapezoidal rule
  printf("\n----- Calculate integral of f1 with trapezoidal rule\n");
  if (itg_trapezoid(f1, 0, 50, 15772, &value)) printf("%.8f\n", value);

  //------ Calculate the integral of f1(x) with Simpson's rule
  printf("\n----- Calculate integral of f1 with Simpson's rule\n");
  if (itg_simpson(f1, 0, 50, 265, &value)) printf("%.8f\n", value);

  //------ Calculate the integral of f1(x) with Boole's rule
  printf("\n----- Calculate integral of f1 with Boole's rule\n");
  for (int n = 13; n < 90; n += 4) {
    int n5 = itg_boole(f1, 0, 50, n, &value, NULL);
    if (n5) printf("%d %.8f\n", n5, value);
    }

  //------ Calculate the integral of f1(x) with a given minimum precision
  printf("\n----- Calculate integral of f1 with a given minimum precision\n");
  double delta = 0.1;
  printf("#  %-10s   n  %-11s  %s\n", "req delta", "value", "err");
  for (int k = 1; k < 9; k++) {
    int n5 = itg_opt(f1, 0, 50, delta, &value);
    if (n5) printf("%d%12.8f%4d%13.8f%12.8f\n", k, delta, n5, value,
        (value - 25.78955371) / 25.78955371);
    delta *= 0.1;
    }
*/

/*
  //------ Calculate the integral of f2(x) with a given minimum precision
  // It only works if you remove the 16384 limit on ng in itg_opt().
  printf("\n----- Calculate integral of f2 with a given minimum precision\n");
  delta = 0.1;
  printf("#  %-10s         n  %s\n", "req delta", "value");
  for (int k = 1; k < 9; k++) {
    int n5 = itg_opt(f2, 0, 1, delta, &value);
    if (n5) printf("%d%12.8f%10d%12.8f\n", k, delta, n5, value);
    delta *= 0.1;
    }
*/

/*
  //------ Calculate the minimum and maximum of f1(x) and f2(x)
  printf("\n----- Calculate the minimum and maximum of f1 and f2\n");
  double min = 0;
  double max = 0;
  printf("      n     minimum     maximum\n");
  int n = itg_minmax(f1, 0, 50, 0.01, &min, &max);
  printf("f1: %d%12.5f%12.5f\n", n, min, max);
  n = itg_minmax(f2, 0, 50, 0.01, &min, &max);
  printf("f2: %d%12.5f%12.5f\n", n, min, max);

  //------ Calculate the integral of f1(x) with the Monte Carlo method
  printf("\n----- Calculate the integral of f1(x) with Monte Carlo\n");
  delta = 0.1;
  value = 0;
  printf("#  %-10s         n  %-11s  %s\n", "req delta", "value", "err");
  for (int k = 1; k < 8; k++) {
    printf("%d", k);
    n = itg_monte(f1, 0, 50, delta, &value);
//    printf("%d%12.8f%10d%13.8f%12.8f\n", k, delta, n, value,
//        (value - 25.78955371) / 25.78955371);
    delta *= 0.1;
    }
*/

  //------ Calculate the integral of g1(x,y)
  printf("\n----- Calculate the integral of g1\n");
  double integral = 0;
  int n = 10;
  double err = 0;
  int k = 0;
  printf("   #     n  %-11s  %-11s\n", "result", "err");
  do {
    k++;
    n += n;
    if (itg_prismx(g1, in_triang, X0, X1, Y0, Y1, n, &integral)) {
//      err = (integral - 41.03076380) / 41.03076380; // rectangle
//      err = (integral - 18.1622642) / 18.1622642;   // left half
      err = (integral - 20.98016139) / 20.98016139;   // triangle
      printf("%4d%6d  %11.8f  %11.8f\n", k, n, integral, err);
      }
    else {
      err = -1;
      }
    } while (fabs(err) > 0.00000001);

	return EXIT_SUCCESS;
  } // main
