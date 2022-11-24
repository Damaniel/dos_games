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

UIConfig g_ui_config;
AppConfig g_app_config;
PaletteMenuConfig g_palette_menu_config;

PaletteEntry g_map_palette[NUM_PALETTE_ENTRIES];
Exit g_exit_list[NUM_EXITS];
State g_state;
State g_prev_state;

unsigned char g_map[MAP_WIDTH][MAP_HEIGHT];

int g_help_page;

void set_state(State s) {
    g_prev_state = g_state;
    g_state = s;
    // Perform any state-specific actions required
    switch(g_state) {
        case MAIN_SCREEN:
            set_all_render_components();        
            break;
        case PALETTE_EDIT:
            set_all_palette_edit_components();
            initialize_palette_menu_defaults();
            copy_palette_to_edit_menu(g_app_config.palette_entry);            
            break;
        case HELP_SCREEN:
            g_help_page = 0;
            set_all_help_screen_components();
            break;
    }
}

int main(void) {

    PaletteEntry p;

    set_state(MAIN_SCREEN);

    initialize_palette();
    initialize_exits();
    initialize_app_defaults();
    initialize_palette_menu_defaults();

    set_text_mode(MODE_80X25);
    set_bg_intensity(1);

    set_exit(0, 23, 17, 32);

    clear_screen();
    hide_cursor();

    clear_map();
    initialize_attributes();
    set_all_render_components();
    set_all_palette_edit_components();

    set_state(MAIN_SCREEN);

    while(!g_app_config.quit) {
        process_input();
        render();
    }
    clear_screen();

    show_cursor();
    set_bg_intensity(0);

   return 0;
}
