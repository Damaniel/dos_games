//==========================================================================================
//   Daminiband
//
//   Copyright (c) 2021 Shaun Brandt / Holy Meatgoat Software
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in all
//   copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//   SOFTWARE.
//==========================================================================================
#include "stateffs.h"

// See stateffs.h for a description of these tables
unsigned char g_poison_table[8] = {0, 0, 1, 10, 2, 15, 5, 20};
unsigned char g_disease_table[4] = {0, 1, 2, 3};
unsigned char g_blind_table[6] = {0, 0, 50, 10, 90, 20};
unsigned char g_weak_table[8] = {0, 0, 15, 10, 25, 15, 50, 20};
unsigned char g_slow_table[8] = {0, 0, 25, 10, 50, 5, 80, 20};

unsigned char g_burn_table[9] = {0, 1, 2, 4, 6, 8, 10, 12, 15};
unsigned char g_freeze_table[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
unsigned char g_shock_table[9] = {0, 1, 1, 2, 2, 3, 3, 4, 5};
unsigned char g_burn_duration = 10;
unsigned char g_freeze_duration = 15;
unsigned char g_shock_duration = 10;