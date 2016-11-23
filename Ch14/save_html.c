/* save_html.c
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
#include "main.h"
#include "save_html.h"

int save_html(char *name, char *id) {

  char *TEXT_HEADER_1[] = {
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>",
      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" "
          "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">",
      "<html xmlns=\"http://www.w3.org/1999/xhtml\">",
      "<head>"
      };
  int text_header_1_len = sizeof(TEXT_HEADER_1) / sizeof(char *);

  char *TEXT_HEADER_2[] = {
      "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\"/>",
      "<meta http-equiv=\"Content-Style-Type\" content=\"text/css\"/>",
      "<style type=\"text/css\">",
      "body {",
      "font-family:Arial,Verdana,Sans-serif; font-size:14px;",
      "background-color:white;",
      "}",
      "table.x {empty-cells:show; border-collapse:collapse; padding:0px;",
      " border-style:solid; border-width:1px; border-color:black;",
      "}",
      "td.x {",
      "width:39px; height:39px; border:0px; padding:0px;",
      " text-align:center; vertical-align:middle; font-size:18px;",
      "}",
      "</style>",
      "</head>",
      "<body>"
      };
  int text_header_2_len = sizeof(TEXT_HEADER_2) / sizeof(char *);

  // Open the HTML file
  int f_res;
  FILE *fp = fopen(name, "w");
  if (fp == NULL) {
    printf("Unable to open the '%s'\n", name);
    return FALSE;                                                       // ==>
    }

  // Write the first block of header lines
  for (int k = 0; k < text_header_1_len; k++) {
    f_res = fprintf(fp, "%s\n", TEXT_HEADER_1[k]);
    if (f_res < 0) { fclose(fp); return FALSE; }                        // ==>
    }

  // Write the title of the page
  f_res = fprintf(fp, "<title>%s</title>\n", name);
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  // Write the second block of header lines
  for (int k = 0; k < text_header_2_len; k++) {
    f_res = fprintf(fp, "%s\n", TEXT_HEADER_2[k]);
    if (f_res < 0) { fclose(fp); return FALSE; }                        // ==>
    }

  // Write the title
  f_res = fprintf(fp, "<p style=\"font-size:20px;\">This grid hides "
      "%d operations</p>\n", n_t);
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  //---------- Write the puzzle as a HTML table
  f_res = fprintf(fp, "<table class=\"x\">\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  for (int k = 0; k < N_NUM + N_NUM - 1; k++) {
    f_res = fprintf(fp, "<tr>\n");
    if (f_res < 0) { fclose(fp); return FALSE; }                        // ==>
    for (int j = 0; j < N_NUM + N_NUM - 1; j++) {
      if (k % 2 == 0  &&  j % 2 == 0  &&  mat[k][j] != -1) {
        f_res = fprintf(fp, "<td class=\"x\" >%3d</td>", mat[k][j]);
        }
      else {
        f_res = fprintf(fp, "<td class=\"x\" >&nbsp;</td>");
        }
      if (f_res < 0) { fclose(fp); return FALSE; }                      // ==>
      }
    f_res = fprintf(fp, "</tr>\n");
    if (f_res < 0) { fclose(fp); return FALSE; }                        // ==>
    }

  f_res = fprintf(fp, "</table>\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  //---------- Write the puzzle identifier and some more text
  f_res = fprintf(fp, "<p style=\"font-family:Courier,monospace;"
      "font-size:x-small;\">%s</p>\n", id
      );
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  f_res = fprintf(fp, "<p style=\"font-size:20px;\">Solution</p>\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  //---------- Write the solution as a HTML table
  f_res = fprintf(fp, "<table class=\"x\">\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  for (int k = 0; k < N_NUM + N_NUM - 1; k++) {
    f_res = fprintf(fp, "<tr>\n");
    if (f_res < 0) { fclose(fp); return FALSE; }                        // ==>
    for (int j = 0; j < N_NUM + N_NUM - 1; j++) {
      f_res = fprintf(fp, "<td class=\"x\" "
          "style=\""
          "background-position:center;"
          "background-repeat:no-repeat;");
      if (f_res < 0) { fclose(fp); return FALSE; }                      // ==>
      if (mat[k][j] == -1) {
        if (z[k][j] == -1) {
          f_res = fprintf(fp, "\">&nbsp;</td>");
          }
        else {
          f_res = fprintf(fp,
              "background-image:url('img/%02d.bmp');"
              "\">&nbsp;</td>", z[k][j]);
          }
        }
      else if (k % 2 == 0  &&  j % 2 == 0){
        f_res = fprintf(fp,
            "background-image:url('img/%02d.bmp');"
            "\">%3d</td>", z[k][j], mat[k][j]);
        }
      else {
        f_res = fprintf(fp,
            "background-image:url('img/%02d.bmp');"
            "\">%c</td>", z[k][j], ops[mat[k][j]]);
        }
      if (f_res < 0) { fclose(fp); return FALSE; }                      // ==>
      }
    f_res = fprintf(fp, "</tr>\n");
    if (f_res < 0) { fclose(fp); return FALSE; }                        // ==>
    }

  f_res = fprintf(fp, "</table>\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  //---------- Write the puzzle identifier and finish off
  f_res = fprintf(fp, "<p style=\"font-family:Courier,monospace;"
      "font-size:x-small;\">%s</p>\n", id
      );
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  //---------- Write the icon to state that it is a valid XHTML 1.0
  f_res = fprintf(fp,
      "<p><a href=\"http://validator.w3.org/check?uri=referer\"><img\n"
      "    src=\"http://www.w3.org/Icons/valid-xhtml10\"\n"
      "    alt=\"Valid XHTML 1.0 Strict\" height=\"31\" width=\"88\"/>\n"
      "  </a></p>\n"
      );
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  // Close the body tag
  f_res = fprintf(fp, "</body>\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  // Close the HTML tag
  f_res = fprintf(fp, "</html>\n");
  if (f_res < 0) { fclose(fp); return FALSE; }                          // ==>

  // Close the HTML file
  f_res = fclose(fp);
  if (f_res != 0) return FALSE;                                         // ==>

  return TRUE;
  }
