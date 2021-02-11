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
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "stateffs.h"

typedef struct {
    // Base stats.  The cur_* values represent the value of a stat for the current
    // turn, while max_hp (in the case of hp), or non-cur* values (in the case of
    // the other stats) represent the nominal value; that is, the value that the
    // player would have assuming no status effects are adjusting the value.
    // Keeping both of these values in sync is a little harder and uses a little
    // more space, but makes status effect math and attack/defense math easier
    // by precomputing the adjusted values.
    unsigned int  cur_hp;
    unsigned int  max_hp;
    unsigned char cur_str;
    unsigned char str;
    unsigned char cur_dex;
    unsigned char dex;
    unsigned char cur_con;
    unsigned char con;
    unsigned char cur_spd;
    unsigned char spd;

    // Calculated stats (largely gear-based)
    unsigned int attack;
    unsigned int defense;

    // Resists.  The player can have between 0 and 90% resist.  If the player
    // has over 70% resist, they are immune to the associated status effect that
    // can be inflicted by that damage type.
    unsigned char fire_res;
    unsigned char cold_res;
    unsigned char lit_res;

    // The collection of status effects.
    StatusEffects effects;

} Player;

void apply_status_to_player(Player *p, int effect, int strength);
void remove_status_from_player(Player *p, int effect);

#endif