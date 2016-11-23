/* exception.h
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#ifndef EXCEPTION
#define EXCEPTION
#include <setjmp.h>

#define EXCEPTION_STATUS_INITIAL  0
#define EXCEPTION_STATUS_THROWN   1
#define EXCEPTION_STATUS_HANDLED  2

typedef struct Exception Exception;
struct Exception {
  char      *name;
  jmp_buf    env;
  char      *file;
  int        line;
  Exception *prev;
  };

#define THROW(e) exception_throw(e, __FILE__, __LINE__)

#define TRY {                                                            \
    volatile int exception_status;                                       \
    Exception exception;                                                 \
    exception.prev = exception_stack;                                    \
    exception_stack = &exception;                                        \
    exception_status = setjmp(exception.env);                            \
    if (exception_status == EXCEPTION_STATUS_INITIAL) {

#define CATCH(e)                                                         \
    if (exception_status == EXCEPTION_STATUS_INITIAL)                    \
        exception_stack = exception_stack->prev;                         \
    }                                                                    \
    else if (exception.name == e) {                                      \
      exception_status = EXCEPTION_STATUS_HANDLED;

#define TRY_DONE                                                         \
    if (exception_status == EXCEPTION_STATUS_INITIAL)                    \
        exception_stack = exception_stack->prev;                         \
    }                                                                    \
    if (exception_status == EXCEPTION_STATUS_THROWN)                     \
        exception_throw(exception.name, exception.file, exception.line); \
    }

extern Exception *exception_stack;

void exception_throw(char *e, char *file, int line);

#endif
