#include "mapedit.h"

UIConfig g_ui_config;
AppConfig g_app_config;

PaletteEntry g_map_palette[NUM_PALETTE_ENTRIES];
Exit g_exit_list[NUM_EXITS];

unsigned char g_map[MAP_WIDTH][MAP_HEIGHT];

void clear_map() {
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
    g_exit_list[index].x_pos = x_pos;
    g_exit_list[index].y_pos = y_pos;
}

void clear_exit(int index) {
    if (index < 0 || index >= NUM_EXITS) {
        return;
    }
    g_exit_list[index].is_set = 0;
    g_exit_list[index].target_room = 0;
    g_exit_list[index].x_pos = 0;
    g_exit_list[index].y_pos = 0;
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

void set_palette_entry(int idx, PaletteEntry p_new) {
    g_map_palette[idx].id = idx;
    g_map_palette[idx].bg = p_new.bg;
    g_map_palette[idx].fg = p_new.fg;
    strncpy(g_map_palette[idx].name, p_new.name, 8);
    g_map_palette[idx].flags1 = p_new.flags1;
    g_map_palette[idx].flags2 = p_new.flags2;
}

int main(void) {

    PaletteEntry p;

    initialize_palette();
    initialize_exits();
    initialize_app_defaults();

    set_text_mode(MODE_80X25);
    set_bg_intensity(1);

    set_exit(0, 23, 17, 32);

    clear_screen();
    hide_cursor();

    clear_map();
    initialize_attributes();
    set_all_render_components();

    g_map_palette[1].glyph = 177;
    g_map_palette[1].fg = 11;
    g_map_palette[1].bg = 1;

    while(!g_app_config.quit) {
        process_input();
        render();
    }
    clear_screen();

    show_cursor();
    set_bg_intensity(0);
    return 0;
}
