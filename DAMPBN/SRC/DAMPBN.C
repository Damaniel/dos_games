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
#include "../include/render.h"

BITMAP *buffer;
DATAFILE *g_res;

State g_state;
State g_prev_state;
int g_game_done;

/*=============================================================================
 * change_state
 *============================================================================*/
void change_state(State new_state, State prev_state) {
  char name[80];

  /* Do stuff */
  g_state = new_state;
  g_prev_state = prev_state;

  switch(g_state) {
    case STATE_LOGO:
      load_logo();
      set_palette(logo_pal);      
      clear_render_components(&g_components);      
      g_title_countdown = 2 * FRAME_RATE;
      break;
    case STATE_TITLE:
      /* If we're coming back from pressing ESC on the load dialog, skip
         some of the init stuff */       
      if(g_prev_state != STATE_LOAD_DIALOG) {
         g_title_anim.update_background = 1;          
         load_title();        
      }
      if(g_prev_state == STATE_REPLAY || g_prev_state == STATE_GAME) {
        g_title_anim.color_start = 0;
      }
      g_title_anim.color_start_counter = FRAME_RATE / 2; 
      break;
    case STATE_GAME:
      /* If the player just picked a file to load, load it and any progress
         file that exists */
      if (prev_state == STATE_LOAD_DIALOG) {
        if(g_load_new_file == 1) {
          init_new_pic_defaults();     
          free_picture_file(g_picture);    
          sprintf(name, "%s/%s/%s.pic", PIC_FILE_DIR, g_collection_name, g_picture_file_basename);
          g_picture = load_picture_file(name);
          load_progress_file(g_picture);
          update_overview_area();
        }
      }
      set_palette(game_pal);
      clear_render_components(&g_components);
      g_components.render_all = 1;
      /* Start the timer */
      game_timer_set(1);      
      break;
    case STATE_MAP:
      clear_render_components(&g_components);
      g_components.render_map = 1;
      g_show_map_text = 1; 
      game_timer_set(0);
      break;
    case STATE_LOAD:
      game_timer_set(0);
      clear_render_components(&g_components);
      /* Force display of loading message */
      load_progress_file(g_picture);        
      update_overview_area();         
      g_highlight_load_button = 0;
      change_state(STATE_GAME, STATE_LOAD);
      break;
    case STATE_SAVE:
      game_timer_set(0);
      clear_render_components(&g_components);      
      /* Force display of saving message */
      do_render();
      save_progress_file(g_picture);
      g_highlight_save_button = 0;
      change_state(STATE_GAME, STATE_SAVE);      
      break;
    case STATE_HELP:
      game_timer_set(0);
      g_help_page = 0;
      break;
    case STATE_OPTS:
      break;
    case STATE_LOAD_DIALOG:
      /* Reset the load dialog positions and such*/
      init_load_dialog_defaults();
      /* Turn the timer off in case we're in the game */
      game_timer_set(0);
      /* Generate the list of collections, then get the files from the first
         collection */
      get_collections();
      get_picture_files(g_collection_items[g_load_collection_index].name);
      clear_render_components(&g_components);    
      break;
    case STATE_FINISHED:
      game_timer_set(0);
      g_components.render_all = 1;
      /* Force redraw the game screen so the last pixel gets displayed */
      render_game_screen(buffer, g_components);
      g_finished_countdown = FRAME_RATE / 2;
      break;
    case STATE_REPLAY:
      /* Draw 1/(5 seconds * frame rate) worth of replay per frame */
      g_replay_increment = g_total_picture_squares / (5 * FRAME_RATE);
      if (g_replay_increment < 1) {
        g_replay_increment = 1;
      }

      /* Calculate the replay pixel scale */
      g_replay_scale = 1;
      if(g_picture->w >= g_picture->h) {
        /* The picture is wider than it is tall, or it's square */
        if (g_picture->w < 160)
          g_replay_scale = 160 / g_picture->w;
        
      } else {
        /* The picture is taller than it is wide */
        if (g_picture->h < 100)
          g_replay_scale = 100 / g_picture->h;
      }

      /* Calculate the start position of the replay */
      g_replay_x = 160 - (g_picture->w * g_replay_scale / 2);
      g_replay_y = 100 - (g_picture->h * g_replay_scale / 2);

      /* Prep the rest of the replay parameters */
      g_replay_first_time = 1;
      g_replay_total = 0;
      break;
    default:
      break;
  }
}

void do_render(void) {

    render_screen(buffer, g_components);
    vsync();
    show_mouse(NULL);    
    blit(buffer, screen, 0, 0, 0, 0, 320, 200);
    show_mouse(screen);
    clear_render_components(&g_components);

}

/*=============================================================================
 * process_timing_stuff
 *============================================================================*/
void process_timing_stuff(void) {

  if (g_state == STATE_REPLAY) {
    g_replay_total += g_replay_increment;
    if (g_replay_total >= g_total_picture_squares) {
      g_replay_total = g_total_picture_squares;
    }
  }

  /* Update the timer for the logo screen */
  if (g_state == STATE_LOGO) {
    g_title_countdown--;
    if (g_title_countdown <= 0) {
      change_state(STATE_TITLE, STATE_LOGO);
    }
  }

  if (g_state == STATE_FINISHED) {
    g_finished_countdown--;
    if (g_finished_countdown <=0 ) {
      change_state(STATE_REPLAY, STATE_FINISHED);
    }
  }
  /* Update the animations on the title screen 
     This is also called when showing the loading dialog on the title
     screen. */
  if (g_state == STATE_TITLE || 
     (g_state == STATE_LOAD_DIALOG && g_prev_state == STATE_TITLE)) {
    g_title_anim.new_color_counter--;
    g_title_anim.color_start_counter--;

    if(g_title_anim.color_start_counter <= 0 && g_title_anim.color_start == 0 ){
      g_title_anim.color_start = 1;
    }

    if(g_title_anim.new_color_counter <= 0) {
      g_title_anim.update_title_color = 1;
      g_title_anim.new_color_counter = NEW_COLOR_COUNT;
    }

  }

  /* Update the elapsed time counter for the on-screen display */
  if(g_game_timer_running)
    g_time_to_update_elapsed--;
  if (g_time_to_update_elapsed <= 0) {
    g_elapsed_time++;
    /* Dump memory info every so often
    if(g_elapsed_time % 20 == 0)
      print_mem_free();
    */
    g_time_to_update_elapsed = FRAME_RATE;
    g_components.render_status_text = 1;
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
    printf("\nfree phys: %d bytes\nfree virtual: %d bytes\n",
          (int)_go32_dpmi_remaining_physical_memory(),
          (int)_go32_dpmi_remaining_virtual_memory());
}

/*=============================================================================
 * main
 *============================================================================*/
int main(int argc, char *argv[]) {

  allegro_init();
  install_keyboard();
  install_timer();

  install_mouse();

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
  init_defaults();

  set_mouse_sprite(g_mouse_cursor);

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

    /* Done in the loop, wait for the next frame */
    g_next_frame = 0;
  }

  free_picture_file(g_picture);
  unload_datafile(g_res);
  free_graphics();
  destroy_bitmap(buffer);

  set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
  allegro_exit();

  return 0;
}
