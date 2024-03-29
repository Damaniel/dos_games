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
    g_ui_config.help_background_attr = make_attr(COLOR_WHITE, COLOR_BLUE);
    g_ui_config.help_text_attr = make_attr(COLOR_WHITE, COLOR_BLUE);
    g_ui_config.help_highlight_attr = make_attr(COLOR_YELLOW, COLOR_BLUE);
    g_ui_config.failed_background_attr = make_attr(COLOR_WHITE, COLOR_RED);
    g_ui_config.failed_text_attr = make_attr(COLOR_WHITE, COLOR_RED);
    g_ui_config.failed_highlight_attr = make_attr(COLOR_YELLOW, COLOR_RED);
}

void initialize_palette(void) {
    int i;

    for(i=0; i < NUM_PALETTE_ENTRIES; i++ ) {
        g_map_palette[i].id = i;
        g_map_palette[i].bg = 0;
        g_map_palette[i].fg = 15;
        // For the first redefineable tile (tile 1), set it to a resonable default
        // (solid white, solid, no damage).  All other tiles are blank.
        if(i == 1) {
            g_map_palette[i].glyph = 219;
            set_palette_flags(i, FLAG_SOLID, DT_NONE);
            strncpy(g_map_palette[i].name, "Wall    ", 8);
        } else {
            g_map_palette[i].glyph = 0;
            set_palette_flags(i, FLAG_PASSABLE, DT_NONE);
            strncpy(g_map_palette[i].name, "        ", 8);
        }
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
    g_palette_menu_config.active_item = PI_NAME;
}

void copy_palette_to_edit_menu(int idx) {
    g_palette_menu_config.foreground = g_map_palette[idx].fg;
    g_palette_menu_config.background = g_map_palette[idx].bg;
    strncpy(g_palette_menu_config.name, g_map_palette[idx].name, 8);
    g_palette_menu_config.character = g_map_palette[idx].glyph;
    g_palette_menu_config.solid = get_palette_solid_value(idx);
    g_palette_menu_config.damage_type = (DamageType)(get_palette_damage_value(idx));
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

void initialize_map_header(void) {
    int i;

    g_map_header.magic[0] = 'D';
    g_map_header.magic[1] = 'H';
    g_map_header.major_version = 0;
    g_map_header.minor_version = 1;
    for(i=0; i<20; i++) {
        g_map_header.name[i] = ' ';
    }
    g_map_header.map_width = MAP_WIDTH;
    g_map_header.map_height = MAP_HEIGHT;
    g_map_header.compression = COMPRESSION_NONE;
    g_map_header.pc_start_x = 2;
    g_map_header.pc_start_y = 2;
}

int read_map_file(char *filename) {
    FILE *fp;
    int i, j;
    char data, data2, magic, magic2;

    // Open the file
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        // file doesn't exist.  Initialize the map, palette and header 
        // data and return a non-zero (but not an 'error')
        if (errno == ENOENT) {
            return 1;
        }
        else {
            return -1;
        }
    }

    // Read the header if a valid file
    fread(&magic, sizeof(char), 1, fp);
    fread(&magic2, sizeof(char), 1, fp);

    if(magic != 'D' || magic2 != 'H') {
        return -1;
    }

    fread(&(g_map_header.major_version), sizeof(char), 1, fp);
    fread(&(g_map_header.minor_version), sizeof(char), 1, fp);
    fread(&(g_map_header.map_id), sizeof(short), 1, fp);
    fread(&(g_map_header.name[0]), sizeof(char), MAP_NAME_LENGTH, fp);
    fread(&(g_map_header.map_width), sizeof(char), 1, fp);
    fread(&(g_map_header.map_height), sizeof(char), 1, fp);
    fread(&(g_map_header.compression), sizeof(char), 1, fp);
    fread(&(g_map_header.pc_start_x), sizeof(char), 1, fp);
    fread(&(g_map_header.pc_start_y), sizeof(char), 1, fp);
    fread(&(g_map_header.padding), sizeof(char), 97, fp);

    // Read and set the palette data
    for (i=0; i< NUM_PALETTE_ENTRIES; i++ ) {
        fread(&(g_map_palette[i].id), sizeof(char), 1, fp);
        fread(&(g_map_palette[i].name[0]), sizeof(char), 8, fp);
        g_map_palette[i].name[8] = '\0';
        fread(&(g_map_palette[i].glyph), sizeof(char), 1, fp);
        fread(&(g_map_palette[i].fg), sizeof(char), 1, fp);
        fread(&(g_map_palette[i].bg), sizeof(char), 1, fp);
        fread(&(g_map_palette[i].flags1), sizeof(char), 1, fp);
        fread(&(g_map_palette[i].flags2), sizeof(char), 1, fp);
    }

    // Read and set the map data
    switch(g_map_header.compression) {
        case COMPRESSION_NONE:
        case COMPRESSION_RLE:
            for(j=0; j < MAP_HEIGHT; j++) {
                for(i=0; i < MAP_WIDTH; i++) {
                    fread(&(g_map[i][j]), sizeof(char), 1, fp);
                }
            }
            break;
        case COMPRESSION_PACKED:
        case COMPRESSION_PACKED_RLE:
            for(j=0; j < MAP_HEIGHT; j++) {
                for(i=0; i < (MAP_WIDTH >> 1); i++) {
                    fread(&data, sizeof(char), 1, fp);
                    fread(&data2, sizeof(char), 1, fp);
                    g_map[i<<1][j] = data;
                    g_map[(i<<1)+1][j] = data2;
                }
            }
            break;
    }

    // Close the file
    fclose(fp);

    return 0;
}

int write_map_file(char *filename) {
    // For now, no RLE.
    FILE *fp;
    int i, j;
    char data;

    // Open the file
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        return -1;
    }

    // // Write the header
    fwrite(&(g_map_header.magic[0]), sizeof(char), 1, fp);
    fwrite(&(g_map_header.magic[1]), sizeof(char), 1, fp);
    fwrite(&(g_map_header.major_version), sizeof(char), 1, fp);
    fwrite(&(g_map_header.minor_version), sizeof(char), 1, fp);
    fwrite(&(g_map_header.map_id), sizeof(short), 1, fp);
    fwrite(&(g_map_header.name[0]), sizeof(char), MAP_NAME_LENGTH, fp);
    fwrite(&(g_map_header.map_width), sizeof(char), 1, fp);
    fwrite(&(g_map_header.map_height), sizeof(char), 1, fp);
    fwrite(&(g_map_header.compression), sizeof(char), 1, fp);
    fwrite(&(g_map_header.pc_start_x), sizeof(char), 1, fp);
    fwrite(&(g_map_header.pc_start_y), sizeof(char), 1, fp);
    fwrite(&(g_map_header.padding[0]), sizeof(char), 97, fp);

    // Write the palette
    for (i=0; i< NUM_PALETTE_ENTRIES; i++ ) {
        fwrite(&(g_map_palette[i].id), sizeof(char), 1, fp);
        fwrite(&(g_map_palette[i].name[0]), sizeof(char), 8, fp);
        fwrite(&(g_map_palette[i].glyph), sizeof(char), 1, fp);
        fwrite(&(g_map_palette[i].fg), sizeof(char), 1, fp);
        fwrite(&(g_map_palette[i].bg), sizeof(char), 1, fp);
        fwrite(&(g_map_palette[i].flags1), sizeof(char), 1, fp);
        fwrite(&(g_map_palette[i].flags2), sizeof(char), 1, fp);
    }
    // Write the map data
    switch(g_map_header.compression) {
        case COMPRESSION_NONE:
        case COMPRESSION_RLE:
            for(j=0; j < MAP_HEIGHT; j++) {
                for(i=0; i < MAP_WIDTH; i++) {
                    fwrite(&(g_map[i][j]), sizeof(char), 1, fp);
                }
            }
            break;
            break;
        case COMPRESSION_PACKED:
        case COMPRESSION_PACKED_RLE:
            for(j=0; j < MAP_HEIGHT; j++) {
                for(i=0; i < MAP_WIDTH; i+=2) {
                    data = (g_map[i][j] << 4) | g_map[i+1][j];
                    fwrite(&data, sizeof(char), 1, fp);
                }
            }
            break;
    }
    // Close the file
    fclose(fp);
    return 0;
}
