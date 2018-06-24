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
unit globals;

interface
uses 
  jfunc,
  timer,
  txtgraph,
  undo;

const
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
  STATE_INGAME = 4;
  STATE_WIN = 5;
  STATE_PAUSE = 6;
  STATE_EXIT = 7;

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
  
implementation

end.
