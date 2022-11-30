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
#define KEY_EQUALS              0x0D
#define KEY_A                   0x1E
#define KEY_B                   0x30
#define KEY_C                   0x2E
#define KEY_D                   0x20
#define KEY_E                   0x12
#define KEY_F                   0x21
#define KEY_G                   0x22
#define KEY_H                   0x23
#define KEY_I                   0x17
#define KEY_J                   0x24
#define KEY_K                   0x25
#define KEY_L                   0x26
#define KEY_M                   0x32
#define KEY_N                   0x31
#define KEY_O                   0x18
#define KEY_P                   0x19
#define KEY_Q                   0x10
#define KEY_R                   0x13
#define KEY_S                   0x1F
#define KEY_T                   0x14
#define KEY_U                   0x16
#define KEY_V                   0x2F
#define KEY_W                   0x11
#define KEY_X                   0x2D
#define KEY_Y                   0x15
#define KEY_Z                   0x2C
#define KEY_0                   0x0B
#define KEY_1                   0x02
#define KEY_2                   0x03
#define KEY_3                   0x04
#define KEY_4                   0x05
#define KEY_5                   0x06
#define KEY_6                   0x07
#define KEY_7                   0x08
#define KEY_8                   0x09
#define KEY_9                   0x0A

void process_input(void);
void process_main_screen_input(unsigned char ascii_code, 
                               unsigned char scan_code, 
                               unsigned char shift_status);
void process_palette_edit_input(unsigned char ascii_code, 
                                unsigned char scan_code,
                                unsigned char shift_status);
void process_help_screen_input(unsigned char ascii_code, 
                               unsigned char scan_code, 
                               unsigned char shift_status);
void process_place_player_input(unsigned char ascii_code, 
                               unsigned char scan_code, 
                               unsigned char shift_status);


void move_left(unsigned char shift_status);
void move_right(unsigned char shift_status);
void move_up(unsigned char shift_status);
void move_down(unsigned char shift_status);
#endif
