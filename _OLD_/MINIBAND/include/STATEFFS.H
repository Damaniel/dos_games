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
#ifndef __STATEFFS_H__
#define __STATEFFS_H__

// The maximum amount of resist that the player can have (representing 90%
// resistance).
#define MAX_RESIST      90

// effects that can be applied
#define EFFECT_NONE      0
#define EFFECT_POISON    1
#define EFFECT_DISEASE   2
#define EFFECT_CONFUSE   3
#define EFFECT_BLIND     4
#define EFFECT_WEAK      5
#define EFFECT_SLOW      6
#define EFFECT_BURN      7
#define EFFECT_FREEZE    8
#define EFFECT_SHOCK     9

// Strengths of non-elemental effects
#define NO_EFFECT        0
#define WEAK_EFFECT      1
#define MEDIUM_EFFECT    2
#define STRONG_EFFECT    3

// Range for elemental effects
#define NO_ELEM_EFFECT       0
#define MINIMUM_ELEM_EFFECT  1
#define MAXIMUM_ELEM_EFFECT  8

// Most effects do a fixed percentage of player HP as damage per turn for a fixed
// number of turns.  These tables contain that data.  In each case, the 0th 
// entry represents the 'not affected by this condition' state, with sequential
// entries representing progressively stronger forms of the effect.

// 4 entries of <percent hp lost per turn, number of turns>
extern unsigned char g_poison_table[8];
// 4 entries of <percent hp lost per turn>
extern unsigned char g_disease_table[4];
// 3 entries of <percent dex lost, number of turns>
extern unsigned char g_blind_table[6];
// 4 entries of <percent str and con lost, number of turns>
extern unsigned char g_weak_table[8];
// 4 entries of <percent spd lost, number of turns>
extern unsigned char g_slow_table[8];

// Confusion has only one effect and a fixed duration
extern unsigned char g_confuse_duration;

// Burning, freezing and shocking effects do a wider range of damage.  Its base
// value is still a percentage of the player's health, but can be modified
// later by resistances.  All 3 effects have a fixed turn duration.
extern unsigned char g_burn_table[9];
extern unsigned char g_freeze_table[9];
extern unsigned char g_shock_table[9];
extern unsigned char g_burn_duration;
extern unsigned char g_freeze_duration;
extern unsigned char g_shock_duration;

typedef struct {
    // Strength of poison and how many turns remain
    unsigned int poison            : 2;
    unsigned int poison_turns      : 6;

    // Is the player confused, and for how many more turns?
    unsigned int confusion         : 2;
    unsigned int confusion_turns   : 6;

    // Strength of blindness and how many turns remain
    unsigned int blind             : 2;
    unsigned int blind_turns       : 6;

    // Strength of weakness and how many turns remain
    unsigned int weak              : 2;
    unsigned int weak_turns        : 6;

    // Strength of slowness and how many turns remain
    unsigned int slow              : 2;
    unsigned int slow_turns        : 6;

    // Strength of burning and how many turns remain
    unsigned int burning           : 3;
    unsigned int burning_turns     : 5;

    // Strength of freezing and how many turns remain
    unsigned int freezing          : 3;
    unsigned int freezing_turns    : 5;

    // Strength of shocking and how many turns remain
    unsigned int shocking          : 3;
    unsigned int shocking_turns    : 5; 

    // Strength of the player's disease.  Disease doesn't wear off naturally.
    unsigned int disease           : 4;
    unsigned int                   :12;

} StatusEffects;

#endif