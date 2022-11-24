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
#ifndef __RENDER_H__
#define __RENDER_H__

typedef struct {
    char render_background;
    char render_menu;
    char render_palette;
    char render_palette_text;
    char render_palette_active;
    char render_exits_text;
    char render_exit_active;
    char render_map_area;
    char render_cursor_position;
    char render_cursor;
} RenderComponents;

typedef struct {
    char render_main;
    char render_name;
    char render_foreground;
    char render_background;
    char render_character;
    char render_solid;
    char render_damage;
    char render_preview;
    char render_active_item;
} RenderPaletteEditComponents;

typedef struct {
    char render_background;
    char render_text;
    char render_page_markers;
} RenderHelpScreenComponents;

extern RenderComponents g_render_components;
extern RenderPaletteEditComponents g_render_palette_edit_components;
extern RenderHelpScreenComponents g_render_help_screen_components;

void render(void);
void set_all_render_components(void);
void clear_render_components(void);
void set_all_palette_edit_components(void);
void clear_all_palette_edit_components(void);
void render_main_screen(void);
void render_palette_edit_screen(void);
void render_help_screen(void);
void clear_all_help_screen_components(void);
void set_all_help_screen_components(void);

#endif
