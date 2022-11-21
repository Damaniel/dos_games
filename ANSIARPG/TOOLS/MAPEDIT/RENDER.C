#include "mapedit.h"

RenderComponents g_render_components;

void render() {
    int i, j, map_x, map_y, col, cur_attr;
    char buf[16];
    char rendered = 0;

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
        g_render_components.render_background = 0;
    }
    if (g_render_components.render_menu) {
        hline_at(0, 0, 80, ' ', g_ui_config.menu_attr);  
        g_render_components.render_menu = 0;           
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
            map_y = g_app_config.map_y;
            for (j = 0; j < MAP_AREA_HEIGHT; j++) {
                col = g_map[map_x][map_y];
                char_at(i + MAP_AREA_X,
                        j + MAP_AREA_Y,
                        g_map_palette[col].glyph,
                        make_attr(g_map_palette[col].fg, g_map_palette[col].bg));
                map_y++;
            }
            map_x++;
        }
        g_render_components.render_map_area = 0;     
    }
    if (g_render_components.render_palette) {
        for (i=0; i < NUM_PALETTE_ENTRIES; i++ ) {
            char_at(i + PALETTE_AREA_X, PALETTE_AREA_Y,
                    g_map_palette[i].glyph,
                    make_attr(g_map_palette[i].fg, g_map_palette[i].bg));
        }
        g_render_components.render_palette = 0;   
    }
    if (g_render_components.render_palette_active) {
        // redraw the palette area line and put an arrow at the current entry
        hline_at(PALETTE_AREA_X, PALETTE_AREA_Y+1, 16, 196, 
                 g_ui_config.background_attr);
        char_at(PALETTE_AREA_X + g_app_config.palette_entry, PALETTE_AREA_Y+1,
                24, make_attr(COLOR_YELLOW, COLOR_BLACK));
        g_render_components.render_palette_active = 0;
    }
    if (g_render_components.render_palette_text) {
        // -- Fixed components
        string_at(PALETTE_AREA_X + 1, 4, "Tile:         ", g_ui_config.background_attr);
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
        g_render_components.render_palette_text = 0;        
    }
    if (g_render_components.render_exits_text) {
        string_at(PALETTE_AREA_X + 5, 14, "Exits", g_ui_config.background_attr);
        hline_at(PALETTE_AREA_X + 1, 15, 14, 205, g_ui_config.background_attr);
        for(i = 0; i < NUM_EXITS; i++) {
            if (g_exit_list[i].is_set) {
                sprintf(buf, "%d - Set", i+1);
            } else {
                sprintf(buf, "%d - <empty>", i+1);
            }
            string_at(PALETTE_AREA_X + 1, 16+i, buf, g_ui_config.background_attr);
        }
        hline_at(PALETTE_AREA_X + 1, 21, 14, 205, g_ui_config.background_attr);  
        g_render_components.render_exits_text = 0;             
    }
    if (g_render_components.render_exit_active) {
        char_at(PALETTE_AREA_X, 16 + g_app_config.exit_entry, 16, 
                make_attr(COLOR_YELLOW, COLOR_BLACK));
        if (g_exit_list[g_app_config.exit_entry].is_set) {
            sprintf(buf, "%d (%d,%d)", 
                    g_exit_list[g_app_config.exit_entry].target_room,
                    g_exit_list[g_app_config.exit_entry].x_pos,
                    g_exit_list[g_app_config.exit_entry].y_pos);
        } else {
            sprintf(buf, "              ");
        }
        string_at(PALETTE_AREA_X+1, 22, buf, g_ui_config.background_attr);
        g_render_components.render_exit_active = 0;        
    }
    if (g_render_components.render_cursor_position) {
        hline_at(1, 24, 10, 196, g_ui_config.background_attr);
        sprintf(buf, "(%d,%d)", g_app_config.map_x + g_app_config.cursor_x - MAP_AREA_X, 
                g_app_config.map_y + g_app_config.cursor_y - MAP_AREA_Y);
        string_at(1, 24, buf, g_ui_config.background_attr);
        g_render_components.render_cursor_position = 0;       
    }
    if (g_render_components.render_cursor) {
        // Replace the old cursor location with the map location that goes
        // under it
        map_x = g_app_config.map_x + g_app_config.old_cursor_x - MAP_AREA_X;
        map_y = g_app_config.map_y + g_app_config.old_cursor_y - MAP_AREA_Y;
        col = get_map_at(map_x, map_y);
        char_at(g_app_config.old_cursor_x,
                g_app_config.old_cursor_y,
                g_map_palette[col].glyph,
                make_attr(g_map_palette[col].fg, g_map_palette[col].bg));

        // Draw the cursor in the new location
        map_x = g_app_config.map_x + g_app_config.cursor_x - MAP_AREA_X;
        map_y = g_app_config.map_y + g_app_config.cursor_y - MAP_AREA_Y;        
        col = get_map_at(map_x, map_y);
        if (col == 0) {
            cur_attr = make_attr(7, 0);;
        } else {
            cur_attr = make_attr(14, 0);
        }        
        char_at(g_app_config.cursor_x, g_app_config.cursor_y, 219, cur_attr);
        g_render_components.render_cursor = 0;        
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
    g_render_components.render_cursor_position = 1;
    g_render_components.render_cursor = 1;
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
    g_render_components.render_cursor_position = 0;
    g_render_components.render_cursor = 0;
}
