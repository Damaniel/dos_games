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

RenderComponents g_render_components;
RenderPaletteEditComponents g_render_palette_edit_components;

void render_main_screen(void) {
    int i, j, map_x, map_y, col, cur_attr;
    char buf[16];
    char rendered = 0;
    char damage, solid;

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
        sprintf(buf, "ANSIARPG Map Editor v%s", APP_VERSION);
        string_at(2, 0, buf, g_ui_config.menu_attr);
        string_at(62, 0, "Press H for help", g_ui_config.menu_attr);
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
        damage = get_palette_damage_value(g_app_config.palette_entry);
        solid = get_palette_solid_value(g_app_config.palette_entry);
        if (solid == FLAG_SOLID) {
            string_at(75, 10, "Yes", g_ui_config.background_attr);
        } else {
            string_at(75, 10, " No", g_ui_config.background_attr);
        }
        switch (damage) {
            case FLAG_DAMAGE_NONE:
                string_at(72, 11, "  None", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_LOW:
                string_at(72, 11, "   Low", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_MEDIUM:
                string_at(72, 11, "Medium", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_HIGH:
                string_at(72, 11, "  High", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_OHKO:
                string_at(72, 11, "  OHKO", g_ui_config.background_attr);
                break;
            default:
                string_at(72, 11, "   ???", g_ui_config.background_attr);
                break;                
        }
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
                    g_exit_list[g_app_config.exit_entry].target_x_pos,
                    g_exit_list[g_app_config.exit_entry].target_y_pos);
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

void render_palette_edit_screen(void) {
    char buf[32];
    int i, x_off;

    if(g_render_palette_edit_components.render_main) {
        // Render background
        fill_box_at(MAP_AREA_X, 
                    MAP_AREA_Y, 
                    MAP_AREA_X + MAP_AREA_WIDTH - 1,
                    MAP_AREA_Y + MAP_AREA_HEIGHT - 1,
                    177,
                    make_attr(9, 1));

        // Render menu box
        box_at(PALETTE_EDIT_X, PALETTE_EDIT_Y,
               PALETTE_EDIT_X + PALETTE_EDIT_WIDTH - 1,
               PALETTE_EDIT_Y + PALETTE_EDIT_HEIGHT -1,
               BORDER_SINGLE, g_ui_config.background_attr);
        fill_box_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 1,
                PALETTE_EDIT_X + PALETTE_EDIT_WIDTH - 2,
                PALETTE_EDIT_Y + PALETTE_EDIT_HEIGHT -2,
                ' ', g_ui_config.background_attr);
        hline_at(PALETTE_EDIT_X, PALETTE_EDIT_Y + 2, PALETTE_EDIT_WIDTH, 196, 
                 g_ui_config.background_attr);
        hline_at(PALETTE_EDIT_X, PALETTE_EDIT_Y + 9, PALETTE_EDIT_WIDTH, 196,
                 g_ui_config.background_attr); 
        hline_at(PALETTE_EDIT_X, PALETTE_EDIT_Y + 11, PALETTE_EDIT_WIDTH, 196, 
                 g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X, PALETTE_EDIT_Y + 2, 195, 
                g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + PALETTE_EDIT_WIDTH - 1, PALETTE_EDIT_Y + 2, 180, 
                g_ui_config.background_attr);    
        char_at(PALETTE_EDIT_X, PALETTE_EDIT_Y + 9, 195, 
                g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + PALETTE_EDIT_WIDTH - 1, PALETTE_EDIT_Y + 9, 180, 
                g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X, PALETTE_EDIT_Y + 11, 195, 
                g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + PALETTE_EDIT_WIDTH - 1, PALETTE_EDIT_Y + 11, 180, 
                g_ui_config.background_attr);               
            
        // render text items
        if (g_app_config.palette_entry < 10) {
            sprintf(buf, " -- Editing tile %d --", g_app_config.palette_entry);
        } else {
            sprintf(buf, " -- Editing tile %d -", g_app_config.palette_entry);        
        }
        string_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 1, buf, g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 3, "Name:", g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 4, "Foreground:", g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 5, "Background:", g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 6, "Character:", g_ui_config.background_attr);    
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 7, "Solid?", g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 8, "Damage:", g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 10, "Preview:         [ ]", g_ui_config.background_attr);
        string_at(PALETTE_EDIT_X + 2, PALETTE_EDIT_Y + 12, "[ OK ]    [ Cancel ]", g_ui_config.background_attr); 

        g_render_palette_edit_components.render_main = 0;   
    }

    // render values
    if (g_render_palette_edit_components.render_name) {
        char_at(PALETTE_EDIT_X + 12, PALETTE_EDIT_Y + 3, '[', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 21, PALETTE_EDIT_Y + 3, ']', g_ui_config.background_attr);        
        for(i = 0; i < PALETTE_ITEM_NAME_LENGTH; i++) {
            buf[i] = g_palette_menu_config.name[i];
        }
        // Mark the end of the string
        buf[PALETTE_ITEM_NAME_LENGTH] = '\0';
        string_at(PALETTE_EDIT_X + 13, PALETTE_EDIT_Y + 3, buf, g_ui_config.background_attr);

        g_render_palette_edit_components.render_name = 0;
    }

    if (g_render_palette_edit_components.render_foreground) {
        if (g_palette_menu_config.foreground < 10) {
            x_off = PALETTE_EDIT_X + 21;
        } else {
            x_off = PALETTE_EDIT_X + 20;
        }
        itoa(g_palette_menu_config.foreground, buf, 10);
        string_at(PALETTE_EDIT_X + 20, PALETTE_EDIT_Y + 4, "  ", g_ui_config.background_attr);
        string_at(x_off, PALETTE_EDIT_Y + 4, buf, g_ui_config.background_attr);

        g_render_palette_edit_components.render_foreground = 0;
    }

    if (g_render_palette_edit_components.render_background) {
        if (g_palette_menu_config.background < 10) {
            x_off = PALETTE_EDIT_X + 21;
        } else {
            x_off = PALETTE_EDIT_X + 20;
        }
        itoa(g_palette_menu_config.background, buf, 10);
        string_at(PALETTE_EDIT_X + 20, PALETTE_EDIT_Y + 5, "  ", g_ui_config.background_attr);        
        string_at(x_off, PALETTE_EDIT_Y + 5, buf, g_ui_config.background_attr);

        g_render_palette_edit_components.render_background = 0;
    }

    if (g_render_palette_edit_components.render_character) {
        if (g_palette_menu_config.character < 10) {
            x_off = PALETTE_EDIT_X + 21;
        } else if (g_palette_menu_config.character < 100){
            x_off = PALETTE_EDIT_X + 20;
        } else {
            x_off = PALETTE_EDIT_X + 19;
        }
        itoa(g_palette_menu_config.character, buf, 10);
        string_at(PALETTE_EDIT_X + 19, PALETTE_EDIT_Y + 6, "   ", g_ui_config.background_attr);
        string_at(x_off, PALETTE_EDIT_Y + 6, buf, g_ui_config.background_attr);

        g_render_palette_edit_components.render_character = 0;
    }

    if (g_render_palette_edit_components.render_solid) {
        if (g_palette_menu_config.solid == FLAG_SOLID) {
            string_at(PALETTE_EDIT_X + 19, PALETTE_EDIT_Y + 7, "[X]", g_ui_config.background_attr);
        } else {
            string_at(PALETTE_EDIT_X + 19, PALETTE_EDIT_Y + 7, "[ ]", g_ui_config.background_attr);
        }
        g_render_palette_edit_components.render_solid = 0;
    }

    if (g_render_palette_edit_components.render_damage) {
        switch (g_palette_menu_config.damage_type) {
            case FLAG_DAMAGE_NONE:
                string_at(PALETTE_EDIT_X + 16, PALETTE_EDIT_Y + 8, "  None", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_LOW:
                string_at(PALETTE_EDIT_X + 16, PALETTE_EDIT_Y + 8, "   Low", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_MEDIUM:
                string_at(PALETTE_EDIT_X + 16, PALETTE_EDIT_Y + 8, "Medium", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_HIGH:
                string_at(PALETTE_EDIT_X + 16, PALETTE_EDIT_Y + 8, "  High", g_ui_config.background_attr);
                break;
            case FLAG_DAMAGE_OHKO:
                string_at(PALETTE_EDIT_X + 16, PALETTE_EDIT_Y + 8, "  OHKO", g_ui_config.background_attr);
                break;
        }  
        g_render_palette_edit_components.render_damage = 0;  
    }

    if (g_render_palette_edit_components.render_preview) {
        char_at(PALETTE_EDIT_X + 20, PALETTE_EDIT_Y + 10,
                g_palette_menu_config.character,
                make_attr(g_palette_menu_config.foreground,
                          g_palette_menu_config.background));

        g_render_palette_edit_components.render_preview = 0;
    }

    // render current highlighted item
    if (g_render_palette_edit_components.render_active_item) {
        // clear all the locations the arrow can be
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 3, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 4, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 5, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 6, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 7, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 8, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 12, ' ', g_ui_config.background_attr);
        char_at(PALETTE_EDIT_X + 11, PALETTE_EDIT_Y + 12, ' ', g_ui_config.background_attr);

        // Draw the new arrow in the correct place
        switch (g_palette_menu_config.active_item) {
            case PI_NAME:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 3, 16, g_ui_config.highlight_attr);
                char_at(PALETTE_EDIT_X + 13 + g_palette_menu_config.name_idx, PALETTE_EDIT_Y + 3, 
                        g_palette_menu_config.name[g_palette_menu_config.name_idx], make_attr(15, 7));
                break;
            case PI_FOREGROUND:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 4, 16, g_ui_config.highlight_attr);            
                break;
            case PI_BACKGROUND:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 5, 16, g_ui_config.highlight_attr);            
                break;
            case PI_CHARACTER:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 6, 16, g_ui_config.highlight_attr);            
                break;
            case PI_SOLID:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 7, 16, g_ui_config.highlight_attr);            
                break;
            case PI_DAMAGE:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 8, 16, g_ui_config.highlight_attr);            
                break;
            case PI_OK:
                char_at(PALETTE_EDIT_X + 1, PALETTE_EDIT_Y + 12, 16, g_ui_config.highlight_attr);
                break;
            case PI_CANCEL:
                char_at(PALETTE_EDIT_X + 11, PALETTE_EDIT_Y + 12, 16, g_ui_config.highlight_attr);
                break;
            default:
                break;
        }
        g_render_palette_edit_components.render_active_item = 0;
    }
}


void render() {
    switch(g_state) {
        case MAIN_SCREEN:
            render_main_screen();
            break;
        case PALETTE_EDIT:
            render_palette_edit_screen();
            break;
        default:
            break;
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

void set_all_palette_edit_components(void) {
    g_render_palette_edit_components.render_main = 1;
    g_render_palette_edit_components.render_background = 1;
    g_render_palette_edit_components.render_name = 1;
    g_render_palette_edit_components.render_foreground = 1;
    g_render_palette_edit_components.render_background = 1;
    g_render_palette_edit_components.render_character = 1;
    g_render_palette_edit_components.render_solid = 1;
    g_render_palette_edit_components.render_damage = 1;
    g_render_palette_edit_components.render_preview = 1;
    g_render_palette_edit_components.render_active_item = 1;
}

void clear_all_palette_edit_components(void) {
    g_render_palette_edit_components.render_main = 0;
    g_render_palette_edit_components.render_background = 0;
    g_render_palette_edit_components.render_name = 0;
    g_render_palette_edit_components.render_foreground = 0;
    g_render_palette_edit_components.render_background = 0;
    g_render_palette_edit_components.render_character = 0;
    g_render_palette_edit_components.render_solid = 0;
    g_render_palette_edit_components.render_damage = 0;
    g_render_palette_edit_components.render_active_item = 0;
}
