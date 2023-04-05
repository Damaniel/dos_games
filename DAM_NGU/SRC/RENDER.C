#include "../INCLUDE/DAM_NGU.H"

BITMAP *g_tiles;
BITMAP *g_main_ui_bg;

PALETTE g_pal;
RenderComponents g_render_components;

void clear_render_components(RenderComponents *r) {
    r->bg = 0;
    r->map = 0;
}

void render(BITMAP *b, RenderComponents *r) {
    // Draw the background layer if requested
    if (r->bg) {
        render_bg(b);
        r->bg = 0;
    }

    // Draw the map tiles (both layers) if requested)
    if (r->map) {
        render_map(b);
        r->map = 0;
    }
}

void render_bg(BITMAP *b) {
    blit(g_main_ui_bg, b, 0, 0, 0, 0, 320, 200);
}

void render_map(BITMAP *b) {
    int i, j, tx, ty;
    MapSquare m;
    for(i=0;i<MAP_WIDTH;i++) {
        for (j=0;j<MAP_HEIGHT;j++) {
            m = g_map_page[i][j];
            tx = m.bg_tile_layer_idx % 16;
            ty = m.bg_tile_layer_idx / 16;
            blit(g_tiles, b, tx * 16, ty * 16, 4 + i*16, 4+j*16, 16, 16);
            if (m.trans_tile_layer_idx != 0) {
                tx = m.trans_tile_layer_idx % 16;
                ty = m.trans_tile_layer_idx / 16;
                masked_blit(g_tiles, b, tx * 16, ty * 16, 4 + i*16, 4+j*16, 16, 16);
            }
        }
    }
}