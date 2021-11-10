/* Copyright 2021 Shaun Brandt
   
   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
   DEALINGS IN THE SOFTWARE.
 */
#ifndef __UI_CONSTS_H__
#define __UI_CONSTS_H__
 
 /* Values for all UI positional constants.  Avoids magic numbers and makes
    it clear where things go on screen. */

/*==============================================
 * General 
 *==============================================*/
#define NUMBER_BOX_WIDTH            11
#define NUMBER_BOX_HEIGHT           11
#define NUMBER_BOX_INTERIOR_WIDTH    9
#define NUMBER_BOX_INTERIOR_HEIGHT   9

/* Boxes are 11, offsets are 10.  This is because the boxes are drawn 
   overlapping (so there's only one shared border pixel between 2
   adjacent boxes */
#define NUMBER_BOX_RENDER_X_OFFSET  10
#define NUMBER_BOX_RENDER_Y_OFFSET  10

#define DRAW_AREA_X                  2
#define DRAW_AREA_Y                  2

/*==============================================
 * High-level elements
 *==============================================*/
#define RIGHT_SIDE_PANEL_X         209
#define RIGHT_SIDE_PANEL_Y           0

#define BOTTOM_PANEL_X               0
#define BOTTOM_PANEL_Y             169

#define MAIN_AREA_X                  0
#define MAIN_AREA_Y                  0

/*==============================================
 * Palette area 
 *==============================================*/
#define NUM_PALETTE_COLUMNS          4
#define NUM_PALETTE_ROWS             8

#define PALETTE_PAGE_1_START         0
#define PALETTE_PAGE_2_START        32

#define PALETTE_BOX_WIDTH            6
#define PALETTE_BOX_HEIGHT          11
#define PALETTE_BOX_INTERIOR_WIDTH   4
#define PALETTE_BOX_INTERIOR_HEIGHT  9

#define PALETTE_AREA_X             226
#define PALETTE_AREA_Y              83

#define SWATCH_AREA_X              236
#define SWATCH_AREA_Y               83

#define PALETTE_COLUMN_WIDTH        20
#define PALETTE_ITEM_HEIGHT         10

#define PALETTE_COLUMN_1_X         226
#define PALETTE_COLUMN_2_X         246
#define PALETTE_COLUMN_3_X         266
#define PALETTE_COLUMN_4_X         286
#define PALETTE_COLUMN_Y            83

/*==============================================
 * Buttons
 *==============================================*/
#define PAGE_1_BUTTON_X            237
#define PAGE_1_BUTTON_Y             69
#define PAGE_2_BUTTON_X            266
#define PAGE_2_BUTTON_Y             69
#define PAGE_BUTTON_WIDTH           25
#define PAGE_BUTTON_HEIGHT          11
/*==============================================
 * Cursors
 *==============================================*/
#define DRAW_CURSOR_WIDTH           10
#define DRAW_CURSOR_HEIGHT          10
#define PALETTE_CURSOR_WIDTH        16
#define PALETTE_CURSOR_HEIGHT       11
 
#endif
 