#ifndef __RENDER_H__
#define __RENDER_H__

typedef struct {
    char render_background;
    char render_menu;
    char render_palette;
    char render_palette_text;
    char render_palette_active;
    char render_exits_text;
    char render_exit_active;
    char render_map_area;
    char render_cursor_position;
    char render_cursor;
} RenderComponents;

extern RenderComponents g_render_components;

void render();
void set_all_render_components(void);
void clear_render_components(void);

#endif
