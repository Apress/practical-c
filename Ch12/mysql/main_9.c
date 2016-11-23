/* Database: main_9.c
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

#define SERVER "localhost"
#define USER   "root"
#define PASS   "apress"
#define DB     "library"

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

  //------- Initialize the connection and connect to the database.
  conn = mysql_init(NULL);
  if (!mysql_real_connect(conn, SERVER, USER, PASS, DB, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }

/*
  //------- List the tables.
  if (mysql_query(conn, "show tables")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  printf("Tables in the database:\n");
  do {
    row = mysql_fetch_row(res);
    if (row) printf("%s \n", row[0]);
    } while (row);
  mysql_free_result(res);
*/

  //------- Remove the "Non-fiction" category.
  if (mysql_query(conn,
      "delete from categories where category_name = 'Non-fiction'")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  printf("Category \"Non-fiction\" deleted\n");

  //------- Find the number of categories.
  if (mysql_query(conn, "select count(*) from categories")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  row = mysql_fetch_row(res);
  int n_cat = 0;
  sscanf(row[0], "%d", &n_cat);
  printf("Number of categories: %d\n", n_cat);
  mysql_free_result(res);

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

/*
  //------- Insert a record.
  if (mysql_query(conn, "insert into books (title, author, ISBN, category_id)"
      " values ('The Martian','Weir, Andy','978-0-553-41802-6',1);")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
*/

  //------- List the books.
  if (mysql_query(conn, "select * from books order by author")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(EXIT_FAILURE);                                                  // -->
    }
  res = mysql_use_result(conn);
  unsigned int n_cols = mysql_num_fields(res);

  // Use the column names as headings. Remember the column with
  // the category_id but print "category" as heading.
  printf("Books in the database:\n");
  MYSQL_FIELD *field = NULL;
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

  // Open a second connection to the database to obtain the category
  // name given the category ID.
  MYSQL *conn1;
  MYSQL_RES *res1;
  MYSQL_ROW row1;
  conn1 = mysql_init(NULL);
  if (!mysql_real_connect(conn1, SERVER, USER, PASS, DB, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn1));
    exit(EXIT_FAILURE);                                                  // -->
    }

  // Fetch the rows selected from the "books" table.
  do {
    row = mysql_fetch_row(res);
    if (row) {
      for (int kol = 0; kol < n_cols; kol++) {
        if (kol != cat_col) {
          printf("%s\t", row[kol]);
          }
        else {

          // Obtain from the "categories" table the category name
          // for the book's category_id.
          char q[70];
          sprintf(q, "select category_name from categories where"
              " category_id = %s", row[kol]);
          if (mysql_query(conn1, q)) {
            fprintf(stderr, "%s\n", mysql_error(conn));
            exit(EXIT_FAILURE);                                          // -->
            }
          res1 = mysql_use_result(conn1);
          row1 = mysql_fetch_row(res1);
          printf("%s\t", row1[0]);
          mysql_free_result(res1);
          }
        }
      printf("\n");
      }
    } while (row);
  mysql_close(conn1);
  mysql_free_result(res);

  //------- Close the connection
  mysql_close(conn);

  return EXIT_SUCCESS;
  } // main
