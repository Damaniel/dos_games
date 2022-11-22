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

typedef struct {
    char render_main;
    char render_name;
    char render_foreground;
    char render_background;
    char render_character;
    char render_solid;
    char render_damage;
    char render_preview;
    char render_active_item;
} RenderPaletteEditComponents;

extern RenderComponents g_render_components;
extern RenderPaletteEditComponents g_render_palette_edit_components;

void render(void);
void set_all_render_components(void);
void clear_render_components(void);
void set_all_palette_edit_components(void);
void clear_all_palette_edit_components(void);
void render_main_screen(void);
void render_palette_edit_screen(void);


#endif
