#include <allegro.h>
#include "render.h"
#include "globals.h"

void copy_data_to_offscreen_vram(void) {
	BITMAP *b = (BITMAP *)g_game_data[DAMRL_MAIN_MAP].dat;	
	blit(b, screen, 0, 0, MAP_VMEM_X, MAP_VMEM_Y, MAP_VMEM_WIDTH, MAP_VMEM_HEIGHT);
}

void render_map(void) {
	blit(screen, screen, MAP_VMEM_X, MAP_VMEM_Y, 0, 0, MAP_VMEM_WIDTH, MAP_VMEM_HEIGHT);
}

void generate_prop_font_offsets(void) {
	int offset = 0;
	int i;
	
	for (i=0; i < FONT_ENTRIES; i++) {
		prop_font_offset[i] = offset;
		offset += prop_font_width[i];
	}
}

void render_prop_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx) {
	// Note: the font bitmap contains all characters in the ASCII range 32-128.  To find the
	// correct offset into the bitmap, the ASCII value needs to be subtracted by 32.	
	int x = x_pos;
	int y = y_pos;
	int offset;
	char *cur = text;
	
	while (*cur != 0) {
		offset = (*cur++) - 32;
		blit((BITMAP *)g_game_data[DAMRL_PROP_FONT].dat, destination, prop_font_offset[offset],
		     font_idx * prop_font_height, x, y, prop_font_width[offset], prop_font_height);
		x += prop_font_width[offset] + 1;
	}
}

void render_fixed_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx) {
	// Note: the font bitmap contains all characters in the ASCII range 32-128.  To find the
	// correct offset into the bitmap, the ASCII value needs to be subtracted by 32.
	int x = x_pos;
	int y = y_pos;
	int offset;
	char *cur = text;
	
	// This isn't a library, so we'll assume strings are terminated.  That should end well...
	while(*cur != 0) {
		offset = (*cur++) - 32;
		blit((BITMAP *)g_game_data[DAMRL_FIXED_FONT].dat, destination, offset * fixed_font_width, 
		     font_idx * fixed_font_height, x, y, fixed_font_width, fixed_font_height);
		x += 8;
	}
}

/* This is filled in by a function based on prop_font_width */
int prop_font_offset[FONT_ENTRIES];

/* This is the width of each character in the font from ASCII values 32 to 127*/
int prop_font_width[FONT_ENTRIES] = {
	6,   /* space */
	2,   /* ! */
	6,   /* " */
	7,   /* # */
	6,   /* $ */
	6,   /* % */
	7,   /* & */
	2,   /* ' */
	4,   /* ( */
	4,   /* ) */
	5,   /* * */
	5,   /* + */
	2,   /* , */
	6,   /* - */
	2,   /* . */
	6,   /* / */
	7,   /* 0 */
	4,   /* 1 */
	7,   /* 2 */
	7,   /* 3 */
	7,   /* 4 */
	7,   /* 5 */
	7,   /* 6 */
	7,   /* 7 */
	7,   /* 8 */
	7,   /* 9 */
	2,   /* : */
	2,   /* ; */
	5,   /* < */
	6,   /* = */
	5,   /* > */
	6,   /* ? */
	8,   /* @ */
	8,   /* A */
	7,   /* B */
	7,   /* C */
	7,   /* D */
	7,   /* E */
	7,   /* F */
	7,   /* G */
	8,   /* H */
	4,   /* I */
	7,   /* J */
	8,   /* K */
	7,   /* L */
	8,   /* M */
	8,   /* N */
	7,   /* O */
	7,   /* P */
	7,   /* Q */
	8,   /* R */
	7,   /* S */
	6,   /* T */
	8,   /* U */
	8,   /* V */
	8,   /* W */
	8,   /* X */
	8,   /* Y */
	6,   /* Z */
	4,   /* [ */
	6,   /* \ */
	4,   /* ] */
	5,   /* ^ */
	8,   /* _ */
	3,   /* ` */
	7,   /* a */
	7,   /* b */
	6,   /* c */
	7,   /* d */
	6,   /* e */
	5,   /* f */
	7,   /* g */
	8,   /* h */
	4,   /* i */
	6,   /* j */
	7,   /* k */
	4,   /* l */
	8,   /* m */
	8,   /* n */
	6,   /* o */
	7,   /* p */
	7,   /* q */
	7,   /* r */
	6,   /* s */
	4,   /* t */
	7,   /* u */
	8,   /* v */
	7,   /* w */
	6,   /* x */
	6,   /* y */
	6,   /* z */
	4,   /* { */
	2,   /* | */
	4,   /* } */
	7,   /* ~ */
	7    /* <del> */
};
