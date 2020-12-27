#ifndef __RENDER_H__
#define __RENDER_H__

void generate_prop_font_offsets(void);
void render_prop_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx);
void render_fixed_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx);
void copy_data_to_offscreen_vram(void);

/* Functions to draw various screen components */
void render_map(void);


enum {
	FONT_YELLOW,
	FONT_BLUE,
	FONT_GREEN,
	FONT_RED,
	FONT_GRAY
};

#define fixed_font_width  8
#define fixed_font_height 8
#define prop_font_height  8

/* A copy of the map bitmap is stored at this location in video memory. */
#define MAP_VMEM_X	0
#define MAP_VMEM_Y	280

/* The size of the map bitmap in offscreen memory */
#define MAP_VMEM_WIDTH   144
#define MAP_VMEM_HEIGHT  160
 
#define FONT_ENTRIES       96
extern int prop_font_offset[FONT_ENTRIES];
extern int prop_font_width[FONT_ENTRIES];

#endif	
	
	
	
	