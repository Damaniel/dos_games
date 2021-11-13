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

/*=============================================================================
 * process_input
 *
 * Take keypresses and perform the appropriate actions.  'Appropriate' actions
 * vary based on the game state.
 *============================================================================*/
void process_input(int state);

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

#endif
