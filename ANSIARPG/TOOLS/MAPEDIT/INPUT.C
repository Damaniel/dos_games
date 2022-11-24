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

void process_main_screen_input(unsigned char ascii_code, 
                               unsigned char scan_code, 
                               unsigned char shift_status) {
    int x, y;
    switch(scan_code) {
        case KEY_ESC:
            g_app_config.quit = 1;
            break;
        case KEY_H:
            set_state(HELP_SCREEN);
            break;
        case KEY_BACKSLASH:
            set_state(PALETTE_EDIT);
            break;
        case KEY_RIGHT:
            // move the cursor right
            // If SHIFT is held down, move a screen at a time
            if (shift_status & 0x01 || shift_status & 0x02) {
                g_app_config.map_x += MAP_AREA_WIDTH;
                if(g_app_config.map_x > MAX_SCREEN_MAP_X) {
                    g_app_config.map_x = MAX_SCREEN_MAP_X;
                }
                g_render_components.render_map_area = 1;                                
            } else {
                // Otherwise, move one space at a time, scrolling as needed
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
            }
            g_render_components.render_cursor = 1;
            g_render_components.render_cursor_position = 1;
            break;
        case KEY_LEFT:
            // move the cursor left
            // If SHIFT is held down, move a screen at a time
            if (shift_status & 0x01 || shift_status & 0x02) {
                if (g_app_config.map_x < MAP_AREA_WIDTH) {
                    g_app_config.map_x = 0;
                } else {
                    g_app_config.map_x = MAX_SCREEN_MAP_X;
                }
                g_render_components.render_map_area = 1;                                
            } else {
                // otherwise move a square at a time
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

// Process the OK button being 'pressed'
void handle_ok_logic(void) {
    // Save changes to palette then return to main screen.
    copy_edit_menu_to_palette(g_app_config.palette_entry);
    set_state(MAIN_SCREEN);
}

// Process the cancel button being 'pressed'
void handle_cancel_logic(void) {
    set_state(MAIN_SCREEN);
}

// Process the change of solid/passable state.  The left and right arrows,
// and the space bar, can toggle it.
void toggle_solid_state(void) {
    if (g_palette_menu_config.solid == FLAG_SOLID) {
        g_palette_menu_config.solid = FLAG_PASSABLE;
    } else {
        g_palette_menu_config.solid = FLAG_SOLID; 
    }
    g_render_palette_edit_components.render_solid = 1;
}

void process_palette_edit_input(unsigned char ascii_code, 
                                unsigned char scan_code,
                                unsigned char shift_status) {

    // Notes about tile name input:
    //
    // Tile names support the characters A-Z, a-z, 0-9, space and dash.
    // The left and right arrow keys move the cursor left and right,
    // the appropriate letter/number key puts the character in the cursor location
    // and moves the cursor one space to the right, and the backspace key deletes
    // the character under the cursor and moves it one space to the left.  In the
    // case of both backspace and adding a letter, the cursor wraps (so if you 
    // delete the 0th position, the cursor moves to the last position, and if you
    // add a character to the 8th position, the cursor moves to the 0th).

    // Handle character input seperately from scan code input.  We never use
    // (most of) these keys in this menu as raw scan codes, so we can process them as
    // their ASCII values instead.  Space is the main exception as it's used for
    // a toggle in the 'Solid?' option and as an alternative to Enter on the
    // OK/cancel menus.  We check for that later.
    // The box can support uppercase, lowercase, numbers, space and dash.
    if (ascii_code == 32 || ascii_code == 45 || 
        (ascii_code >= 48 && ascii_code <=57) ||
        (ascii_code >= 65 && ascii_code <=90) ||
        (ascii_code >= 97 && ascii_code <=122)) {
            if (g_palette_menu_config.active_item == PI_NAME) {
                g_palette_menu_config.name[g_palette_menu_config.name_idx] = ascii_code;
                g_palette_menu_config.name_idx++;
                if (g_palette_menu_config.name_idx >= 8) {
                    g_palette_menu_config.name_idx = 0;
                }
                g_render_palette_edit_components.render_name = 1;
                g_render_palette_edit_components.render_active_item = 1;
            }
    }
    switch (scan_code) {
        // TEMP: ESC is just for testing!
        case KEY_ESC:
            set_state(MAIN_SCREEN);
            break;        
        case KEY_DOWN:
            switch (g_palette_menu_config.active_item) {
                case PI_NAME:
                    g_palette_menu_config.active_item = PI_FOREGROUND;
                    break;
                case PI_FOREGROUND:
                    g_palette_menu_config.active_item = PI_BACKGROUND;
                    break;
                case PI_BACKGROUND:
                    g_palette_menu_config.active_item = PI_CHARACTER;
                    break;
                case PI_CHARACTER:
                    g_palette_menu_config.active_item = PI_SOLID;
                    break;
                case PI_SOLID:
                    g_palette_menu_config.active_item = PI_DAMAGE;
                    break;
                case PI_DAMAGE:
                    g_palette_menu_config.active_item = PI_OK;
                    break;
                case PI_OK:
                    g_palette_menu_config.active_item = PI_CANCEL;
                    break;
                case PI_CANCEL:
                    g_palette_menu_config.active_item = PI_NAME;
                    break;
            }
            g_render_palette_edit_components.render_active_item = 1;
            break;
        case KEY_UP:
            switch (g_palette_menu_config.active_item) {
                case PI_NAME:
                    g_palette_menu_config.active_item = PI_CANCEL;
                    break;
                case PI_FOREGROUND:
                    g_palette_menu_config.active_item = PI_NAME;
                    break;
                case PI_BACKGROUND:
                    g_palette_menu_config.active_item = PI_FOREGROUND;
                    break;
                case PI_CHARACTER:
                    g_palette_menu_config.active_item = PI_BACKGROUND;
                    break;
                case PI_SOLID:
                    g_palette_menu_config.active_item = PI_CHARACTER;
                    break;
                case PI_DAMAGE:
                    g_palette_menu_config.active_item = PI_SOLID;
                    break;
                case PI_OK:
                    g_palette_menu_config.active_item = PI_DAMAGE;
                    break;
                case PI_CANCEL:
                    g_palette_menu_config.active_item = PI_OK;
                    break;
            }
            g_render_palette_edit_components.render_active_item = 1;
            break;        
        case KEY_RIGHT:
            switch (g_palette_menu_config.active_item) {
                case PI_NAME:
                    if (g_palette_menu_config.name_idx < PALETTE_ITEM_NAME_LENGTH - 1) {
                        g_palette_menu_config.name_idx++;
                    } else {
                        g_palette_menu_config.name_idx = 0;
                    }
                    g_render_palette_edit_components.render_name = 1;
                    g_render_palette_edit_components.render_active_item = 1;                    
                    break;                
                case PI_FOREGROUND:
                    ++g_palette_menu_config.foreground;
                    if (g_palette_menu_config.foreground > 15) {
                        g_palette_menu_config.foreground = 0;
                    }
                    g_render_palette_edit_components.render_foreground = 1;
                    g_render_palette_edit_components.render_preview = 1;
                    break;
                case PI_BACKGROUND:
                    ++g_palette_menu_config.background;
                    if (g_palette_menu_config.background > 15) {
                        g_palette_menu_config.background = 0;
                    }
                    g_render_palette_edit_components.render_background = 1;
                    g_render_palette_edit_components.render_preview = 1;
                    break;                    
                case PI_CHARACTER:
                    if (g_palette_menu_config.character == 255) {
                        g_palette_menu_config.character = 0;
                    } else {
                        ++g_palette_menu_config.character;
                    }
                    g_render_palette_edit_components.render_character = 1;
                    g_render_palette_edit_components.render_preview = 1;
                    break;
                case PI_SOLID:
                    toggle_solid_state();
                    break;
                case PI_DAMAGE:
                    switch (g_palette_menu_config.damage_type) {
                        case FLAG_DAMAGE_NONE:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_LOW;
                            break;
                        case FLAG_DAMAGE_LOW:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_MEDIUM;
                            break;
                        case FLAG_DAMAGE_MEDIUM:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_HIGH;
                            break;
                        case FLAG_DAMAGE_HIGH:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_OHKO;                                                                            
                            break;
                        case FLAG_DAMAGE_OHKO:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_NONE;
                            break;                        
                    }
                    g_render_palette_edit_components.render_damage = 1;                    
                    break;
            }   
            break;
        case KEY_LEFT:
            switch (g_palette_menu_config.active_item) {
                case PI_NAME:
                    if (g_palette_menu_config.name_idx > 0) {
                        g_palette_menu_config.name_idx--;
                    } else {
                        g_palette_menu_config.name_idx = PALETTE_ITEM_NAME_LENGTH - 1;
                    }
                    g_render_palette_edit_components.render_name = 1;
                    g_render_palette_edit_components.render_active_item = 1;                    
                    break;
                case PI_FOREGROUND:
                    if (g_palette_menu_config.foreground == 0) {
                        g_palette_menu_config.foreground = 15;
                    } else {
                        --g_palette_menu_config.foreground;
                    }
                    g_render_palette_edit_components.render_foreground = 1;
                    g_render_palette_edit_components.render_preview = 1;
                    break;         
                case PI_BACKGROUND:
                    if (g_palette_menu_config.background == 0) {
                        g_palette_menu_config.background = 15;
                    } else {
                        --g_palette_menu_config.background;
                    }
                    g_render_palette_edit_components.render_background = 1;
                    g_render_palette_edit_components.render_preview = 1;
                    break;                           
                case PI_CHARACTER:
                    if (g_palette_menu_config.character == 0) {
                        g_palette_menu_config.character = 255;
                    } else {
                        --g_palette_menu_config.character;
                    }                
                    g_render_palette_edit_components.render_character = 1;
                    g_render_palette_edit_components.render_preview = 1;
                    break;
                case PI_SOLID:
                    toggle_solid_state();
                    break;
                case PI_DAMAGE:
                    switch (g_palette_menu_config.damage_type) {
                        case FLAG_DAMAGE_NONE:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_OHKO;
                            break;
                        case FLAG_DAMAGE_LOW:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_NONE;
                            break;
                        case FLAG_DAMAGE_MEDIUM:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_LOW;
                            break;
                        case FLAG_DAMAGE_HIGH:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_MEDIUM;                                                                            
                            break;
                        case FLAG_DAMAGE_OHKO:
                            g_palette_menu_config.damage_type = FLAG_DAMAGE_HIGH;
                            break;                        
                    }
                    g_render_palette_edit_components.render_damage = 1;
                    break;                            
            }   
            break;    
        case KEY_ENTER:
            switch (g_palette_menu_config.active_item) {
                case PI_OK:
                    handle_ok_logic();
                    break;
                case PI_CANCEL:
                    handle_cancel_logic();
                    break;
            }
            break;
        case KEY_SPACE:
            switch (g_palette_menu_config.active_item) {
                case PI_OK:
                    handle_ok_logic();
                    break;
                case PI_CANCEL:
                    handle_cancel_logic();
                    break;
                case PI_SOLID:
                    toggle_solid_state();
                    break;                
            }
            break;        
        case KEY_BACKSPACE:
            switch (g_palette_menu_config.active_item) {
                case PI_NAME:
                    g_palette_menu_config.name[g_palette_menu_config.name_idx] = ' ';

                    if (g_palette_menu_config.name_idx > 0) {
                        g_palette_menu_config.name_idx--;
                    } else {
                        g_palette_menu_config.name_idx = PALETTE_ITEM_NAME_LENGTH - 1;
                    }
                    g_render_palette_edit_components.render_name = 1;
                    g_render_palette_edit_components.render_active_item = 1;                    
                    break;             
            }
            break;          
    }
}

void process_help_screen_input(unsigned char ascii_code, 
                               unsigned char scan_code, 
                               unsigned char shift_status) {
    switch (scan_code) {
        case KEY_ESC:
            set_state(MAIN_SCREEN);
            break;        
    }
}

void process_input(void) {
    unsigned short key, shift_status;
    unsigned char ascii_code, scan_code;

    // Get the shift state since it's non-blocking
    shift_status = _bios_keybrd(_KEYBRD_SHIFTSTATUS);
    // Is there a key ready?
    if (_bios_keybrd(_KEYBRD_READY)) {
        key = _bios_keybrd(_KEYBRD_READ);
        ascii_code = (key & 0xFF);
        scan_code = (key >> 8);
        switch(g_state) {
            case MAIN_SCREEN:
                process_main_screen_input(ascii_code, scan_code, shift_status);
                break;
            case PALETTE_EDIT:
                process_palette_edit_input(ascii_code, scan_code, shift_status);
                break;
            case HELP_SCREEN:
                process_help_screen_input(ascii_code, scan_code, shift_status);
                break;
            default:
                break;
        }
    }
}
