// Copyright 2022 Shaun Brandt
//   
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
#include <dpmi.h>
#include <pc.h>
#include <string.h>
#include "textdraw.h"

char *g_screen;
int g_lines;

void set_rect(Rect *r, int x1, int y1, int x2, int y2) {
  r->x1 = x1;
  r->y1 = y1;
  r->x2 = x2;
  r->y2 = y2;
  r->w = x2-x1+1;
  r->h = y2-y1+1;
}

void set_bg_intensity(int enabled) {
  __dpmi_regs regs;
  
  memset(&regs, 0, sizeof regs);
  regs.x.ax = 0x1003;
  if (enabled) {
    regs.h.bl = 0x00;
    regs.h.bh = 0x00;
  } else {
    regs.h.bl = 0x01;
    regs.h.bh = 0x00;
  }
  __dpmi_int(0x10, &regs);
}

char make_attr(int fg, int bg) {
    return (char)(fg | (bg << 4));
}

void clear_screen() {
   int i, j;
   
   // Set the text values to empty.
   // Set the attribute values to the 'default' DOS values (gray fg, black bg)
   for (i=0;i<80;i++) {
     for(j=0;j<50;j++) {
       g_screen[(j*80+i)*2] = 0;
       g_screen[(j*80+i)*2+1] = make_attr(COLOR_LIGHT_GRAY, COLOR_BLACK);
     }
   }
}

int detect_adapter() {
  __dpmi_regs regs;
  
  memset(&regs, 0, sizeof regs);
  regs.h.ah = 0x12;
  regs.h.bl = 0x10;
  __dpmi_int(0x10, &regs);
  if(regs.h.bl == 0x10)
    return CARD_CGA;

  memset(&regs, 0, sizeof regs);  
  regs.x.ax = 0x1a00;
  __dpmi_int(0x10, &regs);
  
  if(regs.h.al == 0x1a)
    return CARD_VGA;
  else 
    return CARD_EGA;
}

void hide_cursor() {
  __dpmi_regs regs;
  
  memset(&regs, 0, sizeof regs);
  regs.h.ah = 0x01;
  regs.h.ch = 0x20;
  regs.h.cl = 0x00;
  __dpmi_int(0x10, &regs);
}

void show_cursor() {
   __dpmi_regs regs;
  
  memset(&regs, 0, sizeof regs);
  regs.h.ah = 0x01;
  regs.h.ch = 0x00;
  regs.h.cl = 0x00;
  __dpmi_int(0x10, &regs); 
}

void set_cursor_at(int x, int y) {
   __dpmi_regs regs;
  
  memset(&regs, 0, sizeof regs);
  regs.h.ah = 0x02;
  regs.h.dh = y;
  regs.h.dl = x;
  __dpmi_int(0x10, &regs);
}
  
void set_text_mode(int mode) {
  __dpmi_regs regs;
  
  /* Clear register object */
  memset(&regs, 0, sizeof regs);
  
  if (mode == MODE_80X50) {
    // Set the 8x8 font
    regs.x.ax = 0x1112;
    __dpmi_int(0x10, &regs);
  } 
  else {   
    // Set the 8x14 font
    regs.x.ax = 0x1111;
    __dpmi_int(0x10, &regs);    
  }
  
  clear_screen();
  set_cursor_at(0, 0);
}

void char_at(int x, int y, char c, char attr) {
  g_screen[(y*80 + x)*2] = c;
  g_screen[(y*80 + x)*2 + 1] = attr;
}

void hline_at(int x, int y, int len, char c, char attr) {
  int i;
  
  for(i=x; i<x+len;i++)
    char_at(i, y, c, attr);
}

void vline_at(int x, int y, int len, char c, char attr) {
  int i;
  
  for(i=y; i<y+len;i++)
    char_at(x, i, c, attr);
}

void string_at(int x, int y, char *str, char attr) {
  int i;
  for (i=0; i< strlen(str); i++) {
    char_at(x+i, y, str[i], attr);
  }
}

void fill_box_at(int x1, int y1, int x2, int y2, char c, char attr) {
  int i;
  
  for(i=y1; i<=y2; i++) {
      hline_at( x1, i, x2-x1+1, c, attr);
  }
}

void box_at(int x1, int y1, int x2, int y2, int border_type, char attr) {
  char ul, ur, ll, lr, horz, vert;
  int i;
  
  if (border_type == BORDER_SINGLE) {
    ul = 218;
    ur = 191;
    ll = 192;
    lr = 217;
    horz = 196;
    vert = 179;
  } else if (border_type == BORDER_DOUBLE) {
    ul = 201;
    ur = 187;
    ll = 200;
    lr = 188;
    horz = 205;
    vert = 186;
  } else {
    return;
  }
  
  // Draw the corners
  char_at(x1, y1, ul, attr);
  char_at(x2, y1, ur, attr);
  char_at(x1, y2, ll, attr);
  char_at(x2, y2, lr, attr);

  // Draw the top and bottom sides
  hline_at(x1+1, y1, (x2-x1-1), horz, attr);
  hline_at(x1+1, y2, (x2-x1-1), horz, attr);

  // Draw the left and right sidex
  vline_at(x1, y1+1, (y2-y1-1), vert, attr);
  vline_at(x2, y1+1, (y2-y1-1), vert, attr);
}

void box_at_rect(Rect r, int border_type, char attr) {
  box_at(r.x1, r.y1, r.x2, r.y2, border_type, attr);
}

void fill_box_at_rect(Rect r, char c, char attr) {
  fill_box_at(r.x1, r.y1, r.x2, r.y2, c, attr);
}