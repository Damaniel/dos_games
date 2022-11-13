#ifndef __MAPEDIT_H__
#define __MAPEDIT_H__

#include "textdraw.h"
#include <bios.h>

typedef struct {
    char background_attr;
    char menu_attr;
} UIConfig;

typedef struct {
    char render_background;
    char render_menu;
    char render_palette;
    char render_palette_text;
    char render_palette_active;
    char render_exits_text;
    char render_exit_active;
    char render_map_area;
} RenderComponents;

extern UIConfig g_ui_config;
extern RenderComponents g_render_components;

void wait_for_key(void);
void set_all_render_components(RenderComponents *rc);
void clear_render_components(RenderComponents *rc);
void initialize_attributes(UIConfig *ui);
void render(UIConfig *ui, RenderComponents *rc);

#endif
