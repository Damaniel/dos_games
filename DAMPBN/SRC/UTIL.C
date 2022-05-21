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
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dir.h>
#include "../include/globals.h"

#define NUM_CATEGORIES 8

volatile unsigned int g_elapsed_time;
volatile unsigned long int g_frame_counter;
volatile int g_next_frame;

int g_game_timer_running;
int g_time_to_update_elapsed;

int g_title_countdown;
int g_finished_countdown;

int g_mistake_count;
int g_correct_count;

int g_total_picture_squares;

char g_collection_name[9];

char g_picture_file_basename[9];

Picture *g_picture;

char *g_categories[NUM_CATEGORIES] = {
    "Misc.",
    "Other",
    "Landscape",
    "Animals",
    "Fantasy",
    "Abstract",
    "Pattern"
};

CollectionItem g_collection_items[MAX_FILES];
PictureItem g_pic_items[MAX_FILES];

int g_num_collections;
int g_load_collection_index;
int g_load_collection_offset;
int g_load_collection_cursor_offset;

int g_num_picture_files;
int g_load_picture_index;
int g_load_picture_offset;
int g_load_cursor_offset;

int g_load_new_file;

int g_replay_increment;
int g_replay_total;

int g_replay_x;
int g_replay_y;

int g_replay_first_time;

int g_load_section_active;

/*=============================================================================
 * get_picture_metadata
 *============================================================================*/
void get_picture_metadata(char *basepath, char *filename, PictureItem *p) {
    char full_file[128];
    FILE *fp;
    int i;

    sprintf(full_file, "%s/%s.pic", basepath, filename);
    fp = fopen(full_file, "rb");

    /* Get the relevant fields from this file */
    /* Category, dimensions, colors */
    fgetc(fp);
    fgetc(fp);
    fread(&p->width, 1, sizeof(short), fp);
    fread(&p->height, 1, sizeof(short), fp);
    fread(&p->category, 1, sizeof(char), fp);
    for(i=0;i<32;i++)
    fgetc(fp);
    fread(&p->colors, 1, sizeof(char), fp);

    fclose(fp);    
}

/*=============================================================================
 * get_progress_metadata
 *============================================================================*/
void get_progress_metadata(char *basepath, char *filename, PictureItem *p) {
    char full_file[128];
    FILE *fp;
    int i;

    sprintf(full_file, "%s/%s.pro", basepath, filename);
    fp = fopen(full_file, "rb");
    if (fp == NULL) {
        p->progress = 0;
        return;
    }

    for (i=0;i<26;i++)
        fgetc(fp);

    fread(&p->progress, 1, sizeof(int), fp);

    fclose(fp);
}

void get_collections(void) {
  struct ffblk f;
  int i;
  int total_collections;
  int done;
  char names[MAX_COLLECTIONS][9];

  total_collections = 0;
  done = findfirst(COLLECTION_PATHSPEC, &f, FA_DIREC);
  while (!done && total_collections < MAX_COLLECTIONS) 
  {
    if (strcmp(f.ff_name, ".") !=0 && strcmp(f.ff_name, "..") != 0) {
      strncpy(names[total_collections], f.ff_name, 8);
      total_collections++;
    }
    done = findnext(&f);    
  }

  for(i=0; i< total_collections; i++) {
    memcpy(g_collection_items[i].name, names[i], 8);
  }

  g_num_collections = total_collections;
}

/*=============================================================================
 * get_picture_files
 *============================================================================*/
void get_picture_files(char *collection) {
    struct ffblk f;
    int done, i;
    int total_files;
    char pic_pathspec[64];
    char pic_dir[64];
    char progress_dir[64];
    char names[MAX_FILES][9];
    char *filename;

    sprintf(pic_pathspec, "%s/%s/*.pic", PIC_FILE_DIR, collection);
    sprintf(pic_dir, "%s/%s", PIC_FILE_DIR, collection);
    sprintf(progress_dir, "%s/%s", PROGRESS_FILE_DIR, collection);

    /* Reset file count.  There may be extra files from a previous call in
       the structure, but if the total file count is correct, we don't 
       really care. */
    total_files = 0;
    done = findfirst(pic_pathspec, &f, 0);
    while (!done && total_files < MAX_FILES) 
    {
        filename = strtok(f.ff_name, ".");
        strncpy(names[total_files], filename, 8);
        total_files++;
        done = findnext(&f);
    } 

    for (i=0; i< total_files; i++) {
        /* Use memcpy instead of strncpy due to GCC 9.x string truncation
           warnings */
        memcpy(g_pic_items[i].name, names[i], 8);
        get_picture_metadata(pic_dir, names[i], &g_pic_items[i]);
        get_progress_metadata(progress_dir, names[i], &g_pic_items[i]);
    }

    g_num_picture_files = total_files;

    /* Update the name of the collection */
    memcpy(g_collection_name, collection, 9);
}

/*=============================================================================
 * save_progress_file
 *============================================================================*/
int save_progress_file(Picture *p) {
  FILE *fp;
  int time, i;
  char progress_file[80];

  sprintf(progress_file, "%s/%s/%s.pro",  PROGRESS_FILE_DIR, 
          g_collection_name,
          g_picture_file_basename);

  fp = fopen(progress_file, "wb");
  if (fp == NULL)
    return -1;

  fprintf(fp, "PR");
  
  /* For now, don't save the file name in here.  */
  for(i=0; i<12; i++)
    fputc(0, fp);

  /* Write the image size */
  fwrite(&p->w, 1, sizeof(short), fp);
  fwrite(&p->h, 1, sizeof(short), fp);

  /* Write total elapsed time */
  time = g_elapsed_time;
  fwrite(&time, 1, sizeof(unsigned int), fp);

  /* Write correct and wrong count */
  fwrite(&g_mistake_count, 1, sizeof(int), fp);
  fwrite(&g_correct_count, 1, sizeof(int), fp);

  fwrite(&g_draw_style, 1, sizeof(char), fp);

  /* Write padding */
  for(i = 0; i < 33; i++)
    fputc(0, fp);
  
  /* Write out move data */
  for(i = 0; i < g_correct_count; i++) {
    fwrite(&(p->draw_order[i].x), 1, sizeof(short), fp);
    fwrite(&(p->draw_order[i].y), 1, sizeof(short), fp);
  }

  /* Write out mistake data */
  fwrite(p->mistakes, p->w * p->h , sizeof(char), fp);

  fclose(fp);

  return 0;
}

/*=============================================================================
 * load_progress_file
 *============================================================================*/
int load_progress_file(Picture *p) {
  FILE *fp;
  unsigned int e_time;
  int i, j, mistakes, progress, size, target_size, offset;
  short x, y;
  short width, height;
  char magic[2];
  char progress_file[128];

  sprintf(progress_file, "%s/%s/%s.pro",  PROGRESS_FILE_DIR, 
         g_collection_name, 
         g_picture_file_basename);

  fp = fopen(progress_file, "rb");
  if (fp == NULL) {
    /* No progress file.  That's fine. */
    return 0;
  }

  /* Get the file size so we can do sanity checks */
  size = 0;
  while (!feof(fp)) {
    fgetc(fp);
    size++;
  }
  size--;
  rewind(fp);

  /* If the file is smaller than the size of a header, return */
  if(size < 64) {
    fclose(fp);
    return -1;
  }

  /* If the first two bytes aren't PR', then return */
  magic[0] = fgetc(fp);
  magic[1] = fgetc(fp);
  if(magic[0] != 'P' || magic[1] != 'R') {
    fclose(fp);
    return -1;
  }

  /* for now, ignore the file name data*/
  for(i = 0; i < 12; i++) 
    fgetc(fp);

  /* Load the width and the height.  If they don't match the provided picture,
     return an error */
  fread(&width, 1, sizeof(short), fp);
  fread(&height, 1, sizeof(short), fp);
  if(width != p->w || height != p->h) {
    return -1;
  }

  /* Load and set elapsed time */
  fread(&e_time, 1, sizeof(unsigned int), fp);
  g_elapsed_time = e_time;

  /* Load and set mistake count */
  fread(&mistakes, 1, sizeof(int), fp);
  g_mistake_count = mistakes;

  /* Load and set progress counter */
  fread(&progress, 1, sizeof(int), fp);
  g_correct_count = progress;

  fread(&g_draw_style, 1, sizeof(char), fp);

  /* Load and discard padding */
  for(i = 0; i < 33; i++)
    fgetc(fp);

  /* Check to see if the number of remaining bytes is enough to load the
     buffer */
  size -= 64;
  target_size = (g_correct_count * 4) + (p->w * p->h);
  if(target_size != size) {
    fclose(fp);
    return -1;
  }

  /* Load the progress data and update the picture structure*/
  for(i = 0; i< g_correct_count; i++) {
    fread(&x, 1, sizeof(short), fp);
    fread(&y, 1, sizeof(short), fp);
    offset = y * p->w + x;
    p->draw_order[i].x = x;
    p->draw_order[i].y = y;
    p->pic_squares[offset].fill_value = p->pic_squares[offset].pal_entry;
    p->pic_squares[offset].correct = 1;
  }

  /* Load mistake data */
  fread(p->mistakes, p->w * p->h, sizeof(char), fp);

  /* Loop through and update the picture structure with the mistake data */
  for(j = 0; j < p->h; j++) {
    for(i = 0;i < p->w; i++) {
      offset = j * p->w + i;
      if(p->mistakes[offset] != 0) {
       p->pic_squares[offset].fill_value = p->mistakes[offset];
       p->pic_squares[offset].correct = 0;       
      }
    }
  }

  fclose(fp);
  return 0;

}

/*=============================================================================
 * load_picture_file
 *============================================================================*/
Picture *load_picture_file(char *filename) {
  FILE *fp;
  Picture *pic;
  char *base_filename, *base_no_ext;
  unsigned char magic[2];
  unsigned char compression;
  int i, bytes_processed;
  float pal_offset;
  RGB pic_pal[64];
  unsigned char first_byte, run_length;

  fp = fopen(filename, "rb");
    if (fp == NULL)
      return NULL;

  /* Check for magic bytes */
  fscanf(fp, "%c%c", &magic[0], &magic[1]);
  if(magic[0] != 'D' || magic[1] != 'P') {
    fclose(fp);
    return NULL;
  }

  /* Set up the Picture object */
  pic = (Picture *)malloc(sizeof(Picture));

  /* Read in the header */
  fread(&(pic->w), 1, sizeof(short), fp);
  fread(&(pic->h), 1, sizeof(short), fp);
  fread(&(pic->category), 1, sizeof(unsigned char), fp);
  fread(pic->image_name, 32, sizeof(char), fp);
  fread(&(pic->num_colors), 1, sizeof(unsigned char), fp);
  fread(&compression, 1, sizeof(unsigned char), fp);
  for(i=0; i<64; i++) {
    pic_pal[i].r = fgetc(fp);
    pic_pal[i].g = fgetc(fp);
    pic_pal[i].b = fgetc(fp);
  }
  for(i=0;i<23;i++) {
    fgetc(fp);
  }

  /* Set the total square count for progress purposes */
  g_total_picture_squares = pic->w * pic->h;

  /* Set the image portion of the global palette */
  for (i=0; i<64; i++) {
    game_pal[i].r = pic_pal[i].r;
    game_pal[i].g = pic_pal[i].g;
    game_pal[i].b = pic_pal[i].b;
  }
  /* Make shading colors */
  for(i=64; i<128; i++) {
    pal_offset = (float)pic_pal[i-64].r * 0.5;
    game_pal[i].r = (int)pal_offset;
    pal_offset = (float)pic_pal[i-64].g * 0.5;    
    game_pal[i].g = (int)pal_offset;
    pal_offset = (float)pic_pal[i-64].b * 0.5;
    game_pal[i].b = (int)pal_offset;
  }

  /* Make highlight colors */
  for(i=128; i<192; i++) {
    pal_offset = (float)pic_pal[i-128].r * 1.5;
    if((int)pal_offset >63)
      game_pal[i].r = 63;
    else
      game_pal[i].r = (int)pal_offset;

    pal_offset = (float)pic_pal[i-128].g * 1.5;
    if((int)pal_offset >63)
      game_pal[i].g = 63;
    else
      game_pal[i].g = (int)pal_offset;

    pal_offset = (float)pic_pal[i-128].b * 1.5;
    if((int)pal_offset >63)
      game_pal[i].b = 63;
    else
      game_pal[i].b = (int)pal_offset;      
  }

  /* Set the size of the play area */
  g_play_area_w = pic->w < MAX_PLAY_AREA_WIDTH ? pic->w : MAX_PLAY_AREA_WIDTH;
  g_play_area_h = pic->h < MAX_PLAY_AREA_HEIGHT ? pic->h : MAX_PLAY_AREA_HEIGHT;

  /* Create required Picture arrays */
  pic->pic_squares = (ColorSquare *)malloc(pic->w * pic->h *
                                           sizeof(ColorSquare));
  pic->draw_order = (OrderItem *)malloc(pic->w * pic->h *
                                        sizeof(OrderItem));
  pic->mistakes = (char *)malloc(pic->w * pic->h * sizeof(char));
  memset(pic->mistakes, 0x00, pic->w*pic->h);

  /* Check compression type and perform appropriate decompression */
  if(compression == COMPRESSION_NONE) {
    for(i=0; i< (pic->w*pic->h); i++) {
      /* Using '+ 1'  since palettes in the Picture go from 1-64, not 0-63 */
      (pic->pic_squares[i]).pal_entry = fgetc(fp) + 1;
      (pic->pic_squares[i]).fill_value = 0;
      (pic->pic_squares[i]).order = -1;
      (pic->pic_squares[i]).correct = 0;
    }
  } else {
    bytes_processed = 0;
    while (bytes_processed < (pic->w * pic->h)) {
      first_byte = fgetc(fp);
      if(first_byte & 0x80) {
        /* found a run.  Load the next byte and write the appropriate
           number of copies to the buffer */
        run_length = fgetc(fp);
        for (i=0;i<run_length;i++) {
          (pic->pic_squares[bytes_processed]).pal_entry =(first_byte & 0x7F)+1;
          (pic->pic_squares[bytes_processed]).fill_value = 0;
          (pic->pic_squares[bytes_processed]).order = -1;
          (pic->pic_squares[bytes_processed]).correct = 0;          
          bytes_processed++;
        }
      } else {
        /* Found a single value */
        (pic->pic_squares[bytes_processed]).pal_entry = first_byte + 1;
        (pic->pic_squares[bytes_processed]).fill_value = 0;
        (pic->pic_squares[bytes_processed]).order = -1;
        (pic->pic_squares[bytes_processed]).correct = 0;         
        bytes_processed++;
      }
    } 
  }

  base_filename = basename(filename);
  base_no_ext = strtok(base_filename, ".");
  strncpy(g_picture_file_basename, base_no_ext, 8);

  fclose(fp);
  return pic;

}

/*=============================================================================
 * free_picture_file
 *============================================================================*/
void free_picture_file(Picture *p) {

  /* It's never been allocated at all, don't free it */
  if(p == NULL) 
    return;

  if(p->pic_squares != NULL) 
    free(p->pic_squares);
  if(p->draw_order != NULL)
    free(p->draw_order);
  if(p->mistakes != NULL)
    free(p->mistakes);
  if(p != NULL)
    free(p);
}

/*=============================================================================
 * check_completion
 *============================================================================*/
int check_completion(void) {
  if (g_correct_count >= g_total_picture_squares)
    return 1;
  return 0;
}

/*=============================================================================
 * int_handler
 *============================================================================*/
void int_handler(void) {
  /* do animation stuff here */
  g_frame_counter++;
  g_next_frame = 1;
}
END_OF_FUNCTION(int_handler);

void init_new_pic_defaults(void) {
  g_draw_cursor_x = 0;
  g_draw_cursor_y = 0;
  g_old_draw_cursor_x = 0;
  g_old_draw_cursor_y = 0;
  g_pic_render_x = 0;
  g_pic_render_y = 0;
  g_draw_position_x = 0;
  g_draw_position_y = 0;
  g_palette_page = 0;
  g_cur_color = 1;
  g_prev_color = 1;
  g_elapsed_time = 0;
  g_mistake_count = 0;
  g_correct_count = 0;
  g_mark_current = 0;
  g_across_scrollbar_x = 0;
  g_across_scrollbar_width = DRAW_AREA_WIDTH;
  g_down_scrollbar_y = 0;
  g_down_scrollbar_height = DRAW_AREA_HEIGHT;

}

void init_load_dialog_defaults(void) {
  g_load_collection_index = 0;
  g_load_collection_offset = 0;
  g_load_collection_cursor_offset = 0;

  g_load_picture_index = 0;
  g_load_picture_offset = 0;
  g_load_cursor_offset = 0;
  g_num_picture_files = 0;  
  g_load_new_file = 0;

  g_load_section_active = LOAD_COLLECTION_ACTIVE;
}

/*=============================================================================
 * init_defaults
 *============================================================================*/
void init_defaults(void) {
  int i;
  
  init_new_pic_defaults();
  init_load_dialog_defaults();

  g_game_done = 0;
  g_game_timer_running = 0;  
  g_show_map_text = 0;
  g_draw_style = STYLE_SOLID;
  g_next_frame = 0;
  g_time_to_update_elapsed = FRAME_RATE;
  g_help_page = 0;
  
  /* Highlight certain UI buttons when the mouse is held down over them */
  g_highlight_style_button = 0;
  g_highlight_save_button = 0;
  g_highlight_load_button = 0;

  g_picture = NULL;

  for(i=0; i<128; i++)
    g_keypress_lockout[i] = 0;

  g_game_area_mouse_mode = MOUSE_MODE_NEUTRAL;
  g_keyboard_has_priority = 1;
  
  /* Variables used in the interrupt handler */
  LOCK_VARIABLE(g_elapsed_time);
  LOCK_VARIABLE(g_frame_counter);
  LOCK_VARIABLE(g_next_frame);
  LOCK_FUNCTION(int_handler);

  g_state = STATE_GAME;
}
