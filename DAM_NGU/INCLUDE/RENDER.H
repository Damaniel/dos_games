#ifndef __RENDER_H__
#define __RENDER_H__

typedef struct {
    unsigned char bg;
    unsigned char map;
} RenderComponents;

void render_map(BITMAP *b);
void render_bg(BITMAP *b);
void clear_render_components(RenderComponents *r);
void render(BITMAP *b, RenderComponents *r);

#endif
