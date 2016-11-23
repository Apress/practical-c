/* Database: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "dar.h"
#include "str.h"

#define ERR_CHK(mess) if (ret != SQLITE_OK) {               \
    fprintf(stderr, "%s: SQL error - %s\n", mess, errmsg);  \
    if (db) sqlite3_close(db);                              \
    return(EXIT_FAILURE);                                   \
    }

typedef struct book_t {
  Str **cat_names;
  int first_row;
  } book_t;

typedef struct cat_t {
  Str **cat_names;
  Str **str_stack_p;
  } cat_t;

static int callback_book(void *book, int nc, char **values, char **names);
static int callback_cat(void *cat, int nc, char **values, char **names);
static int save_col_0(void *col0, int nc, char **values, char **names);

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  sqlite3 *db = NULL;
  char *query = NULL;
  char *errmsg = NULL;
  int ret;

  //------- Connect to the database.
  ret = sqlite3_open_v2("library.db", &db, SQLITE_OPEN_READWRITE, NULL);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    if (db) sqlite3_close(db);
    return(EXIT_FAILURE);                                                // -->
    }

  //------- Find the maximum category ID.
  int max_cat = 0;
  query = "select max(category_id) from categories";
  ret = sqlite3_exec(db, query, save_col_0, &max_cat, &errmsg);
  ERR_CHK("max_cat");                                                    // -->
  printf("Maximum category: %d\n", max_cat);

  //------- Determine how many characters are needed for each category name.
  int n_chars = 0;
  query = "select max(length(category_name)) from categories";
  ret = sqlite3_exec(db, query, save_col_0, &n_chars, &errmsg);
  ERR_CHK("n_chars");                                                    // -->
  printf("Maximum category length: %d\n", n_chars);

  //------- Begin dynamic-array and string operations
  DAR_setup;
  STR_setup;

  //------- Create an array of String pointers (+1 because SQL starts from 1).
  Str **cat_names = DAR_new((Str *)0, max_cat + 1);

  //------- List the categories and save their names
  printf("\nCategories:\ncategory_id\tcategory_name\n");
  query = "select category_id, category_name from categories";
  cat_t cat;
  cat.cat_names = cat_names;
  cat.str_stack_p = &str_stack;
  ret = sqlite3_exec(db, query, callback_cat, &cat, &errmsg);
  ERR_CHK("Categories");                                                 // -->

  //------- List the books
  printf("\nBooks:\n");
  book_t book;
  book.cat_names = cat_names;
  book.first_row = 1;
  query = "select * from books order by author, title";
  ret = sqlite3_exec(db, query, callback_book, &book, &errmsg);
  ERR_CHK("Books");                                                      // -->

  //------- End dynamic-array and string operations.
  STR_dismantle;
  DAR_dismantle;

  //------- Close the database.
  sqlite3_close(db);

	return EXIT_SUCCESS;
  } // main

//--------------------------------------------------------------- callback_book
static int callback_book(void *book, int nc, char **values, char **names) {
  book_t *book_p = book;
  Str **cat_names = book_p->cat_names;
  if (book_p->first_row) {
    for(int k = 0; k < nc; k++) {
      if (!strcmp(names[k], "category_id")) printf("category\t");
      else printf("%s\t", names[k]);
      }
    printf("\n");
    }
  book_p->first_row = 0;
  for(int k = 0; k < nc; k++) {
    if (strlen(values[k])) {
      if (!strcmp(names[k], "category_id")) {
        int cat_id = 0;
        sscanf(values[k], "%d", &cat_id);
        printf("%s\t", cat_names[cat_id]->s);
        }
      else {
        printf("%s\t", values[k]);
        }
      }
    else {
      printf("-\t");
      }
    }
  printf("\n");
  return 0;
  } // callback_book

//---------------------------------------------------------------- callback_cat
static int callback_cat(void *cat, int nc, char **values, char **names) {
  cat_t *cat_p = cat;
  int cat_id = 0;
  sscanf(values[0], "%d", &cat_id);
  cat_p->cat_names[cat_id] = str_new(cat_p->str_stack_p, 0, values[1], '\0');
  printf("%s\t%s\n", values[0], values[1]);
  return 0;
  } // callback_cat

//------------------------------------------------------------------ save_col_0
static int save_col_0(void *col0, int nc, char **values, char **names) {
  int *first_val_p = col0;
  sscanf(values[0], "%d", first_val_p);
  return 0;
  } // save_col_0
