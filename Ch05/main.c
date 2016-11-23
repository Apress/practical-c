/* Try_catch: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdio.h>
#include <stdlib.h>
#include "exception.h"

#define DEFAULT_EXCEPTION  0
#define MULTIPLE_CATCHES   1
#define MULTIPLE_TRIES_NO  2
#define MULTIPLE_TRIES_YES 3
#define ALLOCATE_EXCEPTION 4
#define FILE_EXCEPTIONS    5

#define EXAMPLE FILE_EXCEPTIONS

int main(void) {
#if EXAMPLE == DEFAULT_EXCEPTION
  char *default_exception = "Invalid number";
  for (int k = 0; k < 20; k++) {
    TRY
      switch(k) {
        case 0:
        case 1 ... 9:
          printf(" %d", k);
          break;
        default:
          THROW(default_exception);
          break;
        }
    CATCH(default_exception);
      fprintf(stderr, "%s: switched on %d while only 0 to 9 are foreseen\n",
          default_exception, k);
    TRY_DONE;
    }
  printf("\n");

#elif EXAMPLE == MULTIPLE_CATCHES
  char *exception_1 = "Exception #1";
  char *exception_2 = "Exception #2";

  TRY
    fprintf(stderr, "Throwing exception 1\n");
    THROW(exception_1);
    fprintf(stderr, "Throwing exception 2 for the first time\n");
    THROW(exception_2);
    fprintf(stderr, "Throwing exception 2 for the second time\n");
    THROW(exception_2);
  CATCH(exception_1)
    fprintf(stderr, "This is the catch of 1\n");
  CATCH(exception_2);
    fprintf(stderr, "This is the first catch of 2\n");
  CATCH(exception_2);
    fprintf(stderr, "This is the second catch of 2\n");
  TRY_DONE;
  fprintf(stderr, "Done!\n");

#elif EXAMPLE == MULTIPLE_TRIES_NO
  char *exception_1 = "Exception #1";
  char *exception_2 = "Exception #2";

  TRY
    fprintf(stderr, "Throwing exception 1\n");
    THROW(exception_1);
  CATCH(exception_1)
    fprintf(stderr, "Exception 1 caught\n");
    fprintf(stderr, "Throwing exception 2\n");
    THROW(exception_2);
  CATCH(exception_2);
    fprintf(stderr, "Exception 2 caught\n");
  TRY_DONE;
  fprintf(stderr, "Done!\n");

#elif EXAMPLE == MULTIPLE_TRIES_YES
  char *exception_1 = "Exception #1";
  char *exception_2 = "Exception #2";

//  char *a_string = "TRY 1";
  TRY
//    fprintf(stderr, "%s\n", a_string);
    fprintf(stderr, "Throwing exception 1\n");
    THROW(exception_1);
  CATCH(exception_1)
    fprintf(stderr, "Exception 1 caught\n");
    TRY
//      char *a_string = "TRY 2";
//      fprintf(stderr, "%s\n", a_string);
      fprintf(stderr, "Throwing exception 2\n");
      THROW(exception_2);
    CATCH(exception_2);
      fprintf(stderr, "Exception 2 caught\n");
    TRY_DONE;
//    fprintf(stderr, "%s\n", a_string);
  TRY_DONE;
  fprintf(stderr, "Done!\n");

#elif EXAMPLE == ALLOCATE_EXCEPTION
#include "allocate.h"
  char *buf;
  unsigned int n = 4096;
  TRY
    buf = allocate(n);
  CATCH(allocate_exception)
    fprintf(stderr, "couldn't allocate a buffer of %d\n", n);
    exit(EXIT_FAILURE);
  TRY_DONE;
  free(buf);

#elif EXAMPLE == FILE_EXCEPTIONS
#include "allocate.h"
#define BUF_SIZE 128
#define CLEAN_UP() {            \
    if (filin) fclose(filin);   \
    if (filout) fclose(filout); \
    if (buf) free(buf);         \
    }
  char *open_file_exception = "could not be opened";
  char *write_file_exception = "I/O error when writing file";
  char *filename = NULL;
  FILE *filin = NULL;
  FILE *filout = NULL;
  char *buf = NULL;
  size_t n = 0;

  TRY
    filename = "in.txt";
    filin = fopen(filename, "r");
    if (!filin) THROW(open_file_exception);

    filename = "out.txt";
    filout = fopen(filename, "w");
    if (!filout) THROW(open_file_exception);

    buf = allocate(BUF_SIZE);

    do {
      n = fread(buf, 1, BUF_SIZE, filin);
      if (n > 0 && fwrite(buf, 1, n, filout) != n) THROW(write_file_exception);
      } while (n > 0);

  CATCH(open_file_exception)
    printf("File %s %s\n", filename, open_file_exception);
    CLEAN_UP();
    return EXIT_FAILURE;                                                  //-->
  CATCH(write_file_exception)
    printf("File %s %s\n", write_file_exception, filename);
    CLEAN_UP();
    return EXIT_FAILURE;                                                  //-->
  CATCH(allocate_exception)
    printf("couldn't allocate a buffer of %d\n", BUF_SIZE);
    CLEAN_UP();
    return EXIT_FAILURE;                                                  //-->
  TRY_DONE;
  CLEAN_UP();

#endif
  return EXIT_SUCCESS;
  }
