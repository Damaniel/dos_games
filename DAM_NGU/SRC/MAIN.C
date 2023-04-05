#include "../INCLUDE/DAM_NGU.H"


volatile unsigned int g_game_timer;

void game_handler(void) {
    g_game_timer++;
}
END_OF_FUNCTION(game_handler);


int main(void) {
    int result;
    int finished = 0;
    int next_frame;

    srand(time(NULL));

    LOCK_VARIABLE(g_game_timer);
    LOCK_FUNCTION(game_handler);
    g_game_timer = 0;

    allegro_init();
    install_timer();
    install_keyboard();
    // The other game logic (graphics, etc) updates 30 times per second
    install_int(game_handler, 33);
    
    g_map_location.map_page_x = 0;
    g_map_location.map_page_y = 0;
    g_map_location.p_x = 0;
    g_map_location.p_y = 0;

    result = load_map_from_coordinates(g_map_location.map_page_x, g_map_location.map_page_y);
    if (result != 0) {
        printf("An error occurred while loading the map!\n");
        return 0;
    }

    set_gfx_mode(GFX_VGA, 320, 200, 320, 200);
    g_tiles = load_bitmap("res\\tiles.pcx", g_pal);
    g_main_ui_bg = load_bitmap("res\\bg.pcx", g_pal);
    set_palette(g_pal);

    // 47 is currently the index of black
    clear_to_color(screen, 47);

    clear_render_components(&g_render_components);
    g_render_components.bg = 1;
    g_render_components.map = 1;

    next_frame = g_game_timer + 1;
    while(!finished) {
        finished = process_input();
        while (g_game_timer < next_frame) {
            rest(1);
        }
        render(screen, &g_render_components);
        next_frame = g_game_timer+1;
    }

    set_gfx_mode(GFX_TEXT, 80, 25, 80, 25);

    return 0;
}