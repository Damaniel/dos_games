#include "textdraw.h"
#include "maze.h"
#include "render.h"
#include "player.h"
#include <bios.h>
#include <stdlib.h>
#include <time.h>

void wait_for_key(void) {    
  while(!bioskey(0x01)) {}
  bioskey(0x00);
}

int main(void) {

  char attr;
  int card;
   
  srand(time(NULL));

  card = detect_adapter();
  
  if (card == CARD_VGA) {
    g_lines = 50;
  }
  else if (card == CARD_EGA) {
    g_lines = 43;
  }
  else {
    return 1;
  }
  
  init_player();
  generate_maze();

  set_text_mode(MODE_80X50);
  
  set_bg_intensity(1);
  
  hide_cursor();
  set_extents();
  
  render_main_ui();
  render_play_area();
  wait_for_key();

  show_cursor();
  set_text_mode(MODE_80X25);  
  set_bg_intensity(0);

  return 0;
}