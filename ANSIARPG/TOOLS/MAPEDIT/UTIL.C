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
#include "mapedit.h"

void clear_map(void) {
    int i, j;
    for (i=0 ; i< MAP_WIDTH; i++) { 
        for (j=0; j < MAP_HEIGHT; j++) {
            g_map[i][j] = 0;
        }
    }
}

void set_exit(int index, int room_id, int x_pos, int y_pos) { 
    if (index < 0 || index >= NUM_EXITS) {
        return;
    }
    g_exit_list[index].is_set = 1;
    g_exit_list[index].target_room = room_id;
    g_exit_list[index].target_x_pos = x_pos;
    g_exit_list[index].target_y_pos = y_pos;
}

void clear_exit(int index) {
    if (index < 0 || index >= NUM_EXITS) {
        return;
    }
    g_exit_list[index].is_set = 0;
    g_exit_list[index].target_room = 0;
    g_exit_list[index].target_x_pos = 0;
    g_exit_list[index].target_y_pos = 0;
}

void initialize_exits(void) {
    int i;
    for (i = 0; i < NUM_EXITS; i++) {
        clear_exit(i);
    }
}

void set_map_at(int x, int y, unsigned char palette_entry) {
    g_map[x][y] = palette_entry;
}

char get_map_at(int x, int y) {
    return g_map[x][y];
}

void initialize_attributes(void) {
    g_ui_config.background_attr = make_attr(COLOR_WHITE, COLOR_BLACK);
    g_ui_config.menu_attr = make_attr(COLOR_WHITE, COLOR_BLUE);
    g_ui_config.highlight_attr = make_attr(COLOR_YELLOW, COLOR_BLACK);
}

void initialize_palette(void) {
    int i;

    for(i=0; i < NUM_PALETTE_ENTRIES; i++ ) {
        g_map_palette[i].id = i;
        g_map_palette[i].bg = 0;
        g_map_palette[i].fg = 15;
        strncpy(g_map_palette[i].name, "        ", 8);
        g_map_palette[i].flags1 = 0x00;
        g_map_palette[i].flags2 = 0x00;
    }
}

void initialize_app_defaults(void) {
    g_app_config.palette_entry = 1;
    g_app_config.quit = 0;
    g_app_config.old_cursor_x = MAP_AREA_X;
    g_app_config.old_cursor_y = MAP_AREA_Y;
    g_app_config.cursor_x = 1;
    g_app_config.cursor_y = 2;    
}

void initialize_palette_menu_defaults(void) {
    g_palette_menu_config.name_idx = 0;
    strncpy(g_palette_menu_config.name, "        ", 8);
    g_palette_menu_config.active_item = PI_NAME;
    g_palette_menu_config.background = 0;
    g_palette_menu_config.foreground = 15;
    g_palette_menu_config.character = 65;
    g_palette_menu_config.solid = 1;
}

void copy_palette_to_edit_menu(int idx) {
    g_palette_menu_config.foreground = g_map_palette[idx].fg;
    g_palette_menu_config.background = g_map_palette[idx].bg;
    strncpy(g_palette_menu_config.name, g_map_palette[idx].name, 8);
    g_palette_menu_config.character = g_map_palette[idx].glyph;
    g_palette_menu_config.solid = get_palette_solid_value(idx);
    g_palette_menu_config.damage_type = get_palette_damage_value(idx);
}

void copy_edit_menu_to_palette(int idx) {
    g_map_palette[idx].fg = g_palette_menu_config.foreground;
    g_map_palette[idx].bg = g_palette_menu_config.background;
    strncpy(g_map_palette[idx].name, g_palette_menu_config.name, 8);
    g_map_palette[idx].glyph = g_palette_menu_config.character;
    set_palette_flags(idx, g_palette_menu_config.solid, g_palette_menu_config.damage_type);
}

void set_palette_entry(int idx, PaletteEntry p_new) {
    g_map_palette[idx].id = idx;
    g_map_palette[idx].bg = p_new.bg;
    g_map_palette[idx].fg = p_new.fg;
    strncpy(g_map_palette[idx].name, p_new.name, 8);
    g_map_palette[idx].flags1 = p_new.flags1;
    g_map_palette[idx].flags2 = p_new.flags2;
}

void set_palette_flags(int index, char solid_value, char damage_value) {
    g_map_palette[index].flags1 = solid_value | (damage_value << 1);
}

char get_palette_damage_value(int index) {
    return (g_map_palette[index].flags1 & FLAG_DAMAGE_MASK) >> 1;
}

char get_palette_solid_value(int index) {
    return (g_map_palette[index].flags1 & FLAG_SOLID_MASK);
}
