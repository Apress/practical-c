/* str.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "str.h"

// Local functions.
char *find_last_c(char *s0, char c, size_t before);

//----------------------------------------------------------------- find_last_c
// We assume that checks have been made before invoking this function.
char *find_last_c(char *s0, char c, size_t before) {
  char *p = NULL;
  char *s = s0 + before - 1;
  while (p == NULL && s >= s0) {
    if (*s == c) p = s;
    else s--;
    }
/*
  // Alternative implementation. I don't like it because it temporarily
  // modifies the input string.
  char was = s0[before];
  s0[before] = '\0';
  p = strrchr(s0, c);
  s0[before] = was;
*/
  return p;
  } // find_last_c

//---------------------------------------------------------------- str_clean_up
void str_clean_up(Str *str) {
#if STR_LOG
  printf("=== str_clean_up: %p\n", str);
#endif
  if (str != NULL && str->s != NULL) {
    char *s0 = str->s;
    char *s1 = s0;
    while (*s0 > '\0' && *s0 <= ' ') s0++; // remove leading junk
    if (*s0 == '\0') {
      *s1++ = ' ';
      *s1 = '\0';
      }
    else {
      int space_set = 0;
      while (*s0 != '\0') {
        if (*s0 > ' ') {
          *s1++ = *s0;
          space_set = 0;
          }
        else if (!space_set) {
          *s1++ = ' ';
          space_set = 1;
          }
        s0++;
        }
      if (s1 > str->s) { // we did something
        *s1-- = '\0';
        if (*s1 == ' ') *s1 = '\0'; // remove the trailing space
        }
      }
    }
  } // str_clean_up

//------------------------------------------------------------------ str_concat
Str *str_concat(Str **str0, ...) {
#if STR_LOG
  printf("=== str_concat: %p ...\n", *str0);
#endif
  Str **str_stack_p = str0;
  if (*str_stack_p != NULL) {
    str_stack_p = (*str0)->stack_p;
    if (str_stack_p == NULL) {
      STR_setup;
      Str *e = STR_printf("str_concat: Invalid reference Str (%p)", str0);
      STR_crash(e->s);
      STR_dismantle;
      }
    }

  // First determine the number of strings and their total length.
  size_t tot_l = 0;
  va_list va;
  Str *str_k = NULL;
  va_start(va, str0);
  int n = 0;
  do {
    str_k = va_arg(va, Str *);
    if (str_k != NULL) {
      if (str_k->s != NULL) tot_l += strlen(str_k->s);
      n++;
      }
    } while (str_k != NULL);
  va_end(va);

  // Then create the output string and copy into it the input strings.
  Str *new_str = str_new(str_stack_p, tot_l, NULL, '\0');
  char *s = new_str->s;
  va_start(va, str0);
  for (int k = 0; k < n; k++) {
    str_k = va_arg(va, Str *);
    if (str_k->s != NULL) {
      size_t len = strlen(str_k->s);
      (void)memcpy(s, str_k->s, len);
      s += len;
      }
    }
  va_end(va);

  return new_str;
  } // str_concat

//------------------------------------------------------------- str_copy_string
size_t str_copy_string(Str *str, char *s, char pad) {
#if STR_LOG
  if (pad == '\0') printf("=== str_copy_string: %p \"%s\" '\\0'\n", str, s);
  else printf("=== str_copy_string: \"%s\" '%c' -> %p\n", s, pad, str);
#endif
  if (str == NULL) {
    STR_crash("str_copy_string: destination Str pointer NULL");
    }
  else if ((str)->s == NULL) {
    STR_crash("str_copy_string: destination string pointer NULL");
    }
  size_t n_copy = 0;
  if (s != NULL) {
    size_t len = strlen(s);
    size_t space = str->size - sizeof(Str);
    if (space > 0) {
      space--;
      n_copy = (len > space) ? space : len;
      if (n_copy > 0) (void)memcpy(str->s, s, n_copy);
      if (n_copy < space) (void)memset(&str->s[n_copy], pad, space - n_copy);
      str->s[space] = '\0';
      }
    }
  return n_copy;
  } // str_copy_string

//------------------------------------------------------------------- str_crash
void inline str_crash(char *e, char *file, int line) {
  fprintf(stderr, "STR error \"%s\" (file %s; line %d)\n", e, file, line);
  fflush(stderr);
  abort();
  } // str_crash

//------------------------------------------------------------------- str_first
char *str_first(Str *str, char *s, size_t from) {
#if STR_LOG
  printf("=== str_first %p \"%s\" from: %zu\n", str, s, from);
#endif
  char *p = NULL;
  if (str != NULL && str->s != NULL && *str->s != '\0' &&
      s != NULL && *s != '\0') {
    if (from >= strlen(str->s)) {
      Str *e = STR_printf_str(str,
          "str_first: from >= length (%zu >= %zu)",
          from, strlen(str->s));
      STR_crash(e->s);
      }
    p = strstr(str->s + from, s);
    }
  return p;
  } // str_first

//----------------------------------------------------------------- str_first_c
char *str_first_c(Str *str, char c, int ignore, size_t from) {
#if STR_LOG
  printf("=== str_first_c %p \'%c\' ignore case: %s from: %zu\n",
      str, c, (ignore) ? "true" : "false", from);
#endif
  char *p = NULL;
  if (str != NULL && str->s != NULL && *str->s != '\0') {
    if (from >= strlen(str->s)) {
      Str *e = STR_printf_str(str,
          "str_first: from >= length (%zu >= %zu)",
          from, strlen(str->s));
      STR_crash(e->s);
      }
    char *s = str->s + from;
    if (ignore) {
      char c_u = toupper(c);
      char c_l = tolower(c);
      while (p == NULL && *s != '\0') {
        if (*s == c_u || *s == c_l) p = s;
        else s++;
        }
      }
    else {
      p = strchr(s, c);
      }
    }
  return p;
  } // str_first_c

//-------------------------------------------------------------------- str_last
char *str_last(Str *str, char *s, size_t before) {
#if STR_LOG
  printf("=== str_last %p \"%s\" before: %zu\n", str, s, before);
#endif
  char *p = NULL;
  if (str != NULL && str->s != NULL && *str->s != '\0' &&
      s != NULL && *s != '\0') {
    if (before > strlen(str->s)) {
      Str *e = STR_printf_str(str,
          "str_last: before > length (%zu > %zu)",
          before, strlen(str->s));
      STR_crash(e->s);
      }
    else if (before == 0) {
      STR_crash("str_last: before == 0");
      }
    char *s0 = str->s;
    size_t n = strlen(s);
    char *q = NULL;
    char c = *s;
    int keep_going = 1;
    do {
      q = find_last_c(s0, c, before);
      if (q != NULL) {
        if (strncmp(q, s, n)) {
          before = q - s0;
          }
        else {
          p = q;
          keep_going = 0;
          }
        }
      else {
        keep_going = 0;
        }
      } while (keep_going);
    }
  return p;
  } // str_last

//------------------------------------------------------------------ str_last_c
char *str_last_c(Str *str, char c, int ignore, size_t before) {
#if STR_LOG
  printf("=== str_last_c %p \'%c\' ignore case: %s before: %zu\n",
      str, c, (ignore) ? "true" : "false", before);
#endif
  char *p = NULL;
  if (str != NULL && str->s != NULL && *str->s != '\0') {
    if (before > strlen(str->s)) {
      Str *e = STR_printf_str(str,
          "str_last_c: before > length (%zu > %zu)",
          before, strlen(str->s));
      STR_crash(e->s);
      }
    else if (before == 0) {
      STR_crash("str_last_c: before == 0");
      }
    char *s0 = str->s;
    char *s = s0 + before - 1;
    if (ignore) {
      char c_u = toupper(c);
      char c_l = tolower(c);
      while (p == NULL && s >= s0) {
        if (*s == c_u || *s == c_l) p = s;
        else s--;
        }
      }
    else {
      p = find_last_c(s0, c, before);
      }
    }
  return p;
  } // str_last_c

//-------------------------------------------------------------------- str_list
void str_list(Str *str) {
  if (str == NULL  ||  (str = *str->stack_p) == NULL) {
    printf("Nothing to list\n");
    }
  while (str != NULL) {
    char *s = (char *)((void *)str + sizeof(Str));
    size_t s_len = str->size - sizeof(Str);
    if (s_len > 0) s_len--;
    printf("%p %zu\n\tallocated(%zu):\t\"", str, str->size, s_len);
    for (int i = 0; i < s_len && s[i] != '\0'; i++) printf("%c", s[i]);
    printf("\"\n");
    if (str->s == NULL) printf("\tcurrent(0):\tnull\n");
    else printf("\tcurrent(%zu):\t\"%s\"\n", strlen(str->s), str->s);
    str = str->down;
    }
  } // str_list

//--------------------------------------------------------------------- str_new
Str *str_new(Str **str_p, size_t len, char *s, char pad) {
#if STR_LOG
  if (pad == '\0') printf("=== str_new: %p %zu \"%s\" '\\0'", *str_p, len, s);
  else printf("=== str_new: %p %zu \"%s\" '%c'", *str_p, len, s, pad);
#endif
  Str **str_stack_p = str_p;
  if (*str_stack_p != NULL) {
    str_stack_p = (*str_p)->stack_p;
    if (str_stack_p == NULL) {
      STR_crash("str_new: Str->stack_p found to be NULL");
      }
    }

  // If len > 0, add 1 for the terminating '\0'. Someone has to do it,
  // and the user needs a C string with space for len characters...
  if (len == 0  &&  s != NULL) len = strlen(s);
  size_t size = sizeof(Str) + ((len == 0) ? 0 : len + 1);

  // Allocate the needed memory block.
  void *blk = malloc(size);
  if (blk == NULL) {
    char mess[40];
    sprintf(mess, "str_new: Failed to allocate %lu bytes", size);
    STR_crash(mess);                                                     // ==>
    }

  // Push the block address to the Str stack.
  Str *new_str = (Str *)blk;
  new_str->up = NULL;
  new_str->down = *str_stack_p;
  new_str->size = size;
  new_str->stack_p = str_stack_p;
  if (*str_stack_p != NULL) (*str_stack_p)->up = new_str;
  *str_stack_p = new_str;

  // Process the C string.
  char *chars = NULL;
  if (len > 0) {
    chars = (char *)(blk + sizeof(Str));
    size_t n_copy = 0;
    if (s != NULL) {
      size_t s_size = strlen(s);
      n_copy = (s_size < len) ? s_size : len;
      (void)memcpy(chars, s, n_copy);
      }
    if (n_copy < len) (void)memset(&chars[n_copy], pad, len - n_copy);
    chars[len] = '\0';
    }
  new_str->s = chars;

#if STR_LOG
  printf("-> %p\n", blk);
#endif
  return new_str;
  } // str_new

//------------------------------------------------------------------ str_printf
Str *str_printf(Str **str_p, char *fmt, ...) {
#if STR_LOG
  printf("=== str_printf: %p \"%s\"\n", *str_p, fmt);
#endif
  va_list va;
  va_start(va, fmt);
  size_t n = vsnprintf(NULL, 0, fmt, va);
  va_end(va);
  Str *str = str_new(str_p, n, NULL, ' ');
  va_start(va, fmt);
  vsprintf(str->s, fmt, va);
  va_end(va);
  return str;
  } // str_printf

//----------------------------------------------------------------- str_release
void str_release(Str **str_p) {
  Str *str = *str_p;
  if (str == NULL) return;                                                //==>
#if STR_LOG
  printf("=== str_release: %p %p \"%s\"\n", *str->stack_p, str, str->s);
#endif

  Str *up = str->up;
  Str *down = str->down;
  if (up == NULL) { // str is on top of the stack
    if (down == NULL) {
      *str->stack_p = NULL; // no other strings in the stack
      }
    else {
      *str->stack_p = down;
      down->up = NULL;
      }
    }
  else if (down == NULL) { // str is at the bottom of the stack
    up->down = NULL;
    }
  else { // str is in the middle of the stack
    up->down = down;
    down->up = up;
    }
  free(str);
  *str_p = NULL; // to possibly force a crash if it is used again
  } // str_release

//------------------------------------------------------------- str_release_all
void str_release_all(Str *str) {
#if STR_LOG
  printf("=== str_release_all\n");
#endif
  if (str != NULL) {
    Str **stack_p = str->stack_p;
    while (*stack_p != NULL) {
      Str *p = *stack_p;
      *stack_p = (*stack_p)->down;
      p->down = NULL;
      p->s = NULL;
      p->stack_p = NULL;
      p->up = NULL;
      free(p);
      }
    }
  } // str_release_all

//------------------------------------------------------------------ str_remove
void str_remove(Str *str, size_t from, size_t before) {
#if STR_LOG
  printf("=== str_remove from %p: from %zu before %zu\n", str, from, before);
#endif
  if (str != NULL && str->s != NULL && *str->s != '\0') {
    size_t len = strlen(str->s);
    if (from < 0 || from >= len || before > len || before < 1
        || before <= from) {
      Str *e = STR_printf_str(str,
          "str_remove(%p, %zu, %zu): invalid indices (0, %zu)",
          str, from, before, len);
      STR_crash(e->s);
      }
    char *s = str->s;
    strcpy(s + from, s + before);
    }
  } // str_remove

//----------------------------------------------------------------- str_replace
void str_replace(Str *str, char *old, char *new) {
#if STR_LOG
  printf("=== str_replace %p \"%s\" -> \"%s\"\n", str, old, new);
#endif
  if (str != NULL && str->s != NULL && *str->s != '\0'
      && old != NULL && *old != '\0' && new != NULL) {
    size_t l_new = strlen(new);
    size_t l_old = strlen(old);
    if (l_new > l_old) {
      Str *e = STR_printf_str(str,
          "str_replace: new (\"%s\") longer than old (\"%s\")",
          new, old);
      STR_crash(e->s);
      }
    char *from = str->s;
    char *to = from;
    char *p = from;
    while (p != NULL) {
      p = strstr(from, old);
      if (p != NULL) {
        size_t dist = p - from;
        strncpy(to, from, dist);
        to += dist;
        from += dist + l_old;
        strncpy(to, new, l_new);
        to += l_new;
        }
      }
    strcpy(to, from);
    }
  } // str_replace

//--------------------------------------------------------------- str_replace_c
void str_replace_c(Str *str, char old, char new, int keep) {
#if STR_LOG
  printf("=== str_replace_c %p \'%c\' -> \'%c\' keep case: %s\n",
      str, old, new, (keep) ? "true" : "false");
#endif
  if (str != NULL && str->s != NULL) {
    char *s = str->s;
    if (keep) {
      char old_u = toupper(old);
      char new_l = tolower(new);
      char new_u = toupper(new);
      while (*s != '\0') {
        if (toupper(*s) == old_u) {
          if (isupper(*s)) *s = new_u;
          else *s = new_l;
          }
        s++;
        }
      }
    else {
      while (*s != '\0') {
        if (*s == old) *s = new;
        s++;
        }
      }
    }
  } // str_replace_c

//--------------------------------------------------------------------- str_sub
Str *str_sub(Str *str, size_t from, size_t before) {
#if STR_LOG
  printf("=== str_sub from %p: from %zu before %zu\n", str, from, before);
#endif
  Str *new_str = NULL;
  if (str != NULL && str->s != NULL && *str->s != '\0') {
    size_t len = strlen(str->s);
    if (from < 0 || from >= len || before > len || before < 1
        || before <= from) {
      Str *e = STR_printf_str(str,
          "str_sub(%p, %zu, %zu): invalid indices (0, %zu)",
          str, from, before, len);
      STR_crash(e->s);
      }
    new_str = str_new(&str, before - from, str->s + from, ' ');
    }
  return new_str;
  } // str_sub

//---------------------------------------------------------- str_to_upper_lower
void str_to_upper_lower(Str *str, int (*f)(int)) {
#if STR_LOG
  printf("=== str_to_upper_lower(%p)\n", str);
#endif
  if (str != NULL && str->s != NULL) {
    char *s = str->s;
    while (*s != '\0') {
      *s = (*f)(*s);
      s++;
      }
    }
  } // str_to_upper_lower
