#include <stdlib.h>
#include <stdio.h>
#include <time.h>

unsigned long fact_r(int n) {
  unsigned long fact = 1;
  if (n > 1) fact = fact_r(n - 1) * n;
  return fact;
  }

unsigned long fact_i(int n) {
  unsigned long fact = 1;
  while (n >= 2) fact *= n--;
  return fact;
  }

double how_long(int n, int n_meas, unsigned long (*f)(int)) {
  unsigned long fact;
  unsigned long t = (unsigned long)clock();
  for (int k = 0; k < n_meas; k++) fact = (*f)(n);
  t = (unsigned int)clock() - t;
  double td = (double)t / n_meas;
  printf("%s: %lu\n", (f == &fact_r) ? "Recursion" : "Iteration", fact);
  return td;
  }

//------------------------------------------------------------------------ main
int main(void) {
  double t_r = how_long(20, 10000000, &fact_r);
  double t_i = how_long(20, 10000000, &fact_i);
  printf("r=%f i=%f r/i=%f\n", t_r, t_i, t_r / t_i);
  return EXIT_SUCCESS;
  }
