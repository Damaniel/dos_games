{ Damanoid - An Arkanoid clone, using the 160x100, 16-color CGA text
             'graphics' mode.

             October/November 2014, Shaun Brandt / Holy Meatgoat Software.

             This code is public domain!
}
program Damanoid;
uses
  Dos,                  { for GetIntVec, SetIntVec }
  jfunc,
  timer,
  txtgraph;

const
  { Arrow key flags - held in the extended key array }
  DIR_UP = 0;
  DIR_LEFT = 1;
  DIR_RIGHT = 2;
  DIR_DOWN = 3;

  { Keyboard scan codes for non-extended keys }
  KEY_ESC = 1;

  KEY_CLEAR = 0;
  KEY_DOWN = 1;
  KEY_PRESSED = 2;

  { Sprite sheet offsets used directly in the code (not loaded from level
    or background data files). }
  BORDER_CORNER_TILE = 38;
  BORDER_VERT_TILE = 39;
  BORDER_HORZ_TILE = 40;
  PADDLE_LEFT_TILE = 53;
  PADDLE_MIDDLE_TILE = 54;
  PADDLE_RIGHT_TILE = 55;
  BALL_TILE = 56;

  PADDLE_SPEED = 125;

  { The sprites in the sprite sheet that correspond to the 'Damanoid'
    logo displayed on the upper corner of the playfield. }
  g_LogoSprites: array[0..49] of Byte=(
     57, 58, 59, 60, 61, 58, 59, 60, 61, 62,
     63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
     73, 74, 75, 76, 77, 75, 78, 79, 80, 81,
     82, 83, 84, 85, 86, 84, 87, 88, 89, 90,
     91, 92, 93, 94, 95, 92, 93, 94, 95, 96);

  { Depending on how far from the center of the paddle the ball is hit,
    the speed is set to one of these amounts accordingly.  Direction may
    or may not be maintained as a result of whether the paddle is actively
    moving or not. }
  g_SpeedsNormal: array[0..9] of Integer=(
     50, 60, 70, 80, 90, 100, 110, 120, 130, 140);

  { The number of levels in the game.  Currently 3 experimental ones. }
  g_NumLevels = 4;

var
  g_ExitProgram: Boolean;

  g_KeyScanCodes: array[0..127] of Byte;     { Holds status of normal keys }
  g_ExtendedKeyCodes: array[0..3] of Byte;   { Holds status of arrow keys }

  { Used by the interrupt handler to hold the recived key. }
  g_ScanCodeTemp: Byte;
  g_ExtendedDetected: Boolean;

  { - g_CurrentBoardBG holds the tile data for the board and background, but
      not the bricks.
    - g_CurrentLevelData holds the tile data for the bricks on the board.
    - g_HitsRemaining holds a count of the number of hits a particular
      block needs before it breaks.
    - g_DirtyBlocks is a list of tiles on the board that have changed since
      the last redraw (and as such, need to be drawn again).  The number
      of valid dirty blocks is held in g_NumDirtyBlocks.
  }
  g_CurrentBoardBg: array[0..399] of Byte;
  g_CurrentLevelData: array[0..399] of Byte;
  g_HitsRemaining: array[0..399] of Byte;
  g_DirtyBlocks: array[0..399] of Integer;
  g_NumDirtyBlocks: Integer;

  { Sprite data for the backgrounds, paddle, ball, and bricks. }
  g_GameTiles: tDynamicData;

  { A status flag used to hold the results of opening/reading level data. }
  g_Status: Integer;

  { The level the player is currently on. }
  g_CurrentLevel: Integer;

  { The current number of lives a player has. }
  g_PlayerLives: Integer;

  { The player's score }
  g_PlayerScore: LongInt;
  g_HighScore: LongInt;
  g_OneUpTarget: LongInt;

  { These are actually very low precision fixed point numbers.  The actual
    value of the position or speed is (value / 100).  The division is
    only done when specifically needed (when drawing to the screen), and
    is a truncating integer division (DIV).  Since minor rounding errors
    aren't a big deal, and this beats having floating point code, it is
    what it is. }
  g_BallX: Integer;
  g_BallY: Integer;
  g_BallNewX: Integer;
  g_BallNewY: Integer;
  g_BallXSpeed : Integer;
  g_BallYSpeed : Integer;
  g_PaddleX: Integer;
  g_PaddleY: Integer;
  g_PaddleWidth : Integer;

  { The number of 5-pixel wide middle tiles used in the paddle. }
  g_NumMiddleSections: Integer;

  { The number of unbroken *breakable* blocks left to break in the stage }
  g_BlocksRemaining: Integer;

  { The frame rate of the game.  Set to 60, to play at 60 frames per
    second (assuming no slowdown). }
  g_TimerInterval: Word;

  { A pointer to the old DOS keyboard handler, so we can restore it on
    program exit. }
  g_DefKBInt: pointer;

  { Base score values for each block type.  The sprites representing
    hittable blocks take up the first 20 sprites in the sprite sheet; this
    array just maps the same indices to score values. }
  g_BlockScores: array[0..35] of Integer;

  g_Font: pFontSet;

  g_UpdateText: Boolean;

procedure DrawPaddle; forward;
procedure DrawLives; forward;
procedure Wait(ticks: Integer); forward;
procedure InitPositions; forward;
procedure DrawBall; forward;
procedure StartOnTimeOrPress(ticks: Integer); forward;
procedure RedrawDirtyBlocks; forward;

{ KBInt - A replacement handler for the DOS keyboard interrupt.  Tracks
          the current key up/down status for important keys, plus certain
          'key' keypresses. }
procedure KBInt(Flags, CS, IP, AX, BX, CX, DX, SI, DI, DS, ES, BP : Word);
interrupt;
begin
  asm
     cli
     in   al, $60
     mov  g_ScanCodeTemp, al
     in   al, $61
     mov  bl, al
     or   al, $80
     out  $61, al
     mov  al, bl
     out  $61, al
     mov  al, $20
     out  $20, al
     sti
  end;

  { Check for the scan code E0h.  This code means that the key pressed
    was an 'extended' key, and the next scan code will actually signify
    the key pressed. }
  if(g_ScanCodeTemp = $E0) then
  begin
     g_ExtendedDetected := True;
  end
  else begin
     { If the previous scan code was an extended marker, determine if the
       current code corresponds to either a press or release of an arrow
       key.  If so, set the state of g_ExtendedKeyCodes accordingly. }
     if(g_ExtendedDetected = True) then begin
        if (g_ScanCodeTemp = $48) then
           g_ExtendedKeyCodes[DIR_UP] := KEY_DOWN;
        if (g_ScanCodeTemp = $4B) then
           g_ExtendedKeyCodes[DIR_LEFT] := KEY_DOWN;
        if (g_ScanCodeTemp = $4D) then
           g_ExtendedKeyCodes[DIR_RIGHT] := KEY_DOWN;
        if (g_ScanCodeTemp = $50) then
           g_ExtendedKeyCodes[DIR_DOWN] := KEY_DOWN;
        if (g_ScanCodeTemp = $C8) then
           g_ExtendedKeyCodes[DIR_UP] := KEY_CLEAR;
        if (g_ScanCodeTemp = $CB) then
           g_ExtendedKeyCodes[DIR_LEFT] := KEY_CLEAR;
        if (g_ScanCodeTemp = $CD) then
           g_ExtendedKeyCodes[DIR_RIGHT] := KEY_CLEAR;
        if (g_ScanCodeTemp = $D0) then
           g_ExtendedKeyCodes[DIR_DOWN] := KEY_CLEAR;
        g_ExtendedDetected := False;
     end
     else begin
        { If the scan code is greater than 7fh, then the code corresponds to
          a key release, otherwise, it corresponds to a key press. }
        if(g_ScanCodeTemp > $7F) then begin
           g_KeyScanCodes[(g_ScanCodeTemp and $7F)] := KEY_PRESSED;
        end else begin
           g_KeyScanCodes[(g_ScanCodeTemp)] := KEY_DOWN;
        end;
     end;
  end;
end;

procedure ClearKeyPresses;
var
   i: Integer;
begin
   for i:=0 to 127 do
      if (g_KeyScanCodes[i] = KEY_PRESSED) then
         g_KeyScanCodes[i] := KEY_CLEAR;
end;

{ AddToDirtyList - marks a particular 5x5 tile on the gamefield portion of
                   the screen as requiring an update on the next frame. }
procedure AddToDirtyList(val: Integer);
begin
      g_DirtyBlocks[g_NumDirtyBlocks] := val;
      inc(g_NumDirtyBlocks, 1);
end;

{ AddPaddlePosToDirtyList - adds all 5x5 tile regions covered by the
                            current paddle position to the dirty list. }
procedure AddPaddlePosToDirtyList;
var
   startOffset: Integer;
   i: Integer;
begin
   { The paddle is always on the bottom row.  There are 20 rows of 20
     tiles, so the final row starts at offset 380.  We can find the tile
     offset of the paddle itself this way.  The offset we calculate will
     hold at least part of the first paddle tile.  Since we know the width
     of the paddle, we can derive the number of tiles involved.  For a
     paddle of 4 tiles wide, at least four, and at most five, tiles will
     be marked as dirty.
   }
   startOffset := 380 + (g_PaddleX div 500);
   for i:=1 to (g_NumMiddleSections + 4) do
   begin
      AddToDirtyList(startOffset);
      inc(startOffset, 1);
   end;
end;

procedure ProcessPaddleMovement(dir: Integer; var paddleMoved: Boolean);
begin
   paddleMoved := False;
   if (dir = DIR_RIGHT) then
   begin
     if (g_PaddleX + g_PaddleWidth + PADDLE_SPEED <= 9500) then
     begin
         paddleMoved := True;
         AddPaddlePosToDirtyList;
         RedrawDirtyBlocks;
         g_PaddleX := g_PaddleX + PADDLE_SPEED;
         DrawPaddle;
     end;
   end;

   if (dir = DIR_LEFT) then
   begin
       if (g_PaddleX - PADDLE_SPEED >= 500) then
       begin
           paddleMoved := True;
           AddPaddlePosToDirtyList;
           RedrawDirtyBlocks;
           g_PaddleX := g_PaddleX - PADDLE_SPEED;
           DrawPaddle;
       end;
   end;

end;

{ ProcessInput - Determine if a key is pressed.  Do things accordingly. }
procedure ProcessInput;
var
   paddleMoved: Boolean;
begin
    if (g_KeyScanCodes[KEY_ESC] = KEY_PRESSED) then begin
        g_KeyScanCodes[KEY_ESC] := KEY_CLEAR;
        g_ExitProgram := True;
    end;

    if (g_ExtendedKeyCodes[DIR_RIGHT] = KEY_DOWN) then begin
       ProcessPaddleMovement(DIR_RIGHT, paddleMoved);
    end;

    if (g_ExtendedKeyCodes[DIR_LEFT] = KEY_DOWN) then begin
       ProcessPaddleMovement(DIR_LEFT, paddleMoved);
    end;
end;

procedure LoadScoreValues(var status: Integer);
var
   scoreDataFile: file;
   i: Integer;
begin
     assign(scoreDataFile, 'BLOCKSCO.DAT');

     {$I-}
     reset(scoreDataFile, 1);
     {$I+}

     if (IOResult <> 0) then
     begin
          writeln('Unable to load score table!');
          status := -1;
     end
     else begin
          blockread(scoreDataFile, g_BlockScores, 36 * sizeof(Integer));
          close(scoreDataFile);
          status := 0;
     end;
end;


{ LoadLevel - loads the data for a specified level from an external file.
              That file is always called LEVEL<n>.DAT, where '<n>' is an
              integer representing the level number.
}
procedure LoadLevel(var status: Integer);
var
   levelFile: file;
   i: Integer;
begin
     { assign(levelFile, 'LEVEL' + chr(g_CurrentLevel + 48) + '.DAT'); }
     assign(levelFile, 'LEVEL12.DAT');

     {$I-}
     reset(levelFile, 1);
     {$I+}

     if (IOResult <> 0) then
     begin
          writeln('Unable to load level!');
          status := -1;
     end
     else begin
         blockread(levelFile, g_CurrentLevelData, 400);
         blockread(levelFile, g_HitsRemaining, 400);
         close(levelFile);
         status := 0;
         { Count the blocks remaining }
         g_BlocksRemaining := 0;
         for i:=0 to 399 do
         begin
            if (g_HitsRemaining[i] <> $DD) and (g_HitsRemaining[i] <> $FF) then
               inc(g_BlocksRemaining, 1);
         end;
         { We counted each block twice - each has two halves - and so we must
           divide by 2 to get the actual block count. }
         g_BlocksRemaining := g_BlocksRemaining div 2;
     end;
end;

{ LoadBoard - loads background and board tile information.  Used to draw
              (and redraw) the background.
}
procedure LoadBoard(var status: Integer);
var
   boardFile: file;
begin
     assign(boardFile, 'BOARDBG' + chr(g_CurrentLevel + 48) + '.DAT');

     {$I-}
     reset(boardFile, 1);
     {$I+}

     if (IOResult <> 0) then
     begin
          writeln('Unable to load board!');
          status := -1;
     end
     else begin
          blockread(boardFile, g_CurrentBoardBG, 400);
          close(boardFile);
          status := 0;
     end;
end;

{ CheckForExtraLifeAward - Checks the player's current score and compares
                           it to the target score for a one up.  If the
                           player score has passed the target, award the
                           one-up, and set the next target.

                           The targets are:

                           - 50,000 points
                           - 125,000 points
                           - 250,000 points
                           - and every 250,000 thereafter.
}
procedure CheckForExtraLifeAward;
begin
   if (g_PlayerScore >= g_OneUpTarget) then
   begin
      Inc(g_PlayerLives, 1);
      DrawLives;
      if (g_OneUpTarget = 50000) then begin
         g_OneUpTarget := 125000;
      end
      else begin
        if (g_OneUpTarget = 125000) then begin
           g_OneUpTarget := 250000;
        end
        else begin
           g_OneUpTarget := g_OneUpTarget + 250000;
        end;
      end;
   end;
end;
{ DrawPaddle - draws the paddle at the current paddle position. }
procedure DrawPaddle;
var
   i: Integer;
   dx, dy: Integer;
begin
   dx := g_PaddleX div 100;
   dy := g_PaddleY div 100;

   tg_tile5(dx, dy, PADDLE_LEFT_TILE);
   for i:=1 to g_NumMiddleSections do
   begin
      tg_tile5(dx + (i*5), dy, PADDLE_MIDDLE_TILE);
   end;
   tg_tile5(dx + (g_PaddleWidth div 100) - 5, dy, PADDLE_RIGHT_TILE);
end;

{ RedrawDirtyBlocks - for each block marked as dirty in the dirty list,
                      determine whether it is a background or brick tile.
                      Redraw it using the correct graphic.  This prevents
                      us from needing to draw the entire screen every
                      frame (in fact, only a small number of tiles generally
                      need to be redrawn in any given frame).
}
procedure RedrawDirtyBlocks;
var
   i: Integer;
   xOff, yOff: Integer;
begin
     { Only draw if there are dirty blocks. }
     if (g_NumDirtyBlocks > 0) then
     begin
        for i:= 0 to g_NumDirtyBlocks - 1 do
        begin
           yOff := (g_DirtyBlocks[i] div 20) * 5;
           xOff := (g_DirtyBlocks[i] mod 20) * 5;
           if (g_CurrentLevelData[g_DirtyBlocks[i]] = $FF) then begin
               tg_tile5(xOff, yOff, g_CurrentBoardBG[g_DirtyBlocks[i]]);
           end else begin
               tg_tile5(xOff, yOff, g_CurrentLevelData[g_DirtyBlocks[i]]);
           end;
        end;
        { Reset the dirty block list. }
        g_NumDirtyBlocks := 0;
     end;
end;

{ DrawBoard - Draw the tiles that make up the board (including bricks).
}
procedure DrawBoard;
var
i, j: Integer;
begin
     for i:=0 to 19 do
     begin
        for j:=0 to 19 do
        begin
           if (g_CurrentLevelData[(i * 20) + j] = $FF) then begin
               tg_tile5((j*5), (i*5), g_CurrentBoardBG[(i * 20) + j]);
           end else begin
               tg_tile5((j*5), (i*5), g_CurrentLevelData[(i * 20) + j]);
           end;
        end;
     end;
end;

{ DrawBaseComponents - draws all of the static portions of the screen,
                       including the initial board state.
}
procedure DrawBaseComponents;
var
i, j, count: Integer;
begin
     tg_clear(c_black);           { Clear the screen }
     DrawBoard;

     { Draw the border on the right side of the window as well }
     tg_tile5(155, 0, BORDER_CORNER_TILE);
     tg_tile5(155, 95, BORDER_CORNER_TILE);

     for i := 0 to 10 do
     begin
        tg_tile5(100+(i*5), 0, BORDER_HORZ_TILE);
        tg_tile5(100+(i*5), 95, BORDER_HORZ_TILE);
     end;
     for i := 1 to 18 do
        tg_tile5(155, (i*5), BORDER_VERT_TILE);

     g_Font^.outText(103, 34, 'STAGE:', c_ltBlue);
     g_Font^.outText(103, 42, 'SCORE:', c_ltGreen);
     g_Font^.outText(103, 58, 'HIGH SCORE:', c_yellow);
     g_Font^.outText(103, 74, 'PADDLES LEFT:', c_ltRed);

     count := 0;
     for i := 0 to 4 do
     begin
        for j := 0 to 9 do
        begin
           tg_tile5(103 + (j * 5), 7 + (i*5), g_LogoSprites[count]);
           Inc(count, 1);
        end;
     end;
end;

procedure UpdateText;
var
     level: String;
begin
     Str(g_CurrentLevel, level);
     tg_bar(135, 34, 150, 40, c_black);
     tg_bar(110, 50, 150, 55, c_black);
     tg_bar(110, 66, 150, 73, c_black);

     g_Font^.outText(135, 34, level, c_white);
     g_Font^.outTextRight(150, 50, LongToSt8(g_PlayerScore), c_white);
     g_Font^.outTextRight(150, 66, LongToSt8(g_HighScore), c_white);

end;

procedure DrawLifePaddle(x, y: Integer);
begin
     tg_tile5(x, y, PADDLE_LEFT_TILE);
     tg_tile5(x+5, y, PADDLE_MIDDLE_TILE);
     tg_tile5(x+10, y, PADDLE_MIDDLE_TILE);
     tg_tile5(x+15, y, PADDLE_RIGHT_TILE);
end;

{ DrawLives - draws a paddle on the screen for each life (other than the
              current one) that the player has.  That is, if the player has
              three total lives, this function will draw two paddles (the
              third paddle is on the playfield).  Up to six paddles will
              be drawn in the designated space.
}
procedure DrawLives;
begin
   tg_bar(105, 82, 150, 93, c_black);
   if (g_PlayerLives > 1) then
      DrawLifePaddle(105, 82);
   if (g_PlayerLives > 2) then
      DrawLifePaddle(105, 88);
   if (g_PlayerLives > 3) then
      DrawLifePaddle(130, 82);
   if (g_PlayerLives > 4) then
      DrawLifePaddle(130, 88);
end;

procedure StartNextLevel;
begin
     Inc(g_CurrentLevel, 1);
     if (g_CurrentLevel > g_NumLevels) then
     begin
        g_ExitProgram := True;
     end else begin
        LoadBoard(g_Status);
        LoadLevel(g_Status);
        InitPositions;
        DrawBaseComponents;
        DrawPaddle;
        DrawBall;
        UpdateText;
        DrawLives;
        ClearKeyPresses;
        StartOnTimeOrPress(120);
     end;
end;

{ DrawBall - draws the ball tile at the current ball location.  The
             location corresponds to the center of the ball, not the
             top left!
}
procedure DrawBall;
begin
     tg_tile5((g_BallX - 250) div 100, (g_BallY - 250) div 100, BALL_TILE);
end;

{ ProcessBlockCollision - Checks to see if a block has been hit enough to
                          break.  If so, marks the block as gone and makes
                          sure the background gets redrawn over the now
                          missing block.
}
procedure ProcessBlockCollision(offset: Integer);
var
   baseScore: Integer;
   hMultiplier: Integer;
begin

   { Blocks marked with a hit count of $DD are actually unbreakable.  If
     one is hit, don't do anything here. }
   if (g_HitsRemaining[offset] <> $DD) then
   begin
      { Blocks are actually 2 tiles wide.  If one is hit, make sure the other
        one is marked as hit too. }
      Dec(g_HitsRemaining[offset], 1);
      if (g_CurrentLevelData[offset] >= 12) then
      begin
         Dec(g_CurrentLevelData[offset], 12);
         AddToDirtyList(offset);
      end;

      if (offset mod 2 = 1) then
      begin
         Dec(g_HitsRemaining[offset + 1], 1);
         if (g_CurrentLevelData[offset + 1] >= 12) then
         begin
            Dec(g_CurrentLevelData[offset + 1], 12);
            AddToDirtyList(offset + 1);
         end;
      end else begin
         Dec(g_HitsRemaining[offset - 1], 1);
         if (g_CurrentLevelData[offset - 1] >= 12) then
         begin
            Dec(g_CurrentLevelData[offset - 1], 12);
            AddToDirtyList(offset - 1);
         end;
      end;

      if (g_HitsRemaining[offset] <= 0) then
      begin
        baseScore := g_BlockScores[g_CurrentLevelData[offset]];
        hMultiplier := 1 + (9 - (offset div 20));
        if (hMultiplier < 1) then
           hMultiplier := 1;
        g_PlayerScore := g_PlayerScore + (baseScore * hMultiplier);
        if (g_PlayerScore > g_HighScore) then
           g_HighScore := g_PlayerScore;
        CheckForExtraLifeAward;
        g_UpdateText := True;
        g_CurrentLevelData[offset] := $ff;
        AddToDirtyList(offset);
        { Be sure to delete both tiles that make up the block. }
        if (offset mod 2 = 1) then
        begin
           g_CurrentLevelData[offset + 1] := $ff;
           AddToDirtyList(offset + 1);
        end else begin
           g_CurrentLevelData[offset - 1] := $ff;
           AddToDirtyList(offset - 1);
        end;
        Dec(g_BlocksRemaining, 1);
        if (g_BlocksRemaining < 1) then begin
           RedrawDirtyBlocks;
           UpdateText;
           Wait(120);
           StartNextLevel;
        end;
      end;
   end;
end;

{ CheckBlockCollision - given the current and previous position of the ball,
                        determine if the ball hit any blocks, and if so,
                        which portion of the ball hit the block,
                        adjusting the ball direction accordingly.
}
procedure CheckBlockCollision(xOld, yOld, xNew, yNew: Integer;
                              var collided: Boolean);
begin

   if (g_CurrentLevelData[yNew * 20  + xNew] <> $FF) then
   begin
        { Check to see if (oldx, newy) has no collision.
          If so, change the x direction. }
        if (g_CurrentLevelData[yNew * 20 + xOld] = $FF) then
        begin
           g_BallXSpeed := (g_BallXSpeed * -1) + random(10);
           if ((g_BallNewX mod 500) = (g_BallX mod 500)) then
           begin
              g_BallNewX := g_BallX + g_BallXSpeed;
           end else
           begin
              g_BallNewX := g_BallX;
           end;
        end;

        { Check to see if (newx, oldy) has no collision.
          If so, change the y direction. }
        if (g_CurrentLevelData[yOld * 20 + xNew] = $FF) then
        begin
           g_BallYSpeed := (g_BallYSpeed * -1);
           if ((g_BallNewY div 500) = (g_BallY div 500)) then
           begin
              g_BallNewY := g_BallY + g_BallYSpeed;
           end else
           begin
              g_BallNewY := g_BallY;
           end;
        end;

        processBlockCollision(yNew * 20 + xNew);
        collided := True;

     end
     else begin
        collided := False;
     end;
end;

{ CheckBlockCollisionAlt - given the current and previous position of the ball,
                        determine if the ball hit any blocks, and if so,
                        which portion of the ball hit the block,
                        adjusting the ball direction accordingly.
}
procedure CheckBlockCollisionAlt(xOld, yOld, xNew, yNew: Integer;
                              var collided: Boolean);
var
  xOldOff, yOldOff, xNewOff, yNewOff: Integer;
begin
   xOldOff := xOld div 500;
   yOldOff := yOld div 500;
   xNewOff := xNew div 500;
   yNewOff := yNew div 500;

   if (g_CurrentLevelData[yNewOff * 20  + xNewOff] <> $FF) then
   begin
        { Check to see if (oldx, newy) has no collision.
          If so, change the x direction. }
        if (g_CurrentLevelData[yNewOff * 20 + xOldOff] = $FF) then
        begin
           g_BallXSpeed := (g_BallXSpeed * -1) + random(10);
           if ((g_BallNewX div 500) = (g_BallX div 500)) then
           begin
              g_BallNewX := g_BallX + g_BallXSpeed;
           end else
           begin
              g_BallNewX := g_BallX;
           end;
        end;

        { Check to see if (newx, oldy) has no collision.
          If so, change the y direction. }
        if (g_CurrentLevelData[yOldOff * 20 + xNewOff] = $FF) then
        begin
           g_BallYSpeed := (g_BallYSpeed * -1);
           if ((g_BallNewY mod 500) = (g_BallY mod 500)) then
           begin
              g_BallNewY := g_BallY + g_BallYSpeed;
           end else
           begin
              g_BallNewY := g_BallY;
           end;
        end;

        processBlockCollision(yNewOff * 20 + xNewOff);
        collided := True;

     end
     else begin
        collided := False;
     end;
end;

{ ProcessPaddleCollision - adjust the speed and direction of the ball based
                           on how it hit the paddle.
}
procedure ProcessPaddleCollision(hitX, hitY: Integer);
var
    paddleHalfSize, distanceFromCenter: Integer;
    offset: Integer;
    paddleIsMoving: Boolean;
    wasPositive: Boolean;
begin

    paddleIsMoving := False;
    if (g_ExtendedKeyCodes[DIR_LEFT] = KEY_DOWN) or
       (g_ExtendedKeyCodes[DIR_RIGHT] = KEY_DOWN) then
       paddleIsMoving := True;

    if (g_BallXSpeed < 0) then
       wasPositive := False;
    if (g_BallXSpeed >= 0) then
       wasPositive := True;

    { Increase the vertical speed every time the ball hits the paddle,
      but clamp at 1.25 pixels per update. }
    g_BallYSpeed := g_BallYSpeed * -1;
    if (g_BallYSpeed < 0) then
       Dec(g_BallYSpeed, 5);
    if (g_BallYSpeed > 0) then
       Inc(g_BallYSpeed, 5);
    if (g_BallYSpeed < -125) then
       g_BallYSpeed := -125;
    if (g_BallYSpeed > 125) then
       g_BallYSpeed := 125;

    { Find out how far half the paddle size is, then find out how close to
      the center the ball actually hit }
    paddleHalfSize := g_PaddleWidth div 2;
    distanceFromCenter := abs(hitX - (g_PaddleX + paddleHalfSize));
    offset := distanceFromCenter div 100;
    if (offset > 9) then offset := 9;

    { Look up the appropriate ball speed for where the ball hit the paddle.
      If the ball hit the left half of the paddle, bounce it left, otherwise
      bounce it right...}
    g_BallXSpeed := g_SpeedsNormal[offset];
    if (hitX < (g_PaddleX + paddleHalfSize)) then
       g_BallXSpeed := g_BallXSpeed * -1;

    { ... EXCEPTION!  If the paddle isn't moving at the time of collision,
      make the ball travel in the direction it was originally traveling (in
      other words, act like the paddle hit a wall or a brick). }
    if (paddleIsMoving = False) then
    begin
       if (wasPositive = True) then begin
          g_BallXSpeed := abs(g_BallXSpeed);
       end else begin
          g_BallXSpeed := -1 * abs(g_BallXSpeed);
       end;
    end;
end;

procedure AddBallToDirtyList;
var
    baseX, baseY, modX, modY: Integer;
begin
    { Get the offset on the screen of the ball's current position, and
      add any background tiles behind the ball to the dirty list.
      Depending on the ball's position, there may be as many as 4 tiles
      that need to be redrawn. }
    baseX := (g_BallX - 250) div 500;
    baseY := (g_BallY - 250) div 500;
    modX := (g_BallX - 250) mod 500;
    modY := (g_BallY - 250) mod 500;

    { If the ball is exactly on a multiple of 5 offset, add the
      one square to the dirty list.  If one of the two axes is on
      a multiple of 5 offset, then add 2 dirty rectangles.  Otherwise,
      add 4. }

    { The ball is exactly aligned over a background tile }
    if (modX = 0) and (modY = 0) then
    begin
        AddToDirtyList((baseY * 20) + baseX);
    end;

    { The ball is aligned over two bg tiles stacked on top of each other. }
    if (modX = 0) and (modY <> 0) then
    begin
        AddToDirtyList((baseY * 20) + baseX);
        AddToDirtyList(((baseY + 1) * 20) + baseX);
    end;

    { The ball is aligned over two bg tiles placed side by side. }
    if (modX <> 0) and (modY = 0) then
    begin
        AddToDirtyList((baseY * 20) + baseX);
        AddToDirtyList((baseY * 20) + baseX + 1);
    end;

    { The ball covers parts of four different bg tiles. }
    if (modX <> 0) and (modY <> 0) then
    begin
        AddToDirtyList((baseY * 20) + baseX);
        AddToDirtyList(((baseY + 1) * 20) + baseX);
        AddToDirtyList(((baseY + 1) * 20) + baseX + 1);
        AddToDirtyList((baseY * 20) + baseX + 1);
    end;
end;

{ MoveBall - Moves the ball in the correct direction and amount, and checks
             to see if the move is legal.  Adjusts ball direction, speed
             and position based on any collisions between the ball and
             paddle/walls/blocks that are detected.
}
procedure MoveBall;
var
   collided: Boolean;
begin
     g_BallNewX := g_BallX + g_BallXSpeed;
     g_BallNewY := g_BallY + g_BallYSpeed;

     collided := False;
     { Check for collisions with all four corners of the ball.  Adjust ball
     trajectory as appropriate.  newBallX and newBallY will contain either
     the new position or the pre-collision position, depending on whether
     there was a collision or not. }
     CheckBlockCollisionAlt(g_BallX, g_BallY - 250,
                            g_BallNewX, g_BallNewY - 250,
                            collided);

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX, g_BallY + 250,
                            g_BallNewX, g_BallNewY + 250,
                            collided);
     end;

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX - 250, g_BallY,
                            g_BallNewX - 250, g_BallNewY,
                            collided);
     end;

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX + 250, g_BallY,
                               g_BallNewX + 250, g_BallNewY,
                               collided);
     end;

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX + 150, g_BallY + 150,
                               g_BallNewX + 150, g_BallNewY + 150,
                               collided);
     end;

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX + 150, g_BallY - 150,
                               g_BallNewX + 150, g_BallNewY - 150,
                               collided);
     end;

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX - 150, g_BallY + 150,
                               g_BallNewX - 150, g_BallNewY + 150,
                               collided);
     end;

     if (collided = False) then
     begin
        CheckBlockCollisionAlt(g_BallX - 150, g_BallY - 150,
                               g_BallNewX - 150, g_BallNewY - 150,
                               collided);
     end;

     { Check for a collision with right wall}
     if ((g_BallNewX + 250) > 9500) then begin
        g_BallNewX := 9200;
        g_BallXSpeed := g_BallXSpeed * -1;
     end;

     { Check for a collision with the left wall }
     if ((g_BallNewX -  250) < 500) then begin
        g_BallNewX := 800;
        g_BallXSpeed := g_BallXSpeed * -1;
     end;

     { Check for a collision with the top wall }
     if ((g_BallNewY - 250) < 500) then begin
        g_BallNewY := 800;
        g_BallYSpeed := g_BallYSpeed * -1;
     end;

     { Collision with bottom }
     if (g_BallNewY > 10000) then
     begin
        AddBallToDirtyList;
        RedrawDirtyBlocks;
        DrawPaddle;
        AddPaddlePosToDirtyList;
        Wait(60);
        InitPositions;
        Dec(g_PlayerLives, 1);
        RedrawDirtyBlocks;
        DrawPaddle;
        DrawBall;
        DrawLives;
        ClearKeyPresses;
        if(g_PlayerLives <= 0) then
        begin
           g_ExitProgram := True;
        end else
        begin
           StartOnTimeOrPress(120);
        end;
        g_BallNewX := g_BallX;
        g_BallNewY := g_BallY;
     end else
     begin
     { Paddle collision }
        if ((g_BallNewY + 250) > 9500) and ((g_BallNewY + 250) < 9800) and
           (g_BallNewX >= (g_PaddleX - 250)) and
           (g_BallNewX <= (g_PaddleX + g_PaddleWidth + 250)) and
           (g_BallYSpeed > 0) then
        begin
           ProcessPaddleCollision(g_BallNewX, g_BallNewY);
           g_BallNewY := g_BallY;
           g_BallNewX := g_BallX;
        end;
     end;
     AddBallToDirtyList;

     g_BallX := g_BallNewX;
     g_BallY := g_BallNewY;
end;

procedure InitPositions;
begin
   g_PaddleX := 4000;
   g_PaddleY := 9500;
   g_BallX := 5000;
   g_BallY := 9300;
   g_BallNewX := g_BallX;
   g_BallNewY := g_BallY;
   g_BallXSpeed := 50;
   g_BallYSpeed := -75;
   g_NumMiddleSections := 2;
   g_PaddleWidth := (2 + g_NumMiddleSections) * 500;
end;

procedure Wait(ticks: Integer);
begin
   repeat
      if userTimerExpired(g_TimerInterval) then
         Dec(ticks);
   until ticks <= 0;
end;

procedure StartOnTimeOrPress(ticks: Integer);
var
   paddleMoved: Boolean;
   paddleMovedLeft: Boolean;
begin
   paddleMovedLeft := False;
   
   repeat
      if userTimerExpired(g_TimerInterval) then
      begin
         Dec(ticks);
         { Allow the paddle to move as normal.  Move the ball along with
           the paddle if it's moved. }
         if (g_ExtendedKeyCodes[DIR_LEFT] = KEY_DOWN) then
         begin
            ProcessPaddleMovement(DIR_LEFT, paddleMoved);
            if (paddleMoved = True) then
            begin
               paddleMovedLeft := True;
               AddBallToDirtyList;
               RedrawDirtyBlocks;
               Dec(g_BallX, PADDLE_SPEED);
               Dec(g_BallNewX, PADDLE_SPEED);
               DrawBall;
               DrawPaddle;
            end;
         end;
         if (g_ExtendedKeyCodes[DIR_RIGHT] = KEY_DOWN) then
         begin
            ProcessPaddleMovement(DIR_RIGHT, paddleMoved);
            if (paddleMoved = True) then
            begin
               paddleMovedLeft := False;
               AddBallToDirtyList;
               RedrawDirtyBlocks;
               Inc(g_BallX, PADDLE_SPEED);
               Inc(g_BallNewX, PADDLE_SPEED);
               DrawBall;
               DrawPaddle
            end;
         end;
      end;

      if (g_KeyScanCodes[57] = KEY_PRESSED) then
      begin
         g_KeyScanCodes[57] := KEY_CLEAR;
         ticks := 0;
      end;
      if (g_KeyScanCodes[1] = KEY_DOWN) then
      begin
         g_KeyScanCodes[1] := KEY_CLEAR;
         ticks :=0;
         g_ExitProgram := True;
      end;
   until ticks <= 0;
   
   if (paddleMovedLeft = True) then
   begin
     g_BallXSpeed := g_BallXSpeed * -1;
   end;
end;

{ MAIN }
begin
     { Replace the default DOS keyboard handler with a custom one. }
     GetIntVec($09, g_DefKBInt);
     SetIntVec($09, @KBInt);

     { Replace the default DOS timer handler with a custom one too. }
     startTimer;
     g_TimerInterval := getUserClockInterval(60);

     { Set some initial values.  Remember that paddle and ball positions/
       speeds are scaled by a factor of 100 to help eliminate floating
       point math. }
     g_ExitProgram := False;
     g_CurrentLevel := 1;
     g_PlayerScore := 0;
     g_PlayerLives := 30;
     g_HighScore := 100000;
     g_OneUpTarget := 50000;
     g_UpdateText := False;
     g_GameTiles.init('DAMANOID');
     tileSource := g_GameTiles.dataStart;
     new(g_font, init('FONTS'));

     { Empty out the dirty block list }
     fillChar(g_DirtyBlocks, 400 * sizeof(Integer), $00);
     g_NumDirtyBlocks := 0;

     { Turn on the 160x100 graphics mode. }
     tg_init;

     LoadScoreValues(g_Status);
     LoadBoard(g_Status);
     LoadLevel(g_Status);
     InitPositions;
     DrawBaseComponents;
     DrawPaddle;
     DrawBall;
     UpdateText;
     DrawLives;

     StartOnTimeOrPress(120);

     { Game loop. }
     while (g_ExitProgram = False) do
     begin
        if (userTimerExpired(g_TimerInterval)) then
        begin
           ProcessInput;
           MoveBall;
           RedrawDirtyBlocks;
           DrawPaddle;
           DrawBall;
           if (g_UpdateText = True) then
           begin
              UpdateText;
              g_UpdateText := False;
           end;
        end;
     end;

     { Turn back on standard text mode. }
     tg_term;

     { Replace the original interrupt handlers. }
     killTimer;
     SetIntVec($09, g_DefKBInt);
end.