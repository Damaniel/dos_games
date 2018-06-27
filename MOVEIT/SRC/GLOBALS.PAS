{ globals - a unit containing all of the components that may be shared among different
                parts of Moveit.

          Copyright (c) 2018, Shaun Brandt / Holy Meatgoat Software
          All rights reserved.

          Redistribution and use in source and binary forms, with or without
          modification, are permitted provided that the following conditions are met:
              * Redistributions of source code must retain the above copyright
                notice, this list of conditions and the following disclaimer.
              * Redistributions in binary form must reproduce the above copyright
                notice, this list of conditions and the following disclaimer in the
                documentation and/or other materials provided with the distribution.
              * Neither the name of Holy Meatgoat Software nor the
                names of its contributors may be used to endorse or promote products
                derived from this software without specific prior written permission.

          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
          DISCLAIMED. IN NO EVENT SHALL HOLY MEATGOAT SOFTWARE BE LIABLE FOR ANY
          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       
}

{ Tile indices inside MOVEIT.DAT: 
    0 - 4: Stage elements (wall, floor, target, box, player)
    5 - 8: Tiling background elements
    9 - 202: Title screen elements
    203 - 242: Game logo elements
}

unit globals;

interface
uses 
  jfunc,
  timer,
  txtgraph,
  undo;

const
  FRAME_RATE = 30;

  DIR_UP = 0;
  DIR_LEFT = 1;
  DIR_RIGHT = 2;
  DIR_DOWN = 3;

  { Keyboard scan codes for non-extended keys }
  KEY_ESC = 1;

  { Values used by the keyboard handler }
  KEY_CLEAR = 0;
  KEY_DOWN = 1;
  KEY_PRESSED = 2;

  { Values that represent the current item positioned on a single square of the
    level.  This could be empty (no stage content), walls, floors, the player, 
    boxes, and so on.  This is distinct from the ID_* series of values, which
    contain the current item positioned on a grid square as to be rendered by the
    graphical engine.}
  LEVEL_EMPTY  = 0;
  LEVEL_WALL   = 1;
  LEVEL_FLOOR  = 2;
  LEVEL_TARGET = 3;
  LEVEL_BOX    = 4;
  LEVEL_PLAYER = 5;
  
  { Values that represent the current item positioned on a single square of the level,
   from a rendering perspective.  Determines the specific tile to be placed at a 
   location. These actually represent offsets into the tile array. }
  ID_WALL = 0;
  ID_FLOOR = 1;
  ID_TARGET = 2;
  ID_BOX = 3;
  ID_PLAYER = 4;
  
  { Values representing repeating background tiles. }
  ID_BACKGROUND1 = 5;
  ID_BACKGROUND2 = 6;
  ID_BACKGROUND3 = 7;
  ID_BACKGROUND4 = 8;
  
  { this is the maximum size of a single level.  The size of sprite used to render
    elements depends on the size of the level.  }
  MAX_WIDTH = 64;
  MAX_HEIGHT = 64;
  
  { This is the largest number of boxes that can be contained in a single stage.  Most
    stages have far fewer boxes than this. }
  MAX_BOXES = 512;
  
  { The x and y position of the center of the play area (not the screen).  Used to
    center the stage on the screen. }
  CENTER_OF_PLAY_AREA_X = 50;
  CENTER_OF_PLAY_AREA_Y = 50;
  
  { The size of the tiles that make up the play area. }
  SMALL_TILE_SIZE = 3;  {Currently unused.  May be used for larger stages.}
  LARGE_TILE_SIZE = 5;

  { Various states for the state machine. }
  STATE_TITLE = 0;
  STATE_OPTIONS = 1;
  STATE_SAVE = 2;
  STATE_LOAD = 3;
  STATE_LEVEL_DIALOG = 4;
  STATE_INGAME = 5;
  STATE_WIN = 6;
  STATE_PAUSE = 7;
  STATE_EXIT = 8;

  { Screen offsets for dynamic text in-game. }
  LEVEL_STRING_X = 105;
  LEVEL_STRING_Y =  30;
  LEVEL_VAL_X    = 130;
  LEVEL_VAL_Y    =  38;

  SCORE_STRING_X = 105;
  SCORE_STRING_Y =  46;
  SCORE_VAL_X    = 130;
  SCORE_VAL_Y    =  54;

  MOVES_STRING_X = 105;
  MOVES_STRING_Y =  62;
  MOVES_VAL_X    = 130;
  MOVES_VAL_Y    =  70;

  TIME_STRING_X  = 105;
  TIME_STRING_Y  =  78;
  TIME_VAL_X     = 130;
  TIME_VAL_Y     =  86;

  BLACK_BAR_X1   = 104;
  BLACK_BAR_X2   = 155;

  POINTS_PER_SECOND = 50;
  POINTS_PER_MOVE = 50;

  { The tiles that make up the game logo in the upper right corner of the game window. }
  g_gameLogo: array[0..39] of Byte=(
     203, 204, 205, 206, 207, 208, 209, 210, 211, 212,
     213, 214, 215, 216, 217, 218, 219, 220, 221, 222,
     223, 224, 225, 226, 227, 228, 229, 230, 231, 232,
     233, 234, 235, 236, 237, 238, 239, 240, 241, 242);

type
  { Grid offset in the play area. }
  location = record
    x, y: Integer;
  end;
    
  { A 'dirty' tile - contains a location on the grid and what *should* be drawn on it.
    A list of these is used to redraw only parts of the screen that have changed. }
  dirty = record
    x, y: Integer;
    item: Integer;
  end;

var
  g_ExitProgram: Boolean;

  { The frame rate of the game.  Set to 60, to play at 60 frames per
    second (assuming no slowdown). The Wait() function works in these
    tick values, so if the rate is set to 60, then Wait(60) waits for
    one second, Wait(120) for two seconds, and so on. }
  g_TimerInterval: Word;

  { A pointer to the old DOS keyboard handler, so we can restore it on
    program exit. }
  g_DefKBInt: pointer;

  { The tile sprite data. }
  g_GameTiles: tDynamicData;
  
  { Font data }
  g_font: pFontSet;

  { The data for the current level. }
  g_levelData: array[0..(MAX_WIDTH * MAX_HEIGHT)] of byte;
  g_levelWidth: Integer;
  g_levelHeight: Integer;
  g_levelStartX: Integer;
  g_levelStartY: Integer;
  
  { A generic global status variable. }
  g_status: Integer;
  
  { The position of the boxes and targets on the stage grid. }
  g_boxLocations: array[0..MAX_BOXES] of location;
  g_targetLocations: array[0..MAX_BOXES] of location;
  
  { Box and target count.  These should almost always be the same. }
  g_numBoxes: Integer;
  g_numTargets: Integer;
  
  { The player position in the stage grid. }
  g_playerX: Integer;
  g_playerY: Integer;
  
  { Flags used to decide what will get redrawn, if anything, on the next frame. }
  g_redrawAll: Boolean;
  g_redrawDirtyLevelArea: Boolean;
  g_redrawLevelDialog: Boolean;

  { A list of all 'dirty' blocks that need to be redrawn on the next frame. }
  g_dirtyBlocks: array[0..399] of dirty;
  g_numDirtyBlocks: Integer;

  { The undo buffer.  Currently one move.  Eventually multiple moves. }
  g_undoBuffer: UndoBuf;

  { Current state in the state machine. }
  g_currentState: Integer;

  { Previous state in the state machine. }
  g_previousState: Integer;

  { The current stage the player is on. }
  g_currentLevel: Integer;

  { The number of levels embedded in the data file. }
  g_numLevels: Integer;

  { The player's cumulative score. }
  g_score: Longint;

  { The number of moves (moves + undos) made in the current level. }
  g_numMoves: Integer;

  { The number of seconds since the level started. }
  g_elapsedTime: Integer;

  { A rough count of rendered frames.  Used to track time (very roughly) since the timer interrupt 
    has been replaced with something higher-resolution (but with less functionality...)
    Will overflow after a year of continious running.  Ha.}
  g_frameCounter: Longint;

  { Used to determine when the number of frames equates to a second beyond the previous target. 
    Tracks the amount of time since a new level started. }
  g_frameTarget: Integer;

  g_drawLevel: Boolean;
  g_drawScore: Boolean;
  g_drawMoves: Boolean;
  g_drawTime:  Boolean;

  { A count of boxes sitting on targets. Through careful adding and removing of this variable when boxes
   are moved around, we can check for end of stage with only a single comparison as opposed to scanning
   box and target locations. }
  g_numBoxesOnTargets : Integer;

  g_parTime : Integer;
  g_targetMoves: Integer;

implementation

end.
