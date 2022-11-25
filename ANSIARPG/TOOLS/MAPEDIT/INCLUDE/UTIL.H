/* Copyright 2022 Shaun Brandt
   
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

#define MAP_NAME_LENGTH         20

#define COMPRESSION_NONE         0
#define COMPRESSION_RLE          1
#define COMPRESSION_PACKED       2
#define COMPRESSION_PACKED_RLE   3

typedef struct {
  char magic[2];
  unsigned char major_version;
  unsigned char minor_version;
  unsigned short map_id;
  char name[MAP_NAME_LENGTH];
  unsigned char map_width;
  unsigned char map_height;
  unsigned char compression;
  unsigned char pc_start_x;
  unsigned char pc_start_y;
  unsigned char padding[97];
} MapHeader;

int write_map_file(char *filename, char compression);
int read_map_file(char *filename);

void clear_map(void);
void set_exit(int index, int room_id, int x_pos, int y_pos);
void clear_exit(int index);

void set_palette_flags(int index, char solid_value, char damage_value);
char get_palette_damage_value(int index);
char get_palette_solid_value(int index);

void initialize_exits(void);
void initialize_attributes(void);
void initialize_app_defaults(void);
void initialize_palette_menu_defaults(void);
void initialize_palette(void);

void set_map_at(int x, int y, unsigned char palette_entry);
char get_map_at(int x, int y);

void set_palette_entry(int idx, PaletteEntry p_new);
void copy_palette_to_edit_menu(int idx);
void copy_edit_menu_to_palette(int idx);

#endif
