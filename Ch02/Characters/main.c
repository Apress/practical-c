/* Characters: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <unistd.h>

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
/*
  // Locales and currencies.
  struct lconv *lc;

  char *where = "en_US.UTF-8";
  setlocale(LC_MONETARY, where);
  lc = localeconv();
  printf ("%s: %s %s\n", where, lc->currency_symbol, lc->int_curr_symbol);

  where = "en_AU.UTF-8";
  setlocale(LC_MONETARY, where);
  lc = localeconv();
  printf ("%s: %s %s\n", where, lc->currency_symbol, lc->int_curr_symbol);

  where = "de_DE.UTF-8";
  setlocale(LC_MONETARY, where);
  lc = localeconv();
  printf ("%s: %s %s\n", where, lc->currency_symbol, lc->int_curr_symbol);
*/

/*
  // Printing UTF-8 characters in normal C-strings...
  char *s = "€ © 𐎫 ♥";
  printf("%zu \"%s\"\n", strlen(s), s);
  for (int k = 0; k < strlen(s); k++) printf("%02x ", (unsigned char)s[k]);
  printf("\n");

  // ... and as wide characters after cloning stdout.
  int stdout_fd = dup(1);
  FILE *stdout2 = fdopen(stdout_fd, "w"); // compile with -gnu99
  //
  setlocale(LC_CTYPE, "");
  wchar_t wc = L'€';
  fwprintf(stdout2, L"A wide character: %lc\n", wc);
  //
  fclose(stdout2);
  close(stdout_fd);
*/

/*
  // Setting and checking a stream orientation.
  wprintf(L"Stream orientation: %d\n", fwide(stdout, 3));
  wprintf(L"Stream orientation: %d\n", fwide(stdout, 0));
*/

  // Converting a string to a wide string one character at a time.
  char *s = "€ © 𐎫 ♥";
  setlocale(LC_CTYPE, "");
  wprintf(L"Normal string: %2d \"%s\"\nConversion\n", strlen(s), s);
  wchar_t ws[100] = {};
  size_t conv_size = 0;
  int next = 0;
  wchar_t wc;
  int k = 0;
  do {
    conv_size = mbtowc(&wc, &s[next], strlen(s) - next);
    if (conv_size) {
      wprintf(L"%4d: %d -> %zu '%lc'\n", next, (int)conv_size, sizeof(wc), wc);
      next += (int)conv_size;
      ws[k++] = wc;
      }
    } while (conv_size > 0);
  wprintf(L"Wide string: %zu \"%ls\"\n", wcslen(ws), ws);

  // Converting a string to a wide string
  size_t n = mbstowcs(ws, s, 100);
  wprintf(L"\nWide string: %zu \"%ls\"\n", n, ws);

  // Showing the encoding in wchar_t
  wprintf(L"\n");
  for (int k = 0; k < 7; k++) {
    for (int j = 0; j < 4; j++) {
      wprintf(L"%02x ", ((unsigned char *)ws)[k*4 + j]);
      }
    wprintf(L" '%lc'\n", ws[k]);
    }

  // Converting a wide character to multibyte.
  char airplane[5];
  size_t n_c = wctomb(airplane, L'✈');
  airplane[n_c] = '\0';
  wprintf(L"\nWide to multibyte char %zu: %lc -> %s\n", n_c, L'✈', airplane);

  // Converting a wide string to multibyte.
  char ss[100];
  n_c = wcstombs(ss, L"❰❱", 100);
  wprintf(L"\nWide to multibyte string %zu: %ls -> %s\n", n_c, L"❰❱", ss);

  return EXIT_SUCCESS;
  }
