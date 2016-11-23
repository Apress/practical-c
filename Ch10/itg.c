/* itg.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "itg.h"
#include "dar.h"

//------------------------------------------------------------------- itg_boole
// Returns the number of points actually used if successful.
int itg_boole(double f(double), double x0, double x1, int n, double *res,
    double *fv) {
  int n5 = 0;
  if (!f) fprintf(stderr, "itg_boole: function is NULL\n");
  else if (x0 >= x1) fprintf(stderr, "itg_boole: x-range non-positive\n");
  else if (n < 5) fprintf(stderr, "itg_boole: less than 5 points\n");
  else if (!res) fprintf(stderr, "itg_boole: result address is NULL\n");
  else {
    n5 = (n - 1) / 4 * 4 + 1;
    double h = (x1 - x0) / (n5 - 1);
    double val = 0;
    double ff[5] = {};
    if (fv) ff[0] = fv[0];
    else ff[0] = f(x0);
    for (int k = 0; k < n5 - 1; k += 4) {
      ff[1] = f(x0 + h * (k + 1));
      ff[3] = f(x0 + h * (k + 3));
      if (fv) {
        fv[k + 1] = ff[1];
        fv[k + 3] = ff[3];
        ff[2] = fv[k + 2];
        ff[4] = fv[k + 4];
        }
      else {
        ff[2] = f(x0 + h * (k + 2));
        ff[4] = f(x0 + h * (k + 4));
        }
      val += 7 * (ff[0] + ff[4]) + 12 * ff[2] + 32 * (ff[1] + ff[3]);
      ff[0] = ff[4];
      }
    *res = val * h / 22.5;
    }
  return n5;
  } // itg_boole

//-------------------------------------------------------------------- itg_fill
// Returns true if successful.
int itg_fill(double *x, double *y, int n, double f(double)) {
  int ret = 0;
  if (!x) fprintf(stderr, "itg_fill: x-array address is NULL\n");
  if (!y) fprintf(stderr, "itg_fill: y-array address is NULL\n");
  else if (n < 2) fprintf(stderr, "itg_fill: less than 2 points\n");
  else if (!f) fprintf(stderr, "itg_fill: function is NULL\n");
  else {
    ret = 1;
    for (int k = 0; k < n; k++) y[k] = f(x[k]);
    }
  return ret;
  } // itg_fill

//------------------------------------------------------------------ itg_integr
// Returns the number of points used if successful.
int itg_integr(double f(double), double x0, double x1, double d, double *res) {
  int ng = 0;
  int n = 0;
  double old_res = 0;
  double new_res = 0;
  do {
    old_res = new_res;
    ng++;
    n = itg_boole(f, x0, x1, ng * 4 + 1, &new_res, NULL);
    } while (n && fabs((new_res - old_res) / new_res) > d);
  if (n) *res = new_res;
  return n;
  } // itg_integr

//------------------------------------------------------------------ itg_minmax
// Returns the number of points used.
int itg_minmax(double f(double), double x0, double x1, double d, double *min,
    double *max) {
  int n = 0;
  if (!f) fprintf(stderr, "itg_minmax: function is NULL\n");
  else if (x0 >= x1) fprintf(stderr, "itg_minmax: x-range non-positive\n");
  else if (!min) fprintf(stderr, "itg_minmax: min address is NULL\n");
  else if (!max) fprintf(stderr, "itg_minmax: max address is NULL\n");
  else {
    int n_incr = 50;
    double old_min = 0;
    double new_min = f((x0 + x1) * 0.5);
    double old_max = 0;
    double new_max = new_min;
    double interval = x1 - x0;
    unsigned short int rand_stat[3] = {1, 11, 111};
    do {
      old_min = new_min;
      old_max = new_max;
      n += n_incr;
      for (int k = 0; k < n; k++) {
        double val = f(interval * erand48(rand_stat) + x0);
        if (val > new_max) new_max = val;
        if (val < new_min) new_min = val;
        }
      } while (fabs((new_min - old_min) / new_min) > d ||
               fabs((new_max - old_max) / new_max) > d
               );
    *min = new_min;
    *max = new_max;
    }
  return n;
  } // itg_minmax

//------------------------------------------------------------------- itg_monte
// Returns the number of points used.
int itg_monte(double f(double), double x0, double x1, double d, double *res) {
  int n = 13;
  int n_incr = 111111;
  int n_tot = 0;
  if (!f) fprintf(stderr, "itg_monte: function is NULL\n");
  else if (x0 >= x1) fprintf(stderr, "itg_monte: x-range non-positive\n");
  else if (!res) fprintf(stderr, "itg_monte: result address is NULL\n");
  else {
    double min = 0;
    double max = 0;
    (void)itg_minmax(f, x0, x1, 0.01, &min, &max);
    min -= 0.01 * fabs(min);
    max += 0.01 * fabs(max);
    double xx = x1 - x0;
    double yy = max - min;
    double area = xx * yy;
    double bottom = xx * min;
    double old_res = 0;
    double new_res = bottom + area * 0.5;
    int n_yes = 0;
    unsigned short int rand_stat[3] = {1, 456, 789};
    do {
      old_res = new_res;
      n += n_incr;
      double h = xx / n;
      for (int k = 0; k < n; k++) {
        double dk = k + erand48(rand_stat) - 0.5;
        if (dk < 0) dk = 0;
        else if (dk > n - 1) dk = n - 1;
        double x = x0 + dk * h;
        double y = yy * erand48(rand_stat) + min;
        double val = f(x);
        if (y <= val) n_yes++;
        if (val < min) printf("%.8f < %.8f at x=%.8f\n", val, min, x);
        if (val > max) printf("%.8f > %.8f at x=%.8f\n", val, max, x);
        }
      n_tot += n;
      new_res = bottom + n_yes * area / n_tot;
      } while (fabs((new_res - old_res) / new_res) > d);
    printf("%12.8f%10d%13.8f%15.11f%12.8f\n", d, n_tot, new_res,
        (new_res - old_res) / old_res,
        (new_res - 25.78955371) / 25.78955371);
    *res = new_res;
    }
  return n_tot;
  } // itg_monte

//--------------------------------------------------------------------- itg_opt
// Returns the number of points used if successful.
int itg_opt(double f(double), double x0, double x1, double d, double *res) {
  int n = 5;
  DAR_setup;
  int ng = 1;
  double old_res = 0;
  double new_res = 0;
  double *fv = DAR_new(new_res, n);
  double h = (x1 - x0) / (n - 1);
  for (int k = 0; k < n; k++) fv[k] = f(x0 + h * k);
//for (int k = 0; k < n; k++) printf("%3d%13.8f\n", k, fv[k]);
  do {
    old_res = new_res;
    ng += ng;
    n = ng * 4 + 1;
    DAR_extend(fv, n);
    for (int k = n - 1; k > 1; k -= 2) {
      int k0 = k / 2;
      fv[k] = fv[k0];
      fv[k0] = 0;
      }
    n = itg_boole(f, x0, x1, n, &new_res, fv);
//for (int k = 0; k < n; k++) printf("%3d%13.8f\n", k, fv[k]);
    } while (n && fabs((new_res - old_res) / new_res) > d && ng < 16384);
  if (n) *res = new_res;
  DAR_dismantle;
  return n;
  } // itg_opt

//------------------------------------------------------------------- itg_prism
// Returns true if successful.
int itg_prism(double g(double, double), int in(double, double),
    double x0, double x1, double y0, double y1, int n, double *res) {
  int ret = 0;
  double hx = (x1 - x0) / (n - 1);
  double hy = (y1 - y0) / (n - 1);
  if (!g) fprintf(stderr, "itg_prism: function is NULL\n");
  if (!in) fprintf(stderr, "itg_prism: domain is NULL\n");
  else if (n < 2) fprintf(stderr, "itg_prism: less than 2 points\n");
  else if (hx <= 0) fprintf(stderr, "itg_prism: x-range non-positive\n");
  else if (hy <= 0) fprintf(stderr, "itg_prism: y-range non-positive\n");
  else if (!res) fprintf(stderr, "itg_prism: result address is NULL\n");
  else {
    ret = 1;
    double val = 0;
    for (int k = 0; k < n - 1; k++) {
      double x = x0 + hx * k;
      for (int j = 0; j < n - 1; j++) {
        double y = y0 + hy * j;
        int inn[4] = {};
        inn[0] = in(x, y);
        inn[1] = in(x + hx, y);
        inn[2] = in(x, y + hy);
        inn[3] = in(x + hx, y + hy);
        double vval = 0;
        switch (inn[0] + inn[1] + inn[2] + inn[3]) {
          case 4:
           val += g(x, y) + g(x + hx, y) + g(x, y + hy) + g(x + hx, y + hy);
           break;

          case 3:
            if (inn[0]) vval = g(x, y);
            if (inn[1]) vval += g(x + hx, y);
            if (inn[2]) vval += g(x, y + hy);
            if (inn[3]) vval += g(x + hx, y + hy);
            val += vval * 2 / 3;
            break;

          case 2:
            if (in(x + hx/2, y + hy/2) && !(inn[0]*inn[3]) &&
                !(inn[1]*inn[2])) {
              if (inn[0]) vval = g(x, y);
              if (inn[1]) vval += g(x + hx, y);
              if (inn[2]) vval += g(x, y + hy);
              if (inn[3]) vval += g(x + hx, y + hy);
              vval += g(x + hx/2, y + hy/2);
              val += vval / 3;
              }
            break;

          case 1:
            if (inn[0]) {
              if (in(x, y + hy/2) && in(x + hx/2, y)){
                vval = g(x, y);
                vval += g(x, y + hy/2);
                vval += g(x + hx/2, y);
                }
              }
            else if (inn[1]) {
              if(in(x + hx/2, y) && in(x + hx, y + hy/2)) {
                vval += g(x + hx, y);
                vval += g(x + hx/2, y);
                vval += g(x + hx, y + hy/2);
                }
              }
            else if (inn[2]) {
              if(in(x, y + hy/2) && in(x + hx/2, y + hy)) {
                vval += g(x, y + hy);
                vval += g(x, y + hy/2);
                vval += g(x + hx/2, y + hy);
                }
              }
            else if (in(x + hx/2, y + hy) && in(x + hx, y + hy/2)){
              vval += g(x + hx, y + hy);
              vval += g(x + hx/2, y + hy);
              vval += g(x +hx, y + hy/2);
              }
            val += vval / 6;
            break;

          default:
            break;
          }
        }
      }
    *res = val * hx * hy / 4;
    }
  return ret;
  } // itg_prism

//------------------------------------------------------------------ itg_prism0
// Returns true if successful.
int itg_prism0(double g(double, double), int in(double, double),
    double x0, double x1, double y0, double y1, int n, double *res) {
  int ret = 0;
  double hx = (x1 - x0) / (n - 1);
  double hy = (y1 - y0) / (n - 1);
  if (!g) fprintf(stderr, "itg_prism0: function is NULL\n");
  if (!in) fprintf(stderr, "itg_prism0: domain is NULL\n");
  else if (n < 2) fprintf(stderr, "itg_prism0: less than 2 points\n");
  else if (hx <= 0) fprintf(stderr, "itg_prism0: x-range non-positive\n");
  else if (hy <= 0) fprintf(stderr, "itg_prism0: y-range non-positive\n");
  else if (!res) fprintf(stderr, "itg_prism0: result address is NULL\n");
  else {
    ret = 1;
    double val = 0;
    for (int k = 0; k < n - 1; k++) {
      double x = x0 + hx * k;
      for (int j = 0; j < n - 1; j++) {
        double y = y0 + hy * j;
        if (in(x, y) && in(x + hx, y) && in(x, y + hy) && in(x + hx, y + hy)) {
          val += g(x, y) + g(x + hx, y) + g(x, y + hy) + g(x + hx, y + hy);
          }
        }
      }
    *res = val * hx * hy / 4;
    }
  return ret;
  } // itg_prism0

//------------------------------------------------------------------ itg_prismx
// Returns true if successful.
int itg_prismx(double g(double, double), int in(double, double),
    double x0, double x1, double y0, double y1, int n, double *res) {
  int ret = 0;
  double hx = (x1 - x0) / (n - 1);
  double hy = (y1 - y0) / (n - 1);
  if (!g) fprintf(stderr, "itg_prismx: function is NULL\n");
  if (!in) fprintf(stderr, "itg_prismx: domain is NULL\n");
  else if (n < 2) fprintf(stderr, "itg_prismx: less than 2 points\n");
  else if (hx <= 0) fprintf(stderr, "itg_prismx: x-range non-positive\n");
  else if (hy <= 0) fprintf(stderr, "itg_prismx: y-range non-positive\n");
  else if (!res) fprintf(stderr, "itg_prismx: result address is NULL\n");
  else {
    ret = 1;
    double val = 0;
    for (int k = 0; k < n - 1; k++) {
      double x = x0 + hx * (k + 0.5);
      for (int j = 0; j < n - 1; j++) {
        double y = y0 + hy * (j + 0.5);
        if (in(x, y) ) val += g(x, y);
        }
      }
    *res = val * hx * hy;
    }
  return ret;
  } // itg_prismx

//------------------------------------------------------------------- itg_quadr
// Returns true if successful.
int itg_quadr(double f(double), double x0, double x1, int n, double *res) {
  int ret = 0;
  if (!f) fprintf(stderr, "itg_quadr: function is NULL\n");
  else if (x0 >= x1) fprintf(stderr, "itg_quadr: x-range non-positive\n");
  else if (n < 2) fprintf(stderr, "itg_quadr: less than 2 points\n");
  else if (!res) fprintf(stderr, "itg_quadr: result address is NULL\n");
  else {
    ret = 1;
    double h = (x1 - x0) / (n - 1);
    double val = 0;
    int n5 = (n - 1) / 4 * 4 + 1;

    // Apply the 5-point rule.
    for (int k = 0; k < n5 - 1; k += 4) {
      val += 7 * (f(x0 + h * k)       + f(x0 + h * (k + 4)))
          + 32 * (f(x0 + h * (k + 1)) + f(x0 + h * (k + 3)))
          + 12 *  f(x0 + h * (k + 2))
          ;
      }
    val /= 22.5;

    // Apply one of the simpler rules if there are further points.
    switch (n - n5) {
      case 3: // 4-point rule
        val += (f(x1 - 3 * h) +  3 * (f(x1 - 2 * h) + f(x1 - h)) + f(x1))
            * 0.375;
        break;
      case 2: // 3-point rule
        val += (f(x1 - 2 * h) + 4 * f(x1 - h) + f(x1)) / 3;
        break;
      case 1: // 2-point rule
        val += (f(x1 - h) + f(x1)) * 0.5;
        break;
      default:
        break; // this is the 0 case;
      }

    // Complete the calculation.
    *res = val * h;
    }
  return ret;
  } // itg_quadr

//----------------------------------------------------------------- itg_simpson
// Returns true if successful.
int itg_simpson(double f(double), double x0, double x1, int n, double *res) {
  int ret = 0;
  if (!f) fprintf(stderr, "itg_simpson: function is NULL\n");
  else if (x0 >= x1) fprintf(stderr, "itg_simpson: x-range non-positive\n");
  else if (n < 2) fprintf(stderr, "itg_simpson: less than 2 points\n");
  else if (n % 2 == 0) fprintf(stderr, "itg_simpson: n must be odd\n");
  else if (!res) fprintf(stderr, "itg_simpson: result address is NULL\n");
  else {
    ret = 1;
    double h = (x1 - x0) / (n - 1);
    double val = 0;
    for (int k = 1; k < n - 3; k += 2) {
      val += (f(x0 + h * k) * 2 + f(x0 + h * (k + 1))) * 2;
      }
    val += f(x1 - h) * 4 + f(x0) + f(x1);
    *res = val * h / 3;
    }
  return ret;
  } // itg_simpson

//--------------------------------------------------------------- itg_trapezoid
// Returns true if successful.
int itg_trapezoid(double f(double), double x0, double x1, int n, double *res) {
  int ret = 0;
  if (!f) fprintf(stderr, "itg_trapezoid: function is NULL\n");
  else if (x0 >= x1) fprintf(stderr, "itg_trapezoid: x-range non-positive\n");
  else if (n < 2) fprintf(stderr, "itg_trapezoid: less than 2 points\n");
  else if (!res) fprintf(stderr, "itg_trapezoid: result address is NULL\n");
  else {
    ret = 1;
    double h = (x1 - x0) / (n - 1);
    double val = 0;
    for (int k = 1; k < n - 1; k++) val += f(x0 + h * k);
    val += 0.5 * (f(x0) + f(x1));
    *res = val * h;
    }
  return ret;
  } // itg_trapezoid

//------------------------------------------------------------ itg_trapezoid_xy
// Returns true if successful.
int itg_trapezoid_xy(double *x, double *y, int n, double *res) {
  int ret = 0;
  if (!x) fprintf(stderr, "itg_trapezoid_xy: x-array address is NULL\n");
  else if (!y) fprintf(stderr, "itg_trapezoid_xy: y-array address is NULL\n");
  else if (n < 2) fprintf(stderr, "itg_trapezoid_xy: less than 2 points\n");
  else if (!res) fprintf(stderr, "itg_trapezoid_xy: result address is NULL\n");
  else {
    ret = 1;
    double val = 0;
    for (int k = 0; k < n - 1; k++) {
      val += (y[k + 1] + y[k]) * (x[k + 1] - x[k]);
      }
    *res = val * 0.5;
    }
  return ret;
  } // itg_trapezoid_xy
