/* Database: main_10.c
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
#include <mysql.h>
#include "dar.h"
#include "str.h"

#define SERVER "localhost"
#define USER   "root"
#define PASS   "apress"
#define DB     "library"

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_FIELD *field;

  //------- Initialize the connection and connect to the database.
  conn = mysql_init(NULL);
  if (!mysql_real_connect(conn, SERVER, USER, PASS, DB, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }

  //------- Find the maximum category ID.
  if (mysql_query(conn, "select max(category_id) from categories")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  row = mysql_fetch_row(res);
  int max_cat = 0;
  sscanf(row[0], "%d", &max_cat);
  printf("Maximum category: %d\n", max_cat);
  mysql_free_result(res);

  //------- Determine how many characters are needed for each category name.
  if (mysql_query(conn, "select category_name from categories limit 1")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  field = mysql_fetch_field(res);
  int n_chars = (int)field->length;
  printf("Category length: %d\n", n_chars);
  mysql_free_result(res);

  //------- Begin dynamic-array and string operations
  DAR_setup;
  STR_setup;

  //------- Create an array of String pointers (+1 because SQL starts from 1).
  Str **cat_names = DAR_new((Str *)0, max_cat + 1);

  //------- Collect and list the category names
  if (mysql_query(conn, "select category_id, category_name from categories")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  do {
    row = mysql_fetch_row(res);
    if (row) {
      int cat_id = 0;
      sscanf(row[0], "%d", &cat_id);
      cat_names[cat_id] = STR_new(row[1]);
      }
    } while (row);
  mysql_free_result(res);
  printf("\nList of categories saved in memory:\n");
  for (int k = 1; k <= max_cat; k++) {
    if (cat_names[k]) printf("%d: \"%s\"\n", k, cat_names[k]->s);
    }

  //------- List the books.
  if (mysql_query(conn, "select * from books order by author, title")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  unsigned int n_cols = mysql_num_fields(res);

  // Use the column names as headings. Remember the column with
  // the category_id but print "category" as heading.
  printf("\nBooks in the database:\n");
  int cat_col = -1;
  int cat_col_found = 0;
  do {
    field = mysql_fetch_field(res);
    if (field) {
      if (!cat_col_found) cat_col++;
      if (!strcmp(field->name, "category_id")) {
        cat_col_found = 1;
        printf("category\t");
        }
      else {
        printf("%s\t", field->name);
        }
      }
    } while (field);
  printf("\n");

  // Fetch the rows selected from the "books" table.
  do {
    row = mysql_fetch_row(res);
    if (row) {
      for (int kol = 0; kol < n_cols; kol++) {
        if (kol != cat_col) {
          printf("%s\t", row[kol]);
          }
        else {
          int n_cat = 0;
          sscanf(row[kol], "%d", &n_cat);
          printf("%s\t", cat_names[n_cat]->s);
          }
        }
      printf("\n");
      }
    } while (row);
  mysql_free_result(res);

  //------- End dynamic-array and string operations.
  STR_dismantle;
  DAR_dismantle;

  //------- Close the connection
  mysql_close(conn);

  return EXIT_SUCCESS;
  } // main
