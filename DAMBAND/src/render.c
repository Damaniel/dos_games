#include "render.h"

void set_extents(void) {
  
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

void render_main_ui(void) {
  char attr;

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
  
}

void render_maze(void) {
  char i, j;
  char i_d, j_d;
  char wall_attr, floor_attr;

  wall_attr = make_attr(COLOR_WHITE, COLOR_BLACK);
  floor_attr = make_attr(COLOR_DARK_GRAY, COLOR_BLACK);

  for (i=0; i < MAZE_AREA_WIDTH; i++ ) {
    i_d = i - 30 + g_player.x_pos;
    for (j=0; j < MAZE_AREA_HEIGHT; j++) {
      j_d = j - 16 + g_player.y_pos;
      if (i_d >=0 && j_d >= 0 && i_d < MAZE_AREA_WIDTH && j_d < MAZE_AREA_HEIGHT) {
        if (g_maze[i_d][j_d].is_lit) {
          if (g_maze[i_d][j_d].carved) {
            char_at(i+1, j+2, ' ', floor_attr);
          }
          else {
            char_at(i+1, j+2, '#', wall_attr);
          }
        }
      }
    }
  }
}

void render_play_area(void) {
  char attr;

  attr = make_attr(COLOR_WHITE, COLOR_BLACK);
  render_maze();

  char_at(31, 18, '@', attr);
}

void render_status(void) {

}

void render_logs(void) {

}
