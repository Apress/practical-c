/* exception.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <stdio.h>
#include "exception.h"

Exception *exception_stack = NULL;

void exception_throw(char *e_name, char *file, int line) {
  Exception *p = exception_stack;
  if (p == NULL) { // no stack -> no TRY -> exception not caught
    fprintf(stderr, "Uncaught exception ");
    if (e_name) fprintf(stderr, "\"%s\"", e_name);
    else fprintf(stderr, "%p", e_name);
    if (file) {
      fprintf(stderr, " thrown from %s", file);
      if (line) fprintf(stderr, ":%d", line);
      }
    fflush(stderr);
    abort();
    }
  p->name = e_name; // stack is there -> jump to catch exception
  p->file = file;
  p->line = line;
  exception_stack = exception_stack->prev;
  longjmp(p->env, EXCEPTION_STATUS_THROWN);
  }
