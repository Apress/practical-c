/* numbers.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef NUM
#define NUM

void num_binfmt(void *p, int n, char *s, int space);
void num_binprt(void *p, int n, int space, int line);
int  num_dblcmp(double a, double b, unsigned int n_bits);
int  num_fltcmp(float a, float b, unsigned int n_bits);
int  num_fltequ(float a, float b, unsigned int n_bits);
int  num_ldblcmp(long double a, long double b, int n_bits);
void num_to_big_endian(void *in, void *out, int n_bytes);
#endif
