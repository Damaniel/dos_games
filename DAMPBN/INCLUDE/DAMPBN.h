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
#ifndef __DAMPBN_H__
#define __DAMPBN_H__

typedef enum {
  STATE_NONE,
  STATE_LOGO,
  STATE_TITLE,
  STATE_GAME,
  STATE_LOAD,
  STATE_SAVE,
  STATE_HELP,
  STATE_OPTS,
  STATE_MAP,
  STATE_LOAD_DIALOG,
  STATE_FINISHED,
  STATE_REPLAY
} State;

/* Definitions from dpmi.h */
unsigned long _go32_dpmi_remaining_physical_memory(void);
unsigned long _go32_dpmi_remaining_virtual_memory(void);


/*=============================================================================
 * change_state
 *
 * Change state in the state machine 
 *============================================================================*/
void change_state(State new_state, State prev_state);

/*=============================================================================
 * game_timer_set
 *
 * Used to start or stop the game timer 
 *============================================================================*/
void game_timer_set(int status);

/*=============================================================================
 * do_render
 *
 * Update the display.  Done 30 times per second, or can be forced on demand.
 * (An example is any dialog that shows transiently as part of a state change.
 *  The 'Saving' message that's rendered in the STATE_SAVE state is does this.)
 *============================================================================*/
void do_render(void);

/*=============================================================================
 * process_timing_stuff
 *
 * Decrement counters and do things when they expire
 *============================================================================*/
void process_timing_stuff(void);

/*=============================================================================
 * print_mem_free
 *
 * Debug function.  Show free DPMI memory (physical + virtual)
 *============================================================================*/
void print_mem_free(void);

#endif
