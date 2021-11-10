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
#ifndef __RENDER_H__
#define __RENDER_H__

/*=============================================================================
 * clear_render_components
 *
 * Resets all of the render flags for the display.  
 *============================================================================*/
void clear_render_components(RenderComponents *c);

/*=============================================================================
 * load_palette_swatches
 *
 * Populates the 2 palette swatch bitmaps (one for the main draw area, one
 * for the color select area) with the colors used by the current Picture.  
 *============================================================================*/
void load_palette_swatches(void);

/*=============================================================================
 * render_main_area_squares
 *
 * Draws content for each of the squares of the play area (either a number,
 * if the square isn't filled in, otherwise a block of color respresenting the
 * color that has been filled in).
 * 
 * - x_off and y_off refer to the position of the picture that makes up the
 *   upper left part of the play area
 *============================================================================*/
void render_main_area_squares(BITMAP *dest, int x_off, int y_off);

/*=============================================================================
 * render_main_area_square_at
 *
 * Draws content of a specific square of the play area (either a number if
 * the square isn't filled in, otherwise a block of color representing the
 * color that has been filled in).
 * 
 * - tl_x and tl_y refer to the position of the picture that makes up the upper
 *   left part of the play area
 * - off_x and off_y refer to the position within the play area to draw
 *============================================================================*/
void render_main_area_square_at(BITMAP *dest, int tl_x, int tl_y,
                               int off_x, int off_y);

/*=============================================================================
 * render_palette_item_at
 *
 * Draws content of a specific index of the palette.  If change_page is set,
 * the page will be adjusted as well.  Otherwise, it will just draw the
 * specified index number and swatch at the place where it would normally go.
 *============================================================================*/

void render_palette_item_at(BITMAP *dest, int palette_index, int change_page);

/*=============================================================================
 * render_screen
 *
 * Draws screen components.  The components that are drawn is dictated by
 * the global RenderComponents object.
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c);

#endif
