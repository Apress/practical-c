/* f1.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef F1_H
#define F1_H
#include <math.h>

#define F1(x) ({                                  \
  double _x = x;                                  \
  0.2*cos(0.24*_x) + 0.2*cos(0.16*_x) + 0.5;  \
  })

double f1(double x);

#endif
