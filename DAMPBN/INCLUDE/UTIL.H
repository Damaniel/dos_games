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

#ifndef __UTIL_H__
#define __UTIL_H__

/*=============================================================================
 * load_picture_file
 *
 * Loads an image created using the dampbn convert tool into an appropriate
 * instance of the Picture data structure.  
 *============================================================================*/
Picture *load_picture_file(char *filename);

/*=============================================================================
 * free_picture_file
 *
 * Deallocates all objects associated with a picture file.
 *============================================================================*/
void free_picture_file(Picture *p);


/*=============================================================================
 * int_handler
 *
 * An interrupt handler called 30 times per second.  Not currently used for
 * anything but will be used for progress timing.
 *============================================================================*/
void int_handler(void);

/*=============================================================================
 * init_defaults
 *
 * Resets various variables to their default values
 *============================================================================*/
void init_defaults(void);

/*=============================================================================
 * load_graphics
 *
 * Load in-game graphics into bitmap objects.  All graphics are stored in the
 * PCX format.
 *============================================================================*/
int load_graphics(void);

/*=============================================================================
 * destroy_graphics
 *
 * Free memory used by loaded bitmaps.
 *============================================================================*/
void destroy_graphics(void);

#endif
