/* Copyright 2022 Shaun Brandt
   
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
#ifndef __MAPEDIT_H__
#define __MAPEDIT_H__

// Structures are all defined here
#include "structs.h"
// All defines/constants are defined here
#include "defines.h"
// Text drawing library
#include "textdraw.h"
// Code related to drawing the screen
#include "render.h"
// Code related to keyboard input
#include "input.h"
// Helper functions
#include "util.h"

// Standard library stuff
#include <bios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// --- extern definitions for program globals ---

// The game state
extern State g_state;

// UI, app and palette menu configurations (to control features of each)
extern UIConfig g_ui_config;
extern AppConfig g_app_config;
extern PaletteMenuConfig g_palette_menu_config;

// The tile palette
extern PaletteEntry g_map_palette[];

// The map exists
extern Exit g_exit_list[];

// The map itself
extern unsigned char g_map[][MAP_HEIGHT];

void set_state(State s);

#endif
