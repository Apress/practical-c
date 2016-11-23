/* itg.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef ITG
#define ITG
#include <stdlib.h>

int itg_boole(double f(double), double x0, double x1, int n, double *res,
    double *fv);
int itg_fill(double *x, double *y, int n, double f(double));
int itg_minmax(double f(double), double x0, double x1, double d, double *min,
    double *max);
int itg_monte(double f(double), double x0, double x1, double d, double *res);
int itg_opt(double f(double), double x0, double x1, double d, double *res);
int itg_prism(double g(double, double), int in(double, double),
    double x0, double x1, double y0, double y1, int n, double *res);
int itg_simpson(double f(double), double x0, double x1, int n, double *res);
int itg_trapezoid(double f(double), double x0, double x1, int n, double *res);
int itg_trapezoid_xy(double *x, double *y, int n, double *res);

#endif
