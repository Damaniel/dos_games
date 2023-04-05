#include "../INCLUDE/DAM_NGU.H"

int process_input(void) {
    int key, result, finished = 0;
    int map_changed = 0;

    if (keypressed()) {
        key = readkey() >> 8;
        if (key == KEY_RIGHT) {
            if (g_map_location.map_page_x < MAP_WIDTH_PAGES - 1) {
                ++g_map_location.map_page_x;
                map_changed = 1;
            }
        }
        if (key == KEY_LEFT) {
            if(g_map_location.map_page_x > 0) {
                --g_map_location.map_page_x;
                map_changed = 1;
            }
        }
        if (key == KEY_ESC) {
            finished = 1;
        }
    }

    if (map_changed) {
        result = load_map_from_coordinates(g_map_location.map_page_x, g_map_location.map_page_y);
        g_render_components.map = 1;
    }

    return finished;
}