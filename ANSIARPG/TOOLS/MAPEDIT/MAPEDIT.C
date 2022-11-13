#include "mapedit.h"

UIConfig g_ui_config;
RenderComponents g_render_components;

void wait_for_key(void) {
  while(!_bios_keybrd(_KEYBRD_READY)) {}
  _bios_keybrd(0x00);
}

// Helper function - set the global components object to render all components
void set_all_render_components(RenderComponents *rc) {
    rc->render_background = 1;
    rc->render_menu = 1;
    rc->render_palette = 1;
    rc->render_palette_text = 1;
    rc->render_palette_active = 1;
    rc->render_exits_text = 1;
    rc->render_exit_active = 1;
    rc->render_map_area = 1;
}

// Helper function - disable the rendering of all components
void clear_render_components(RenderComponents *rc) {
    rc->render_background = 0;
    rc->render_menu = 0;
    rc->render_palette = 0;
    rc->render_palette_text = 0;
    rc->render_palette_active = 0;
    rc->render_exits_text = 0;
    rc->render_exit_active = 0;
    rc->render_map_area = 0;
}

void initialize_attributes(UIConfig *ui) {
    ui->background_attr = make_attr(COLOR_WHITE, COLOR_BLACK);
    ui->menu_attr = make_attr(COLOR_WHITE, COLOR_BLUE);
}

void render(UIConfig *ui, RenderComponents *rc) {
    if (rc->render_background) {
        box_at(0, 1, 79, 24, BORDER_SINGLE, ui->background_attr);
        char_at(63, 1, 194, ui->background_attr);
        char_at(63, 24, 193, ui->background_attr);
    }
    if (rc->render_menu) {
        hline_at(0, 0, 80, ' ', ui->menu_attr);        
    }

    clear_render_components(rc);
}

int main(void) {
    set_text_mode(MODE_80X25);
    set_bg_intensity(1);

    clear_screen();
    hide_cursor();

    initialize_attributes(&g_ui_config);
    set_all_render_components(&g_render_components);
    render(&g_ui_config, &g_render_components);

    wait_for_key();
    clear_screen();

    show_cursor();
    return 0;
}
