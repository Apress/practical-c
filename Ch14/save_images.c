/* save_images.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "main.h"
#include "save_images.h"

#define CELL_SIDE 160
#define FONT_SIZE 100 // was (CELL_SIDE/2)
#define CHAR_H (FONT_SIZE*7/10)
#define CHAR_W (FONT_SIZE*45/100)
#define CHAR_2W (FONT_SIZE)
#define OFFS_X ((CELL_SIDE-CHAR_W)/2)
#define OFFS_2X ((CELL_SIDE-CHAR_2W)/2)
#define OFFS_Y ((CELL_SIDE-CHAR_H)/2+CHAR_H)
#define MARGIN (CELL_SIDE/20)
#define EDGE_WIDTH 4
#define LINE_WIDTH 4
#define GRID_SIDE (CELL_SIDE*(N_NUM+N_NUM-1))
#define IMG_SIDE (GRID_SIDE+EDGE_WIDTH+EDGE_WIDTH)
#define Xo (EDGE_WIDTH-1)
#define Yo (EDGE_WIDTH-1)

MagickWand *m_puzz;
DrawingWand *d_puzz;
PixelWand *c_puzz;

void init_grid(void) {
  PixelSetColor(c_puzz,"black");
  MagickNewImage(m_puzz, IMG_SIDE, IMG_SIDE, c_puzz);
  MagickSetImageUnits(m_puzz, PixelsPerInchResolution);  // unnecessary
  MagickSetImageResolution(m_puzz, 300, 300);
  DrawSetStrokeOpacity(d_puzz,1);
  DrawSetStrokeAntialias(d_puzz,TRUE);
  DrawSetStrokeColor(d_puzz, c_puzz);
  DrawSetStrokeWidth(d_puzz, LINE_WIDTH);
  PushDrawingWand(d_puzz);
    PixelSetColor(c_puzz, "white");
    DrawSetFillColor(d_puzz, c_puzz);
    DrawSetStrokeWidth(d_puzz, 1);
    DrawRectangle(d_puzz, Xo, Yo, IMG_SIDE - EDGE_WIDTH, IMG_SIDE - EDGE_WIDTH);
    PopDrawingWand(d_puzz);
  }

// i0: 0 for numbers; 1 for operations
void draw_content(int i0) {
  PixelSetColor(c_puzz, "black");
  DrawSetFillColor(d_puzz, c_puzz);
  DrawSetFont (d_puzz, "Courier" ) ;
  DrawSetFontSize(d_puzz, FONT_SIZE);
  DrawSetTextAntialias(d_puzz, MagickTrue);
  for (int k = i0; k < N_NUM + N_NUM - 1; k += 2) {
    int y0 = CELL_SIDE * k;
    for (int j = i0; j < N_NUM + N_NUM - 1; j += 2) {
      if (mat[k][j] != -1) {
        int x0 = CELL_SIDE * j;
        char val_s[5];
        int offs_x = OFFS_X;
        if (i0 == 0) {
          sprintf(val_s, "%d", mat[k][j]);
          if (mat[k][j] >= 10) offs_x = OFFS_2X;
          }
        else {
          sprintf(val_s, "%c", ops[mat[k][j]]);
          }
        DrawAnnotation(d_puzz, x0 + offs_x, y0 + OFFS_Y, (unsigned char *)val_s);
        }
      }
    }
  }

void draw_loops(void) {
  double del = (1 - sqrt(2)/2) * (CELL_SIDE - MARGIN - MARGIN) / 2;
  PixelSetColor(c_puzz, "white");
  DrawSetFillColor(d_puzz, c_puzz);
  for (int k = 0; k < N_NUM + N_NUM - 1; k += 2) {
    int y0 = EDGE_WIDTH + CELL_SIDE * k;
    for (int j = 0; j < N_NUM + N_NUM - 1; j += 2) {
      if (mat[k][j] != -1) {
        int x0 = EDGE_WIDTH + CELL_SIDE * j;
        switch (z[k][j]) {
          case 0:
            DrawArc(d_puzz, x0 + MARGIN, y0 + MARGIN, x0 + CELL_SIDE - MARGIN, y0 + CELL_SIDE - MARGIN, 90, 270);
            DrawArc(d_puzz, x0 + 4*CELL_SIDE + MARGIN, y0 + MARGIN, x0 + 5*CELL_SIDE - MARGIN, y0 + CELL_SIDE - MARGIN, -90, 90);
            DrawLine(d_puzz, x0 + CELL_SIDE/2, y0 + MARGIN, x0 + CELL_SIDE/2 + 4*CELL_SIDE, y0 + MARGIN);
            DrawLine(d_puzz, x0 + CELL_SIDE/2, y0 + CELL_SIDE - MARGIN, x0 + CELL_SIDE/2 + 4*CELL_SIDE, y0 + CELL_SIDE - MARGIN);
            break;
          case 10:
            DrawArc(d_puzz, x0 + MARGIN, y0 + MARGIN, x0 + CELL_SIDE - MARGIN, y0 + CELL_SIDE - MARGIN, -180, 0);
            DrawArc(d_puzz, x0 + MARGIN, y0 + 4*CELL_SIDE + MARGIN, x0 +CELL_SIDE - MARGIN, y0 + 5*CELL_SIDE - MARGIN, 0, 180);
            DrawLine(d_puzz, x0 + MARGIN, y0 + CELL_SIDE/2, x0 + MARGIN, y0 + CELL_SIDE/2 + 4*CELL_SIDE);
            DrawLine(d_puzz, x0 + CELL_SIDE - MARGIN, y0 + CELL_SIDE/2, x0 + CELL_SIDE - MARGIN, y0 + CELL_SIDE/2 + 4*CELL_SIDE);
            break;
          case 20:
            // Patched up the lines
            DrawArc(d_puzz, x0 + MARGIN, y0, x0 + CELL_SIDE - MARGIN, y0 + CELL_SIDE - MARGIN, 45, 225);
            DrawArc(d_puzz, x0 + 4*CELL_SIDE - MARGIN, y0 - 4*CELL_SIDE + MARGIN, x0 + 5*CELL_SIDE - MARGIN, y0 - 3*CELL_SIDE, -135, 45);
            DrawLine(d_puzz, x0 + MARGIN + del, y0 + del, x0 + 4*CELL_SIDE - MARGIN + del + 3, y0 - 4*CELL_SIDE + MARGIN + del);
            DrawLine(d_puzz, x0 + CELL_SIDE - del - MARGIN, y0 + CELL_SIDE - MARGIN - del, x0 + 5*CELL_SIDE - MARGIN - del - 3, y0 - 3*CELL_SIDE - del - 1);
            break;
          case 30:
            // Patched up the lines
            DrawArc(d_puzz, x0 + MARGIN, y0, x0 + CELL_SIDE - MARGIN, y0 + CELL_SIDE - MARGIN, -225, -45);
            DrawArc(d_puzz, x0 + 4*CELL_SIDE - MARGIN, y0 + 4*CELL_SIDE, x0 + 5*CELL_SIDE - MARGIN, y0 + 5*CELL_SIDE - MARGIN, -45, 135);
            DrawLine(d_puzz, x0 +CELL_SIDE - MARGIN - del, y0 + del, x0 + 5*CELL_SIDE - MARGIN - del - 3, y0 + 4*CELL_SIDE + del + 1);
            DrawLine(d_puzz, x0 + del + MARGIN, y0 + CELL_SIDE - MARGIN - del, x0 + 4*CELL_SIDE + del - 4, y0 + 5*CELL_SIDE - MARGIN - del);
            break;
          default:
            break;
          }
        }
      }
    }
  }

void save_images(char *name) {
  char fname[32];

  MagickWandGenesis();
  m_puzz = NewMagickWand();
  d_puzz = NewDrawingWand();
  c_puzz = NewPixelWand();
  init_grid();

  draw_content(0);
  MagickDrawImage(m_puzz, d_puzz);
  sprintf(fname, "%s.jpg", name);
  MagickWriteImage(m_puzz, fname);

  draw_loops();  // must be first because it blanks half-circles
  draw_content(0);
  draw_content(1);
  MagickDrawImage(m_puzz, d_puzz);
  sprintf(fname, "%s_s.jpg", name);
  MagickWriteImage(m_puzz, fname);

  c_puzz = DestroyPixelWand(c_puzz);
  d_puzz = DestroyDrawingWand(d_puzz);
  m_puzz = DestroyMagickWand(m_puzz);
  MagickWandTerminus();
  }
