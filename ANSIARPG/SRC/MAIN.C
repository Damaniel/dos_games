#include "textdraw.h"
#include <bios.h>

void wait_for_key(void) {
  while(!_bios_keybrd(_KEYBRD_READY)) {}
  _bios_keybrd(0x00);
}

int main(void) {
  
  char attr;

  set_text_mode(MODE_80X25);

  set_bg_intensity(1);
  clear_screen();

  attr = make_attr(COLOR_WHITE, COLOR_BLUE);
  hline_at(0, 0, 50, 177, attr);
  hline_at(0, 24, 50, 177, attr);
  wait_for_key();

  clear_screen();

  return 0;
}
