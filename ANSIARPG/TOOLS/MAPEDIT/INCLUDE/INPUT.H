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
#ifndef __INPUT_H__
#define __INPUT_H__

// Keyboard scancodes
#define KEY_ESC                 0x01
#define KEY_UP                  0x48
#define KEY_DOWN                0x50
#define KEY_LEFT                0x4B
#define KEY_RIGHT               0x4D
#define KEY_LBRACKET            0x1A
#define KEY_RBRACKET            0x1B
#define KEY_SPACE               0x39
#define KEY_BACKSLASH           0x2B
#define KEY_ENTER               0x1C
#define KEY_BACKSPACE           0x0E

void process_input(void);
void process_main_screen_input(unsigned char ascii_code, 
                               unsigned char scan_code, 
                               unsigned char shift_status);
void process_palette_edit_input(unsigned char ascii_code, 
                                unsigned char scan_code,
                                unsigned char shift_status);

#endif
