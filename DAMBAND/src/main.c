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
#include <bios.h>
#include <string.h>
#include <stdio.h>
#include <sys/nearptr.h>
#include "textdraw.h"
#include "cavern.h"

void wait_for_key() {    
  while(!bioskey(0x01)) {}
  bioskey(0x00);
}

Rect g_menu_bar;
Rect g_main_window;
Rect g_play_area;
Rect g_status_area;
Rect g_extra_area;
Rect g_text_box_window;
Rect g_text_box_text_area;

void set_extents() {
  
  // Set the common areas
  set_rect(&g_menu_bar, 0, 0, 79, 0);
  set_rect(&g_main_window, 0, 1, 61, 35);
  set_rect(&g_play_area, 1, 2, 60, 34);
  set_rect(&g_status_area, 62, 1, 79, 24);
  set_rect(&g_extra_area, 62, 25, 79, 35);
  
  // Set the areas that vary depending on whether we're VGA/EGA
  set_rect(&g_text_box_window, 0, 36, 79, g_lines - 1);
  set_rect(&g_text_box_text_area, 1, 37, 78, g_lines - 2);
    
}

int main(void) {

    srand(time(NULL));

    init_maze_structs(70, 20);
    generate_maze();
    print_maze();
    free_maze_structs();

    return 0;
}

/*
int main(void) {
  int ch;
  char attr;
  int card;
   
  __djgpp_nearptr_enable();
   
  card = detect_adapter();

  if (card == CARD_VGA) {
    g_lines = LINES_VGA;
  } else if (card == CARD_EGA) {
    g_lines = LINES_EGA;
  } else {
    printf("Not supported on CGA/MDA adapters!\n");
    return 1;
  }
 
  g_screen = (char *)(0xb8000 + __djgpp_conventional_base);
  
  set_text_mode(MODE_80X50);
  set_bg_intensity(1);
  
  hide_cursor();
  set_extents();
  
  attr = make_attr(COLOR_WHITE, COLOR_BLUE);
  hline_at(g_menu_bar.x1, g_menu_bar.y1, g_menu_bar.w, ' ', attr);
  string_at(2, 0, "File  Commands  Status  Equip  Items  GemBox  Cards  Help", attr);

  attr = make_attr(COLOR_WHITE, COLOR_BLACK);
  box_at_rect(g_main_window, BORDER_DOUBLE, attr);
  box_at_rect(g_status_area, BORDER_DOUBLE, attr);
  box_at_rect(g_extra_area, BORDER_DOUBLE, attr);
  
  attr = make_attr(COLOR_LIGHT_BLUE, COLOR_BLUE);
  box_at_rect(g_text_box_window, BORDER_DOUBLE, attr);
  fill_box_at_rect(g_text_box_text_area, ' ', attr);

  attr = make_attr(COLOR_WHITE, COLOR_BLUE);
  string_at(1, 37, "You hit the bat for 6 damage!", attr);
  wait_for_key();
  
  set_text_mode(MODE_80X25);  
  set_bg_intensity(0);
  
  __djgpp_nearptr_disable();
   
  return 0;
}
*/
