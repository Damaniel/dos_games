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

typedef struct {
  /* The color this square should be */
  char pal_entry;
  /* 0 = not filled in, x = value actually filled in */
  char fill_value;
  /* The order in which this square was *correctly* filled in */
  int  order;
  /* 0 - this square isn't correct yet.  1 = this square is correct */
  char correct;
} ColorSquare;

typedef struct {
  /* The x and y position of the Nth square filled in correctly */
  short x;
  short y;
} OrderItem;

/* A picture (that the player can color in) */
typedef struct {
  char image_name[32];
  short w;
  short h;
  unsigned char category;
  unsigned char num_colors;
  ColorSquare *pic_squares;
  OrderItem *draw_order;
  char *mistakes;
} Picture;

/* A collection of metadata regarding a picture and completion progress */
typedef struct {
    char name[9];
    char category;
    short width;
    short height;
    char colors;
    int progress;
} PictureItem;

/* A collection of metadata representing a collection of pictures */
typedef struct {
    char name[9];
    int items;
} CollectionItem;

/*=============================================================================
 * get_picture_metadata
 *
 * Gets information about a Picture file
 *============================================================================*/
void get_picture_metadata(char *basepath, char *filename, PictureItem *p);

/*=============================================================================
 * get_progress_metadata
 *
 * Gets information about a progress file
 *============================================================================*/
void get_progress_metadata(char *basepath, char *filename, PictureItem *p);

/*=============================================================================
 * get_collections
 * 
 * Populates a list of collection directories
 *============================================================================*/
void get_collections(void);

/*=============================================================================
 * get_picture_files
 * 
 * Populates a list of Picture files along with their metadata
 *============================================================================*/
void get_picture_files(char *collection);

/*=============================================================================
 * save_progress_file
 *
 * Dumps the player's progress to a file
 *============================================================================*/
int save_progress_file(Picture *p);

/*=============================================================================
 * load_progress_file
 *
 * Load's the saved progress for a picture from a file.
 *============================================================================*/
int load_progress_file(Picture *p);

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
 * check_completion
 *
 * Checks to see if the correct square count matches the total number of 
 * squares in the picture
 *============================================================================*/
int check_completion(void);

/*=============================================================================
 * int_handler
 *
 * An interrupt handler called 30 times per second.  Not currently used for
 * anything but will be used for progress timing.
 *============================================================================*/
void int_handler(void);

/*=============================================================================
 * init_new_pic_defaults
 *
 * Resets picture-specific variables to their default values
 *============================================================================*/
void init_new_pic_defaults(void);

/*=============================================================================
 * init_load_dialog_defaults
 *
 * Resets load dialog-specific variables to their default values
 *============================================================================*/
void init_load_dialog_defaults(void);

/*=============================================================================
 * init_defaults
 *
 * Resets various variables to their default values
 *============================================================================*/
void init_defaults(void);

#endif
