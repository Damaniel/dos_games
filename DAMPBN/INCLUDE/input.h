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
#ifndef __INPUT_H__
#define __INPUT_H__

typedef struct {
  int x;
  int y;
} Position;

/*=============================================================================
 * process_input
 *
 * Take keypresses and perform the appropriate actions.  'Appropriate' actions
 * vary based on the game state.
 *============================================================================*/
void process_input(int state);

/*=============================================================================
 * input_state_help
 *
 * Process input for the help state (g_cur_state = STATE_HELP)
 *============================================================================*/
void input_state_help(void);

/*=============================================================================
 * input_state_logo
 *
 * Process input for the logo state (g_cur_state = STATE_LOGO)
 *============================================================================*/
void input_state_logo(void);

/*=============================================================================
 * input_state_title
 *
 * Process input for the in game state (g_cur_state = STATE_TITLE)
 *============================================================================*/
void input_state_title(void);

/*=============================================================================
 * input_state_load_dialog
 *
 * Process input for the load dialog state (g_cur_state = STATE_LOAD_DIALOG)
 *============================================================================*/
void input_state_load_dialog(void);

/*=============================================================================
 * input_state_game
 *
 * Process input for the in game state (g_cur_state = STATE_GAME)
 *============================================================================*/
void input_state_game(void);

/*=============================================================================
 * input_state_map
 *
 * Process input for the map state (g_cur_state = STATE_MAP)
 *============================================================================*/
void input_state_map(void);

/*=============================================================================
 * input_state_replay
 *
 * Process input for the complete state (g_cur_state = STATE_REPLAY)
 *============================================================================*/
void input_state_replay(void);

/*=============================================================================
 * update_mouse_status
 *
 * Unlocks the mouse click lockout.
 *============================================================================*/
void update_mouse_status(void);

/*=============================================================================
 * mouse_clicked_here
 *
 * Check to see if the mouse button has been pressed inside the box defiend
 * by x1, x2, y1 and y2.  If lockout is non-zero, further clicks won't be 
 * processed until the mouse button is released.
 *============================================================================*/
int mouse_clicked_here(int x1, int y1, int x2, int y2, int lockout);

/*=============================================================================
 * process_palette_press
 *
 * Process mouse and keyboard input for the palette page buttons
 *============================================================================*/
void process_palette_press(void);

/*=============================================================================
 * process_palette_color_press
 *
 * Process mouse and keyboard input for the palette color swatch area
 *============================================================================*/
void process_palette_color_press(void);

/*=============================================================================
 * process_mark_press
 *
 * Process mouse and keyboard input for the Mark button
 *============================================================================*/
void process_mark_press(void);

/*=============================================================================
 * process_help_press
 *
 * Process mouse and keyboard input for the Help button
 *============================================================================*/
void process_help_press(void);

/*=============================================================================
 * process_exit_press
 *
 * Process mouse and keyboard input for the Exit button
 *============================================================================*/
void process_exit_press(void);

/*=============================================================================
 * process_map_press
 *
 * Process mouse and keyboard input for the Map button
 *============================================================================*/
void process_map_press(void);

/*=============================================================================
 * process_style_press
 *
 * Process mouse and keyboard input for the Style button
 *============================================================================*/
void process_style_press(void);

/*=============================================================================
 * process_save_press
 *
 * Process mouse and keyboard input for the Save button
 *============================================================================*/
void process_save_press(void);

/*=============================================================================
 * process_load_press
 *
 * Process mouse and keyboard input for the Load button
 *============================================================================*/
void process_load_press(void);

/*=============================================================================
 * process_main_area_keyboard_input
 *
 * Process all keyboard-specific input related to the main game area
 * (moving the cursor, clicking on a space)
 *============================================================================*/
void process_main_area_keyboard_input(void);

/*=============================================================================
 * process_main_area_mouse_input
 *
 * Process all mouse-specific input related to the main game area
 * (clicking/dragging on spaces, hiding the cursor while the mouse is moving)
 *============================================================================*/
void process_main_area_mouse_input(void);

/*=============================================================================
 * is_in_game_area
 *
 * Returns 1 if the mouse is in the main draw area, otherwise 0.
 *============================================================================*/
int is_in_game_area(int x, int y);

/*=============================================================================
 * is_in_overview_area
 *
 * Returns 1 if the mouse is in the overview area, otherwise 0.
 *============================================================================*/
int is_in_overview_area(int x, int y);

#endif
