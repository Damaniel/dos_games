/* Copyright 2021 Shaun Brandt
   
   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
   DEALINGS IN THE SOFTWARE.
 */
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dpmi.h>
#include "../include/globals.h"
#include "../include/util.h"

BITMAP *buffer;
DATAFILE *g_res;

State g_state;
State g_prev_state;
int g_game_done;

/*=============================================================================
 * change_state
 *============================================================================*/
void change_state(State new_state, State prev_state) {
  /* Do stuff */
  g_state = new_state;
  g_prev_state = prev_state;

  switch(g_state) {
    case STATE_LOGO:
      load_logo();
      set_palette(logo_pal);      
      clear_render_components(&g_components);      
      g_update_screen = 1;
      g_title_countdown = 3 * FRAME_RATE;
      break;
    case STATE_TITLE:
      load_title();
      set_palette(title_pal);
      g_title_anim.color_start_counter = FRAME_RATE / 2;
      g_title_anim.update_background = 1;  
      g_update_screen = 1;
      break;
    case STATE_GAME:
      set_palette(game_pal);
      clear_render_components(&g_components);
      g_components.render_all = 1;
      g_update_screen = 1;
      /* Start the timer */
      game_timer_set(1);      
      break;
    case STATE_MAP:
      clear_render_components(&g_components);
      g_components.render_map = 1;
      g_show_map_text = 1;
      g_update_screen = 1;    
      game_timer_set(0);
      break;
    case STATE_LOAD:
      game_timer_set(0);
      clear_render_components(&g_components);
      g_update_screen = 1;
      /* Force display of loading message */
      do_render();
      load_progress_file(g_picture);           
      change_state(STATE_GAME, STATE_LOAD);

      break;
    case STATE_SAVE:
      game_timer_set(0);
      clear_render_components(&g_components);
      g_update_screen = 1;      
      /* Force display of saving message */
      do_render();
      save_progress_file(g_picture);
      change_state(STATE_GAME, STATE_SAVE);      
      break;
    case STATE_HELP:
      break;
    case STATE_OPTS:
      break;
    case STATE_LOAD_DIALOG:
      game_timer_set(0);
      clear_render_components(&g_components);
      g_update_screen = 1;
      /* Force display of loading dialog */
      do_render();           
      break;
    default:
      break;
  }
}

void do_render(void) {
  if(g_update_screen) {
    render_screen(buffer, g_components);
    blit(buffer, screen, 0, 0, 0, 0, 320, 200);
    clear_render_components(&g_components);
    g_update_screen = 0;
  }  
}

/*=============================================================================
 * process_timing_stuff
 *============================================================================*/
void process_timing_stuff(void) {

  /* Update the timer for the logo screen */
  if (g_state == STATE_LOGO) {
    g_title_countdown--;
    if (g_title_countdown <= 0) {
      change_state(STATE_TITLE, STATE_LOGO);
    }
  }

  /* Update the animations on the title screen */
  if (g_state == STATE_TITLE) {
    g_title_anim.new_color_counter--;
    g_title_anim.color_start_counter--;

    if(g_title_anim.color_start_counter <= 0 && g_title_anim.color_start == 0 ){
      g_title_anim.color_start = 1;
      g_update_screen = 1;
    }

    if(g_title_anim.new_color_counter <= 0) {
      g_title_anim.update_title_color = 1;
      g_title_anim.new_color_counter = NEW_COLOR_COUNT;
      g_update_screen = 1;
    }

  }

  /* Update the elapsed time counter for the on-screen display */
  if(g_game_timer_running)
    g_time_to_update_elapsed--;
  if (g_time_to_update_elapsed <= 0) {
    g_elapsed_time++;
    if(g_elapsed_time % 60 == 0) {
      print_mem_free();
    }
    g_time_to_update_elapsed = FRAME_RATE;
    g_components.render_status_text = 1;
    g_update_screen = 1;
  }

  /* Actually update the screen */
  do_render();
}

/*=============================================================================
 * game_timer_set
 *============================================================================*/
void game_timer_set(int status) {
  g_game_timer_running = status;
}

/*=============================================================================
 * print_mem_free
 *============================================================================*/
void print_mem_free(void) {
    printf("free phys: %d bytes\nfree virtual: %d bytes\n",
          (int)_go32_dpmi_remaining_physical_memory(),
          (int)_go32_dpmi_remaining_virtual_memory());
}

/*=============================================================================
 * main
 *============================================================================*/
int main(int argc, char *argv[]) {
  int done;
  
  allegro_init();
  install_keyboard();
  install_timer();

  install_int(int_handler, 1000/FRAME_RATE);

  srand(time(NULL));

  set_gfx_mode(GFX_VGA, 320, 200, 0, 0);

  buffer = create_bitmap(320, 200);

  g_res = load_datafile("RES/DAMPBN.DAT");
  if(g_res == NULL) {
    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
    printf("Unable to load data!\n");
    destroy_bitmap(buffer);
    allegro_exit();
    exit(1);
  }

  load_graphics();

  g_picture = load_picture_file(argv[1]);
  if (g_picture == NULL) {
    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);    
    printf("Unable to load picture!\n");
    allegro_exit();
    exit(1);
  }

  init_defaults();

  change_state(STATE_LOGO, STATE_NONE);
  
  blit(buffer, screen, 0, 0, 0, 0, 320, 200);

  while(!g_game_done) {  

    /* Wait until the next frame ticks */
    while (!g_next_frame) {
       rest(1); 
    }

    /* Do anything that relies on the frame counter */
    process_timing_stuff();

    /* Get input */
    process_input(g_state);

    /* Check to see if we're done with the picture */
    done = check_completion();
    if (done)
      g_game_done = 1;

    /* Done in the loop, wait for the next frame */
    g_next_frame = 0;
  }

  free_picture_file(g_picture);
  
  unload_datafile(g_res);
  destroy_bitmap(buffer);
  set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
  allegro_exit();

  return 0;
}
