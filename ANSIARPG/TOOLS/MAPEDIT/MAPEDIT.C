#include "mapedit.h"

UIConfig g_ui_config;
AppConfig g_app_config;

RenderComponents g_render_components;
PaletteEntry g_map_palette[NUM_PALETTE_ENTRIES];

unsigned char g_map[MAP_WIDTH][MAP_HEIGHT];

void wait_for_key(void) {
  while(!_bios_keybrd(_KEYBRD_READY)) {}
  _bios_keybrd(0x00);
}

void clear_map() {
    int i, j;
    for (i=0 ; i< MAP_WIDTH; i++) { 
        for (j=0; j < MAP_HEIGHT; j++) {
            g_map[i][j] = 0;
        }
    }
}

// Helper function - set the global components object to render all components
void set_all_render_components(void) {
    g_render_components.render_background = 1;
    g_render_components.render_menu = 1;
    g_render_components.render_palette = 1;
    g_render_components.render_palette_text = 1;
    g_render_components.render_palette_active = 1;
    g_render_components.render_exits_text = 1;
    g_render_components.render_exit_active = 1;
    g_render_components.render_map_area = 1;
}

// Helper function - disable the rendering of all components
void clear_render_components(void) {
    g_render_components.render_background = 0;
    g_render_components.render_menu = 0;
    g_render_components.render_palette = 0;
    g_render_components.render_palette_text = 0;
    g_render_components.render_palette_active = 0;
    g_render_components.render_exits_text = 0;
    g_render_components.render_exit_active = 0;
    g_render_components.render_map_area = 0;
}

void set_map_at(int x, int y, unsigned char palette_entry) {
    g_map[x][y] = palette_entry;
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

void set_palette_entry(int idx, PaletteEntry p_new) {
    g_map_palette[idx].id = idx;
    g_map_palette[idx].bg = p_new.bg;
    g_map_palette[idx].fg = p_new.fg;
    strncpy(g_map_palette[idx].name, p_new.name, 8);
    g_map_palette[idx].flags1 = p_new.flags1;
    g_map_palette[idx].flags2 = p_new.flags2;
}

void render() {
    int i, j, map_x, map_y, col;
    char buf[8];

    if (g_render_components.render_background) {
        // main box
        box_at(0, 1, 79, 24, BORDER_SINGLE, g_ui_config.background_attr);
        // top and bottom pieces of the center line
        char_at(62, 1, 194, g_ui_config.background_attr);
        char_at(62, 24, 193, g_ui_config.background_attr);
        // center line
        vline_at(62, 2, 22, 179, g_ui_config.background_attr);
        // left and right pieces of the palette area line
        char_at(62, 3, 195, g_ui_config.background_attr);
        char_at(79, 3, 180, g_ui_config.background_attr);   
        // The divider between the palette area and exit area
        hline_at(63, 13, 16, 196, g_ui_config.background_attr);
        char_at(62, 13, 195, g_ui_config.background_attr);
        char_at(79, 13, 180, g_ui_config.background_attr);    
    }
    if (g_render_components.render_menu) {
        hline_at(0, 0, 80, ' ', g_ui_config.menu_attr);        
    }
    if (g_render_components.render_map_area) {
        // Note - the map area will never scroll past the edges of the 
        // map.  That is, the lowest x and y values for the top left position
        // are 0, and the highest x and y values for the top left position 
        // are 150-MAP_AREA_WIDTH and 150-MAP_AREA_HEIGHT. Some other code
        // will enforce that (famous last words)
        map_x = g_app_config.map_x;
        map_y = g_app_config.map_y;
        for(i = 0; i < MAP_AREA_WIDTH ; i++) {
            map_x = g_app_config.map_x;
            for (j = 0; j < MAP_AREA_HEIGHT; j++) {
                col = g_map[map_x][map_y];
                char_at(i + MAP_AREA_X,
                        j + MAP_AREA_Y,
                        g_map_palette[col].glyph,
                        make_attr(g_map_palette[col].fg, g_map_palette[col].bg));
                map_x++;
            }
            map_y++;
        }
    }
    if (g_render_components.render_palette) {
        for (i=0; i < NUM_PALETTE_ENTRIES; i++ ) {
            char_at(i + PALETTE_AREA_X, PALETTE_AREA_Y,
                    g_map_palette[i].glyph,
                    make_attr(g_map_palette[i].fg, g_map_palette[i].bg));
        }
    }
    if (g_render_components.render_palette_active) {
        // redraw the palette area line and put an arrow at the current entry
        hline_at(PALETTE_AREA_X, PALETTE_AREA_Y+1, 16, 196, 
                 g_ui_config.background_attr);
        char_at(PALETTE_AREA_X + g_app_config.palette_entry, PALETTE_AREA_Y+1,
                24, make_attr(COLOR_YELLOW, COLOR_BLACK));
    }
    if (g_render_components.render_palette_text) {
        // -- Fixed components
        string_at(PALETTE_AREA_X + 1, 4, "Tile:", g_ui_config.background_attr);
        hline_at(PALETTE_AREA_X + 1, 5, 14, 205, g_ui_config.background_attr);
        string_at(PALETTE_AREA_X + 1, 6, "Name:", g_ui_config.background_attr);
        string_at(PALETTE_AREA_X + 1, 7, "FG color:", g_ui_config.background_attr);
        string_at(PALETTE_AREA_X + 1, 8, "BG color:", g_ui_config.background_attr);
        string_at(PALETTE_AREA_X + 1, 9, "Character:", g_ui_config.background_attr);   
        string_at(PALETTE_AREA_X + 1, 10, "Solid:", g_ui_config.background_attr); 
        string_at(PALETTE_AREA_X + 1, 11, "Damage:", g_ui_config.background_attr);                      
        // -- Dynamic components
        // Tile number
        itoa(g_map_palette[g_app_config.palette_entry].id, buf, 10);
        string_at(78 - strlen(buf), 4, buf, g_ui_config.background_attr);
        // Name
        string_at(78 - strlen(g_map_palette[g_app_config.palette_entry].name), 
                  6, g_map_palette[g_app_config.palette_entry].name,
                  g_ui_config.background_attr);
        // FG color
        itoa(g_map_palette[g_app_config.palette_entry].fg, buf, 10);
        string_at(78 - strlen(buf), 7, buf, g_ui_config.background_attr); 
        // BG color
        itoa(g_map_palette[g_app_config.palette_entry].bg, buf, 10);
        string_at(78 - strlen(buf), 8, buf, g_ui_config.background_attr); 
        // Character 
        itoa(g_map_palette[g_app_config.palette_entry].glyph, buf, 10);
        string_at(78 - strlen(buf), 9, buf, g_ui_config.background_attr); 
        // Flags (just hard coded for now)
        string_at(76, 10, "No", g_ui_config.background_attr);
        string_at(74, 11, "None", g_ui_config.background_attr);                               
    }

    clear_render_components();
}

int main(void) {

    PaletteEntry p;

    initialize_palette();

    set_text_mode(MODE_80X25);
    set_bg_intensity(1);

    clear_screen();
    hide_cursor();

    clear_map();
    initialize_attributes();
    set_all_render_components();

    g_map_palette[0].glyph = 177;
    strncpy(g_map_palette[0].name, "TILE1", 8);
    render();

    wait_for_key();
    clear_screen();

    show_cursor();
    set_bg_intensity(0);
    return 0;
}
