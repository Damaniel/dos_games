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
#include "player.h"
#include "stateffs.h"

void remove_status_from_player(Player *p, int status) {
    return;
}

void apply_status_to_player(Player *p, int status, int strength) {

    int adjusted_strength;
    int table_offset;

    adjusted_strength = strength;

    // If poison, disease, weak or slow, clamp the strength range
    if (status == EFFECT_POISON || status == EFFECT_DISEASE || 
        status == EFFECT_WEAK || status == EFFECT_SLOW) {
        if (adjusted_strength < NO_EFFECT) {
            adjusted_strength = NO_EFFECT;
        }
        if (adjusted_strength > STRONG_EFFECT) {
            adjusted_strength = STRONG_EFFECT;
        }
        // adjust table_offset
        table_offset = adjusted_strength * 2;
    }
    // If blind, clamp to a (narrower) range
    if (status == EFFECT_BLIND) {
        if (adjusted_strength < NO_EFFECT) {
            adjusted_strength = NO_EFFECT;
        }
        if (adjusted_strength > MEDIUM_EFFECT) {
            adjusted_strength = MEDIUM_EFFECT;
        }
        // adjust table offset
        table_offset = adjusted_strength;
    }
    // If elemental effect, clamp to a (wider) range
    if (status == EFFECT_BURN || status == EFFECT_FREEZE || status == EFFECT_SHOCK) {
        if (adjusted_strength < NO_ELEM_EFFECT) {
            adjusted_strength = NO_ELEM_EFFECT;
        }
        if (adjusted_strength > MAXIMUM_ELEM_EFFECT) {
            adjusted_strength = MAXIMUM_ELEM_EFFECT;
        }
        table_offset = adjusted_strength;
    }

    switch (status) {
        case EFFECT_POISON:
            p->effects.poison = g_poison_table[table_offset];
            p->effects.poison_turns = g_poison_table[table_offset+1];
            break;
        case EFFECT_DISEASE:
            p->effects.disease = g_disease_table[table_offset];
            break;
        case EFFECT_BLIND:
            p->effects.blind = g_blind_table[table_offset];
            p->effects.blind_turns = g_blind_table[table_offset+1];
            break;
        case EFFECT_WEAK:
            p->effects.weak = g_weak_table[table_offset];
            p->effects.weak_turns = g_weak_table[table_offset+1];
            break;
        case EFFECT_SLOW:
            p->effects.slow = g_slow_table[table_offset];
            p->effects.slow_turns = g_slow_table[table_offset+1];
            break;
        case EFFECT_BURN:
            p->effects.burning = g_burn_table[table_offset];
            p->effects.burning_turns = g_burn_duration;
            break;
        case EFFECT_FREEZE:
            p->effects.freezing = g_freeze_table[table_offset];
            p->effects.freezing_turns = g_freeze_duration;
            break;
        case EFFECT_SHOCK:
            p->effects.shocking = g_shock_table[table_offset];
            p->effects.shocking_turns = g_shock_duration;
            break;
        default:
            // do nothing since we don't recognize the effect.
            break;
    };
}