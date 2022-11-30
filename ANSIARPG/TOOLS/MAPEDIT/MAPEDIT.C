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

MapHeader g_map_header;

char g_map_file_name[MAP_FILE_NAME_LENGTH];
unsigned char g_map[MAP_WIDTH][MAP_HEIGHT];

int g_help_page;

void set_state(State s) {
    int result;

    g_prev_state = g_state;
    g_state = s;
    // Perform any state-specific actions required
    switch(g_state) {
        case MAIN_SCREEN:
            // if we're not toggling the place player option, draw everything.
            // (Otherwise, we'll just draw the updated cursor based on other
            // code.)
            if (g_prev_state != PLACE_PLAYER) {
                set_all_render_components();
            }
            break;
        case PALETTE_EDIT:
            set_all_palette_edit_components();
            copy_palette_to_edit_menu(g_app_config.palette_entry);            
            break;
        case HELP_SCREEN:
            g_help_page = 0;
            set_all_help_screen_components();
            break;
        case FILE_SAVE:
            result = write_map_file(g_map_file_name);
            if (result == 0) {
                g_render_save_file_components.render_saved_text = 1;
            } else {
                g_render_save_file_components.render_save_failed_text = 1;
            }
            break;
        case PLACE_EXIT:
            break;
        case PLACE_PLAYER:
            break;
    }
}

void initialize(void) {
    // Clear the exits structure
    initialize_exits();
    // Set default app settings
    initialize_app_defaults();
    // Set the default values for the palette edit menu
    initialize_palette_menu_defaults();
    // Initialize the map header (in case a new file is being created)
    initialize_map_header();  
    // Create all of the text attributes used (foreground + background color)
    initialize_attributes();  

    // Set the graphics mode and settings
    set_text_mode(MODE_80X25);
    // Set intensity bit to add extra colors and not blink
    set_bg_intensity(1);
    // Clear the screen and hide the cursor (since we never use it)
    clear_screen();
    hide_cursor();    

}

void cleanup(void) {
    // Erase everything on screen
    clear_screen();
    // Restore cursor and default background intensity bit behavior
    show_cursor();
    set_bg_intensity(0);
}

int main(int argc, char *argv[]) {

    PaletteEntry p;
    int result;

    if (argc != 2) {
        printf("Usage mapedit.exe <map_file>\n");
        exit(1);
    } else {
        strncpy(g_map_file_name, argv[1], MAP_FILE_NAME_LENGTH - 1);
    }

    initialize();
    set_exit(0, 23, 17, 32);

    result = read_map_file(g_map_file_name);
    if(result != 0) {
        if (result == -1) {
            printf("Unable to open file!\n");
            show_cursor();
            set_bg_intensity(0);
            exit(-1);
        }
        if (result == 1) {
            // File doesn't exist - create it anyway.
            clear_map();            
            initialize_palette();        
        }
    }

    set_state(MAIN_SCREEN);

    while(!g_app_config.quit) {
        process_input();
        render();
    }

   cleanup();
   return 0;
}
