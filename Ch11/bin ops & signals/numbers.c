/* numbers.c
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
#include <math.h>
#include "numbers.h"

//------------------------------------------------------------------ num_binfmt
void num_binfmt(void *p, int n, char *s, int space) {
  unsigned char c;
  while (n > 0) {
    c = *((unsigned char *)p++);
    for (int nb = 0; nb < 8 && n > 0; nb++) {
      *s++ = (c & 128) ? '1' : '0';
      c <<= 1;
      n--;
      }
    if (space) *s++ = ' ';
    }
  *s = '\0';
  } // num_binfmt

//------------------------------------------------------------------ num_binprt
void num_binprt(void *p, int n, int space, int line) {
  unsigned char c;
  while (n > 0) {
    c = *((unsigned char *)p++);
    for (int nb = 0; nb < 8 && n > 0; nb++) {
      printf("%c", (c & 128) ? '1' : '0');
      c <<= 1;
      n--;
      }
    if (space) printf(" ");
    }
  if (line) printf("\n");
  } // num_binprt

//------------------------------------------------------------------ num_dblcmp
int num_dblcmp(double a, double b, unsigned int n_bits) {
  if (n_bits > DBL_MANT_DIG - 1) n_bits = DBL_MANT_DIG - 1; //#
  if (a == b) return 0;                                                   //-->
  union ieee754_double *aa = (union ieee754_double *)&a; //#
  union ieee754_double *bb = (union ieee754_double *)&b; //#

  // Compare the signs.
  char a_sign = (char)aa->ieee.negative;
  char b_sign = (char)bb->ieee.negative;
  if (a_sign != b_sign) return b_sign - a_sign;                           //-->
  if (a == 0) return ((b_sign) ? 1 : -1);                                 //-->
  if (b == 0) return ((a_sign) ? -1 : 1);                                 //-->

  // Compare the exponents.
  int a_exp = (char)aa->ieee.exponent - 1023; //#
  int b_exp = (char)bb->ieee.exponent - 1023; //#
  if (a_exp != b_exp) {
    int ret = (a_exp > b_exp) ? 1 : -1;
    return (a_sign) ? -ret : ret;                                         //-->
    }

  // Compare the mantissas.
  unsigned long a_mant = (unsigned int)aa->ieee.mantissa1
                       | (unsigned long)aa->ieee.mantissa0 << 32
                       ; //#
  unsigned long b_mant = (unsigned int)bb->ieee.mantissa1
                       | (unsigned long)bb->ieee.mantissa0 << 32
                       ; //#
  int n_shift = (int)sizeof(unsigned int) * 8 - DBL_MANT_DIG + 32 + 1; //#
  a_mant <<= n_shift;
  b_mant <<= n_shift;
# define MASK 0x8000000000000000 //# 2^63
  for (int k = 0; k < n_bits; k++) {
    if ((a_mant & MASK) != (b_mant & MASK)) {
      int ret = (a_mant & MASK) ? 1 : -1;
      return (a_sign) ? -ret : ret;                                       //-->
      }
    a_mant <<= 1;
    b_mant <<= 1;
    }
# undef MASK
  return 0;
  } // num_dblcmp

//------------------------------------------------------------------ num_fltcmp
int num_fltcmp(float a, float b, unsigned int n_bits) {
  if (n_bits > FLT_MANT_DIG - 1) n_bits = FLT_MANT_DIG - 1;
  if (a == b) return 0;                                                   //-->
  union ieee754_float *aa = (union ieee754_float *)&a;
  union ieee754_float *bb = (union ieee754_float *)&b;

  // Compare the signs.
  char a_sign = (char)aa->ieee.negative;
  char b_sign = (char)bb->ieee.negative;
  if (a_sign != b_sign) return b_sign - a_sign;                           //-->
  if (a == 0) return ((b_sign) ? 1 : -1);                                 //-->
  if (b == 0) return ((a_sign) ? -1 : 1);                                 //-->

  // Compare the exponents.
  char a_exp = (char)aa->ieee.exponent - 127;
  char b_exp = (char)bb->ieee.exponent - 127;
  if (a_exp != b_exp) {
    int ret = (a_exp > b_exp) ? 1 : -1;
    return (a_sign) ? -ret : ret;                                         //-->
    }

  // Compare the mantissas.
  int n_shift = (int)sizeof(unsigned int) * 8 - FLT_MANT_DIG + 1;
  unsigned int a_mant = (unsigned int)aa->ieee.mantissa << n_shift;
  unsigned int b_mant = (unsigned int)bb->ieee.mantissa << n_shift;
# define MASK 0x80000000 // 2^31
  for (int k = 0; k < n_bits; k++) {
    if ((a_mant & MASK) != (b_mant & MASK)) {
      int ret = (a_mant & MASK) ? 1 : -1;
      return (a_sign) ? -ret : ret;                                       //-->
      }
    a_mant <<= 1;
    b_mant <<= 1;
    }
# undef MASK
  return 0;
  } // num_fltcmp

//------------------------------------------------------------------ num_fltequ
// Returns 1 when the numbers match to the required number of mantissa bits.
int num_fltequ(float a, float b, unsigned int n_bits) {
  if (n_bits > FLT_MANT_DIG - 1) n_bits = FLT_MANT_DIG - 1;
  if (a == b) return 1;                                                   //-->
  union ieee754_float *aa = (union ieee754_float *)&a;
  union ieee754_float *bb = (union ieee754_float *)&b;

  // Compare the signs.
  char a_sign = (char)aa->ieee.negative;
  char b_sign = (char)bb->ieee.negative;
  if (a_sign != b_sign) return 0;                                         //-->

  // Compare the exponents.
  char a_exp = (char)aa->ieee.exponent - 127;
  char b_exp = (char)bb->ieee.exponent - 127;
  if (a_exp != b_exp) return 0;                                           //-->

  // Compare the mantissas.
  int n_shift = (int)sizeof(unsigned int) * 8 - FLT_MANT_DIG + 1;
  unsigned int a_mant = (unsigned int)aa->ieee.mantissa << n_shift;
  unsigned int b_mant = (unsigned int)bb->ieee.mantissa << n_shift;
# define MASK 0x80000000 // 2^31
  for (int k = 0; k < n_bits; k++) {
    if ((a_mant & MASK) != (b_mant & MASK)) return 0;                     //-->
    a_mant <<= 1;
    b_mant <<= 1;
    }
# undef MASK
  return 1;
  } // num_fltequ

//----------------------------------------------------------------- num_ldblcmp
int num_ldblcmp(long double a, long double b, int n_bits) {
  if (n_bits > LDBL_MANT_DIG - 1) n_bits = LDBL_MANT_DIG - 1; //#
  if (a == b) return 0;                                                   //-->
  union ieee854_long_double *aa = (union ieee854_long_double *)&a; //#
  union ieee854_long_double *bb = (union ieee854_long_double *)&b; //#

  // Compare the signs.
  char a_sign = (char)aa->ieee.negative;
  char b_sign = (char)bb->ieee.negative;
  if (a_sign != b_sign) return b_sign - a_sign;                           //-->
  if (a == 0) return ((b_sign) ? 1 : -1);                                 //-->
  if (b == 0) return ((a_sign) ? -1 : 1);                                 //-->

  // Compare the exponents.
  int a_exp = (char)aa->ieee.exponent - 32767;// - 16383; //#
  int b_exp = (char)bb->ieee.exponent - 32767; // - 16383; //#
  if (a_exp != b_exp) {
    int ret = (a_exp > b_exp) ? 1 : -1;
    return (a_sign) ? -ret : ret;                                         //-->
    }

  // Compare the mantissas.
  unsigned long a_mant = (unsigned int)aa->ieee.mantissa1
                       | (unsigned long)aa->ieee.mantissa0 << 32
                       ; //#
  unsigned long b_mant = (unsigned int)bb->ieee.mantissa1
                       | (unsigned long)bb->ieee.mantissa0 << 32
                       ; //#
  a_mant <<= 1;
  b_mant <<= 1;
# define MASK 0x8000000000000000 //# 2^63
  for (int k = 0; k < n_bits; k++) {
    if ((a_mant & MASK) != (b_mant & MASK)) {
      int ret = (a_mant & MASK) ? 1 : -1;
      return (a_sign) ? -ret : ret;                                       //-->
      }
    a_mant <<= 1;
    b_mant <<= 1;
    }
# undef MASK
  return 0;
  } // num_ldblcmp

//----------------------------------------------------------- num_to_big_endian
void num_to_big_endian(void *in, void *out, int n_bytes) {
  unsigned char *from = in;
  unsigned char *to = out + n_bytes - 1;
  for (int k = 0; k < n_bytes; k++) *to-- = *from++;
  } // num_to_big_endian
