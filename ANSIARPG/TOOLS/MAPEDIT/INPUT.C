#include "mapedit.h"

void process_input(void) {
    unsigned short key, shift_status;
    unsigned char ascii_code, scan_code;
    int x, y;

    // Get the shift state since it's non-blocking
    shift_status = _bios_keybrd(_KEYBRD_SHIFTSTATUS);
    // Is there a key ready?
    if (_bios_keybrd(_KEYBRD_READY)) {
        key = _bios_keybrd(_KEYBRD_READ);
        ascii_code = (key & 0xFF);
        scan_code = (key >> 8);
        switch(scan_code) {
            case KEY_ESC:
                g_app_config.quit = 1;
                break;
            case KEY_RIGHT:
                // move the cursor right
                g_app_config.old_cursor_x = g_app_config.cursor_x;
                g_app_config.old_cursor_y = g_app_config.cursor_y;
                g_app_config.cursor_x = g_app_config.cursor_x + 1;
                if (g_app_config.cursor_x > MAX_SCREEN_CURSOR_X) {
                    // if all the way over, don't move the cursor, but move the map
                    g_app_config.cursor_x = MAX_SCREEN_CURSOR_X;
                    g_app_config.map_x = g_app_config.map_x + 1;
                    // If past the end of the map, don't move the map either.
                    // If the map *has* moved, redraw it.
                    if(g_app_config.map_x > MAX_SCREEN_MAP_X) {
                        g_app_config.map_x = MAX_SCREEN_MAP_X;
                        g_render_components.render_map_area = 0;
                    } else {
                        g_render_components.render_map_area = 1;
                    }
                }
                g_render_components.render_cursor = 1;
                g_render_components.render_cursor_position = 1;
                break;
            case KEY_LEFT:
                // move the cursor left
                g_app_config.old_cursor_x = g_app_config.cursor_x;
                g_app_config.old_cursor_y = g_app_config.cursor_y;
                g_app_config.cursor_x = g_app_config.cursor_x - 1;            
                if (g_app_config.cursor_x < MIN_SCREEN_CURSOR_X) {
                    // if all the way over, don't move the cursor, but move the map...
                    g_app_config.cursor_x = MIN_SCREEN_CURSOR_X;
                    // ... but only if there's room.     
                    // If the map *has* moved, redraw it.                                   
                    if(g_app_config.map_x > MIN_SCREEN_MAP_X) {
                        g_app_config.map_x = g_app_config.map_x - 1;
                        g_render_components.render_map_area = 1;
                    } else {
                        g_render_components.render_map_area = 0;
                    }
                }
                g_render_components.render_cursor = 1;
                g_render_components.render_cursor_position = 1;
                break;
            case KEY_UP:
                // move the cursor left
                g_app_config.old_cursor_x = g_app_config.cursor_x;
                g_app_config.old_cursor_y = g_app_config.cursor_y;
                g_app_config.cursor_y = g_app_config.cursor_y - 1;
                if (g_app_config.cursor_y < MIN_SCREEN_CURSOR_Y) {
                    // if all the way over, don't move the cursor, but move the map...
                    g_app_config.cursor_y = MIN_SCREEN_CURSOR_Y;
                    // ... but only if there's room.                    
                    if(g_app_config.map_y > MIN_SCREEN_MAP_Y) {
                        g_app_config.map_y = g_app_config.map_y - 1;
                        g_render_components.render_map_area = 1;
                    } else {
                        g_render_components.render_background = 0;
                    }
                }
                g_render_components.render_cursor = 1;
                g_render_components.render_cursor_position = 1;
                break;     
            case KEY_DOWN:
                // move the cursor right
                g_app_config.old_cursor_x = g_app_config.cursor_x;
                g_app_config.old_cursor_y = g_app_config.cursor_y;
                g_app_config.cursor_y = g_app_config.cursor_y + 1;
                if (g_app_config.cursor_y > MAX_SCREEN_CURSOR_Y) {
                    // if all the way over, don't move the cursor, but move the map
                    g_app_config.cursor_y = MAX_SCREEN_CURSOR_Y;
                    g_app_config.map_y = g_app_config.map_y + 1;
                    // If past the end of the map, don't move the map either.
                    if(g_app_config.map_y > MAX_SCREEN_MAP_Y) {
                        g_app_config.map_y = MAX_SCREEN_MAP_Y;
                        g_render_components.render_map_area = 0;
                    } else {
                        g_render_components.render_map_area = 1;
                    }
                }
                g_render_components.render_cursor = 1;
                g_render_components.render_cursor_position = 1;
                break;
            // Note that entry 0 can't be selected - it's always blank.
            case KEY_LBRACKET:
                if (g_app_config.palette_entry > MIN_PALETTE_ENTRY) {
                    g_app_config.palette_entry = g_app_config.palette_entry - 1;                    
                } else {
                    g_app_config.palette_entry = MAX_PALETTE_ENTRY;
                }
                g_render_components.render_palette_active = 1;
                g_render_components.render_palette_text = 1;                 
                break;
            case KEY_RBRACKET:
                g_app_config.palette_entry = g_app_config.palette_entry + 1;
                if (g_app_config.palette_entry > MAX_PALETTE_ENTRY) {
                    g_app_config.palette_entry = MIN_PALETTE_ENTRY;
                }     
                g_render_components.render_palette_active = 1;
                g_render_components.render_palette_text = 1;                 
                break;
            case KEY_SPACE:
                x = g_app_config.cursor_x + g_app_config.map_x - MAP_AREA_X;
                y = g_app_config.cursor_y + g_app_config.map_y - MAP_AREA_Y;
                if (get_map_at(x, y) == 0) {
                    set_map_at(x, y, g_app_config.palette_entry);
                } else
                {
                    set_map_at(x, y, 0);
                }
                g_render_components.render_cursor = 1;
                break;
            default:
                break;
        }
    }
}
