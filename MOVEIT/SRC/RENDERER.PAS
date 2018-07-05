{ renderer -  functions related to drawing stuff on the screen.  

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
unit renderer;

interface

procedure addDirtyBlock(x: Integer; y: Integer; item: Integer);
procedure drawBackground(x1: Integer; y1: Integer; x2: Integer; y2: Integer);
procedure drawCreditsScreen;
procedure drawEndGameDialog;
procedure drawInGameText(drawLevel: Boolean; drawScore: Boolean; drawMoves: Boolean; drawTime: Boolean);
procedure drawLevel;
procedure drawLevelCompleteDialog;
procedure drawLevelDialog;
procedure drawRevertDialog;
procedure drawUi;
procedure render;
procedure redrawDirtyLevelArea;

implementation

uses globals,
     jfunc,
     levelops,
     timer,
     title,
     txtgraph,
     utils;

{ Add an item (position, item ID) to the dirty block list. }
procedure addDirtyBlock(x: Integer; y: Integer; item: Integer);
begin
  g_dirtyBlocks[g_numDirtyBlocks].x := x;
  g_dirtyBlocks[g_numDirtyBlocks].y := y;
  g_dirtyBlocks[g_numDirtyBlocks].item := item;
  g_numDirtyBlocks := g_numDirtyBlocks + 1;
end;

procedure drawInGameText(drawLevel: Boolean; drawScore: Boolean; drawMoves: Boolean; drawTime: Boolean);
var
  renderString, minString, secString, timeString: String;
  minutes, seconds: Integer;
begin

  if (drawLevel = True) then
  begin
    { Draw the score and move text }
    Str(g_currentLevel, renderString);
    tg_bar(LEVEL_VAL_X, LEVEL_VAL_Y, LEVEL_VAL_X+15, LEVEL_VAL_Y+4, c_black);
    g_font^.outText(LEVEL_STRING_X, LEVEL_STRING_Y, 'LEVEL:', c_cyan);
    g_font^.outTextCentered(LEVEL_VAL_X, LEVEL_VAL_Y, renderString, c_white);
  end;

  if (drawScore = True) then
  begin
    tg_bar(SCORE_VAL_X-20, SCORE_VAL_Y, SCORE_VAL_X+20, SCORE_VAL_Y+4, c_black);  
    g_font^.outText(SCORE_STRING_X, SCORE_STRING_Y, 'SCORE:', c_cyan);
    g_font^.outTextCentered(SCORE_VAL_X, SCORE_VAL_Y, longtost8(g_score), c_white);
  end;

  if (drawMoves = True) then
  begin
    tg_bar(MOVES_VAL_X-20, MOVES_VAL_Y, MOVES_VAL_X+20, MOVES_VAL_Y+4, c_black);
    g_font^.outText(MOVES_STRING_X, MOVES_STRING_Y, 'MOVES MADE:', c_cyan);
    g_font^.outTextCentered(MOVES_VAL_X, MOVES_VAL_Y, longtost8(g_numMoves), c_white);
  end;

  if (drawTime = True) then
  begin
    g_font^.outText(TIME_STRING_X, TIME_STRING_Y, 'TIME:', c_cyan);
    secondsToHourMinSecString(g_elapsedTime, timeString);
    tg_bar(TIME_VAL_X-15, TIME_VAL_Y, TIME_VAL_X+15, TIME_VAL_Y+4, c_black);
    g_font^.outTextCentered(TIME_VAL_X, TIME_VAL_Y, timeString, c_white);
  end;
end;

{ Draws the list of dirty tiles; areas of the screen that changed since the last
  screen draw.  This is used to minimize the amount of drawing, keeping the
  updates relatively fast even for slow PCs. }
procedure redrawDirtyLevelArea;
var
  i, item: Integer;
begin
  if (g_numDirtyBlocks > 0) then
  begin
    for i := 0 to g_numDirtyBlocks - 1 do
    begin  
      tg_tile5(g_levelStartX + g_dirtyBlocks[i].x * LARGE_TILE_SIZE, 
               g_levelStartY + g_dirtyBlocks[i].y * LARGE_TILE_SIZE,
               g_dirtyBlocks[i].item);       
    end;  
    g_numDirtyBlocks := 0;
  end;
end;

{ TODO: remove magic numbers }
procedure drawRevertDialog;
begin
  tg_bar(0, 23, 99, 75, c_black);

  tg_bar(16, 31, 83, 68, c_blue);
  tg_rectangle(14, 29, 85, 70, c_white);
  tg_bar(15,30,84,30, c_ltgrey);
  tg_bar(15,31,15,69, c_ltgrey);
  tg_bar(84,31,84,69, c_ltblue);
  tg_bar(16,69,83,69, c_ltblue);

  g_font^.outTextCentered(50, 33, 'RESET LEVEL?', c_white);
  g_font^.outTextCentered(50, 44, '(LEVEL PROGRESS', c_white);
  g_font^.outTextCentered(50, 51, 'WILL BE LOST! )', c_white);
  g_font^.outTextCentered(50, 62, '(Y)ES OR (N)O?', c_white);
end;

{ TODO: remove magic numbers }
procedure drawEndGameDialog;
begin
  tg_bar(0, 23, 99, 75, c_black);

  tg_bar(16, 31, 83, 68, c_blue);
  tg_rectangle(14, 29, 85, 70, c_white);
  tg_bar(15,30,84,30, c_ltgrey);
  tg_bar(15,31,15,69, c_ltgrey);
  tg_bar(84,31,84,69, c_ltblue);
  tg_bar(16,69,83,69, c_ltblue);

  g_font^.outTextCentered(50, 33, 'EXIT GAME?', c_white);
  g_font^.outTextCentered(50, 44, '(LEVEL PROGRESS', c_white);
  g_font^.outTextCentered(50, 51, 'WILL BE LOST! )', c_white);
  g_font^.outTextCentered(50, 62, '(Y)ES OR (N)O?', c_white);
end;

{ Does a *full* draw of the level area, as opposed to just dirty blocks.  Gets all of
  the level contents on the screen, but can cause noticeable flicker on very slow PCs.
  Mainly used to draw the level initially; dirty blocks are used afterward to only
  draw the changes. }
procedure drawLevel;
var 
  startX, startY: Integer;
  i, j, item: Integer;
begin 
  
  { draw the walls and floors }
  for i := 0 to g_levelWidth - 1 do
  begin
    for j := 0 to g_levelHeight - 1 do
    begin
      getStageItem(i, j, item);      
      if (item = LEVEL_WALL) then
      begin
        tg_tile5(g_levelStartX + i * LARGE_TILE_SIZE, g_levelStartY + j * LARGE_TILE_SIZE, ID_WALL);       
      end;
      if (item = LEVEL_FLOOR) then
      begin
        tg_tile5(g_levelStartX + i * LARGE_TILE_SIZE, g_levelStartY + j * LARGE_TILE_SIZE, ID_FLOOR);             
      end;      
    end;
  end;
  
  { Draw the target tiles }
  for i := 0 to g_numTargets - 1 do
  begin
    tg_tile5(g_levelStartX + g_targetLocations[i].x * LARGE_TILE_SIZE, 
             g_levelStartY + g_targetLocations[i].y * LARGE_TILE_SIZE, 
             ID_TARGET);
  end;
  
  { Draw the box tiles }
  for i := 0 to g_numBoxes - 1 do
  begin
      tg_tile5(g_levelStartX + g_boxLocations[i].x * LARGE_TILE_SIZE, 
             g_levelStartY + g_boxLocations[i].y * LARGE_TILE_SIZE, 
             ID_BOX);
  end;
  
  { Draw the player. }
  tg_tile5(g_levelStartX + g_playerX * LARGE_TILE_SIZE, 
           g_levelStartY + g_playerY * LARGE_TILE_SIZE, 
           ID_PLAYER);

end;

{ Draws the dialog showing the level number, par time and move target }
{ TODO: Remove magic numbers }
procedure drawLevelDialog;
var
  tmpString, tmpString2: String;
  timeString: String;
begin
  tg_bar(0, 23, 99, 75, c_black);

  tg_bar(16, 29, 83, 70, c_blue);
  tg_rectangle(14, 27, 85, 72, c_white);
  tg_bar(15,28,84,28, c_ltgrey);
  tg_bar(15,29,15,71, c_ltgrey);
  tg_bar(84,29,84,71, c_ltblue);
  tg_bar(16,71,83,71, c_ltblue);
  
  Str(g_currentLevel, tmpString);
  tmpString2 := Concat('LEVEL ', tmpString);
  g_font^.outTextCentered(50, 34, tmpString2, c_white);

  g_font^.outText(29, 44, 'PAR:', c_white);
  secondsToHourMinSecString(g_parTime, timeString);
  g_font^.outText(49, 44, timeString, c_yellow);
  
  g_font^.outText(29, 51, 'TARGET:', c_white);
  Str(g_targetMoves, tmpString);
  g_font^.outText(59, 51, tmpString, c_yellow);
  g_font^.outText(27, 61, 'PRESS ENTER!', c_white);

  tg_tile5(18, 61, ID_BOX);
  tg_tile5(77, 61, ID_BOX);
end;

{ Draws the dialog showing the score for the level }
{ TODO: Remove magic numbers }
procedure drawLevelCompleteDialog;
var
  tmpString, tmpString2, tmpString3: String;
  timeString, moveString: String;
  timeMultString, moveMultString: String;
  totalTimeString, totalMoveString: String;
  passwordString: String;
  timeBase, moveBase: Integer;
  timeScore, moveScore: Longint;
begin

  calculateScore(timeBase, moveBase, timeScore, moveScore);

  tg_bar(0, 23, 99, 75, c_black);
  tg_bar(8, 27, 89, 71, c_blue);
  tg_rectangle(6, 25, 91, 73, c_white);
  tg_bar(7, 26, 90, 26, c_ltgrey);
  tg_bar(7, 27, 7, 72, c_ltgrey);
  tg_bar(90, 27, 90, 72, c_ltblue);
  tg_bar(8, 72, 89, 72, c_ltblue);
  
  g_font^.outTextCentered(48, 29, 'LEVEL COMPLETE!', c_white);
  g_font^.outTextCentered(48, 38, 'BONUS:', c_yellow);

  g_font^.outText(12, 47, 'TIME:', c_ltGreen);
  g_font^.outText(12, 56, 'MOVES:', c_ltGreen);

  { put the score numbers together }
  Str(timeBase, timeString);
  Str(moveBase, moveString);
  Str(POINTS_PER_SECOND, timeMultString);
  Str(POINTS_PER_MOVE, moveMultString);
  Str(timeScore, totalTimeString);
  Str(moveScore, totalMoveString);
  
  tmpString := Concat(timeString, 'x', timeMultString, '=', totalTimeString);
  tmpString2 := Concat(moveString, 'x', moveMultString, '=', totalMoveString);

  g_font^.outTextCentered(61, 47, tmpString, c_white);
  g_font^.outTextCentered(61, 56, tmpString2, c_white);
  
  g_font^.outText(27, 65, 'PRESS ENTER!', c_white);
  tg_tile5(18, 65, ID_BOX);
  tg_tile5(77, 65, ID_BOX);  

end;

procedure drawBackground(x1: Integer; y1: Integer; x2: Integer; y2: Integer);
var
  i, j: Integer;
  tileId: Integer;
begin
  { draw background squares The background consists of a 2x2 grid of tiles containing 
    4 different tile types, allowing seamless tiling across the background. }
  for i := x1 to x2 do
  begin
    for j := y1 to y2 do
    begin
      case (j mod 2) of
        0: begin 
             case (i mod 2) of
               0: tileId := ID_BACKGROUND1;
               1: tileId := ID_BACKGROUND2;
             end;
           end; 
        1: begin
             case (i mod 2) of
               0: tileId := ID_BACKGROUND3;
               1: tileId := ID_BACKGROUND4;
             end;
           end;
      end;
      tg_tile5(i * LARGE_TILE_SIZE, j * LARGE_TILE_SIZE, tileId);
    end;
  end;
end;

{ UI rendering function.  Draws all parts of the screen that aren't the play area. }
procedure drawUi;
var
  i, j: Integer;
begin  
  { Draw the stuff on the right side }
  tg_bar(102,2,157,97,c_blue);
  tg_rectangle(100, 0, 159, 99, c_white);
  tg_bar(101,1,158,1, c_ltgrey);
  tg_bar(101,2,101,98, c_ltgrey);
  tg_bar(158,2,158,98, c_ltblue);
  tg_bar(102,98, 157,98, c_ltblue);

  { Draw the logo in the upper corner. }
  for i:= 0 to 3 do
  begin
    for j := 0 to 9 do
    begin
      tg_tile5(105 + j*5, 5 + i*5, g_gameLogo[i*10+j]);
    end;
  end;

  { Draw the black boxes }
  tg_bar(BLACK_BAR_X1, LEVEL_VAL_Y-1, BLACK_BAR_X2, LEVEL_VAL_Y+5, c_black);
  tg_bar(BLACK_BAR_X1, SCORE_VAL_Y-1, BLACK_BAR_X2, SCORE_VAL_Y+5, c_black);
  tg_bar(BLACK_BAR_X1, MOVES_VAL_Y-1, BLACK_BAR_X2, MOVES_VAL_Y+5, c_black);
  tg_bar(BLACK_BAR_X1, TIME_VAL_Y-1,  BLACK_BAR_X2, TIME_VAL_Y+5,  c_black);

  drawInGameText(True, True, True, True);
end;

procedure drawMainMenuOptions;
var 
  levString: String;
  timeString: String;  
  leftString: String;
begin
  if(g_mainMenuOption = OPTION_NEW_GAME) then
  begin
    g_font^.outText(32, 61, '>', c_yellow);
    g_font^.outText(74, 61, '<', c_yellow);
    g_font^.outText(85, 61, '>', c_black);
    g_font^.outText(125, 61, '<', c_black);
    tg_bar(32, 69, 127, 77, c_blue);
    tg_bar(31, 69, 31, 78, c_white);
    tg_bar(128, 69, 128, 78, c_white);
    tg_bar(32, 78, 127, 78, c_white);
    g_font^.outTextCentered(80, 71, 'START FROM LEVEL 1', c_white);
  end;
  if(g_mainMenuOption = OPTION_CONTINUE) then
  begin
    g_font^.outText(85, 61, '>', c_yellow);
    g_font^.outText(125, 61, '<', c_yellow);
    g_font^.outText(32, 61, '>', c_black);
    g_font^.outText(74, 61, '<', c_black);
    tg_bar(32, 69, 127, 77, c_green);
    tg_bar(31, 69, 31, 78, c_white);
    tg_bar(128, 69, 128, 78, c_white);
    tg_bar(32, 78, 127, 78, c_white);
    Str(g_currentLevel, levString);
    secondsToHourMinSecString(g_totalElapsedTime, timeString);
    leftString := Concat('LEVEL ', levString);
    g_font^.outTextCentered(57, 71, leftString , c_white);    
    g_font^.outTextCentered(101, 71, timeString , c_white);
  end;
end;

{ Draws the main menu.  Could stand to use optimization}
procedure drawMainMenu;
begin
  tg_bar(28, 59, 133, 69, c_black);
  tg_bar(27, 58, 132, 58, c_white);
  tg_bar(27, 68, 132, 68, c_white);
  tg_bar(27, 59, 27, 67, c_white);
  tg_bar(132, 59, 132, 67, c_white);

  g_font^.outText(37, 61, 'NEW GAME', c_white);
  g_font^.outText(90, 61, 'CONTINUE', c_white);

  drawMainMenuOptions;

end;

procedure drawCreditsScreen;
var
  i: Integer;
  outStr: String;
begin

  tg_clear(0);
  
  { Draw the background }
  drawBackground(0, 0, 31, 19);

  { Draw the boxes and borders }
  tg_bar(15, 8, 144, 40, c_black);
  tg_bar(15, 46, 144, 77, c_black);
  tg_bar(15, 83, 144, 91, c_black);

  { Top box border }
  tg_bar(14, 7, 145, 7, c_white);
  tg_bar(14, 8, 14, 41, c_white);
  tg_bar(15, 41, 145, 41, c_white);
  tg_bar(145, 8, 145, 40, c_white);
 
  { Middle box border }
  tg_bar(14, 45, 145, 45, c_white);
  tg_bar(14, 46, 14, 78, c_white);
  tg_bar(15, 78, 145, 78, c_white);
  tg_bar(145, 46, 145, 77, c_white);

  { Bottom box border }
  tg_bar(14, 82, 145, 82, c_white);
  tg_bar(14, 83, 14, 92, c_white);
  tg_bar(15, 92, 145, 92, c_white);
  tg_bar(145, 83, 145, 91, c_white);

  { Edging }
  tg_bar(5, 5, 154, 5, c_black);
  tg_bar(5, 94, 154, 94, c_black);
  tg_bar(5, 6, 5, 93, c_black);
  tg_bar(154, 5, 154, 93, c_black);
  
  { Boxes }
  for i:=0 to 31 do
  begin
    tg_tile5(i * LARGE_TILE_SIZE, 0, ID_BOX);
    tg_tile5(i * LARGE_TILE_SIZE, 95, ID_BOX);
  end;

  for i:=1 to 18 do
  begin
    tg_tile5(0, i * LARGE_TILE_SIZE, ID_BOX);
    tg_tile5(155, i * LARGE_TILE_SIZE, ID_BOX);
  end;

  { Text }
  g_font^.outTextCentered(80, 10, 'WOW - YOU ACTUALLY FINISHED!' , c_white);  
  g_font^.outTextCentered(80, 17, 'THANKS FOR PLAYING!' , c_white);  
  g_font^.outText(38, 27, 'FINAL SCORE:' , c_yellow);  
  g_font^.outText(38, 34, 'PLAYED TIME:' , c_yellow);  
  
  g_font^.outText(90, 27, longtost8(g_score), c_white);
  secondsToHourMinSecString(g_totalElapsedTime, outStr);
  g_font^.outText(90, 34, outStr , c_white);  

  g_font^.outTextCentered(80, 48, 'CREDITS' , c_magenta);  
  g_font^.outText(19, 56, 'EVERYTHING:' , c_yellow);
  g_font^.outText(93, 56, 'SHAUN BRANDT' , c_white);
  g_font^.outTextCentered(80, 64, 'HTTPS://GITHUB.COM/DAMANIEL/' , c_green);
  g_font^.outTextCentered(80, 72, 'WWW.HOLYMEATGOAT.COM' , c_ltblue); 

  g_font^.outTextCentered(80, 85, 'PRESS ENTER!' , c_white); 
end;


{ Code that draws stuff on the screen. }
procedure render;
var
  x, y: Integer;
begin
  case g_currentState of
    STATE_TITLE: begin
      if (g_redrawAll = True) then
      begin
        tg_clear(0);
        { Read the tile data and draw the tiles to the display}
        for y := 0 to 19 do
        begin
          for x := 0 to 31 do
          begin
            tg_tile5(x*5, y*5, g_titleScreen[y*32 + x]);
          end;
        end;
        g_redrawAll := False;    
      end;
    end;
 
    STATE_MAIN_MENU: begin
      if (g_redrawMainMenu = True) then
      begin
        drawMainMenu;
        g_redrawMainMenu := False;
      end;
      if (g_redrawMenuOptions = True) then
      begin
        drawMainMenuOptions;
        g_redrawMenuOptions := False;
      end;
    end;

    STATE_LEVEL_DIALOG: begin
      if( g_redrawLevelDialog = True) then
      begin
        tg_clear(0);
        drawBackground(0, 0, 19, 19);
        drawUi;
        drawLevelDialog;
        g_redrawAll := False;
        g_redrawLevelDialog := False;
      end;
    end;
 
    STATE_INGAME: begin
      g_frameCounter := g_frameCounter + 1;
      if (g_frameCounter >= g_frameTarget) then
      begin
        g_elapsedTime := g_elapsedTime + 1;
        { Set the next frame target, accounting for any extra frames of overflow (to minimize 
          counter inaccuracy). }
        g_frameTarget := g_frameCounter + (FRAME_RATE - (g_frameCounter - g_frameTarget));
        g_drawTime := True;
      end;    
      if (g_redrawAll = True) then
      begin
        drawBackground(0, 4, 19, 16);
        drawLevel;     
        g_redrawAll := False;
      end;
      if (g_redrawDirtyLevelArea = True) then
      begin
        redrawDirtyLevelArea;
        g_redrawDirtyLevelArea := False;
      end;
      if (g_drawLevel = True) or (g_drawScore = True) or (g_drawMoves = True) or (g_drawTime = True) then
      begin
        drawInGameText(g_drawLevel, g_drawScore, g_drawMoves, g_drawTime);
        g_drawLevel := False;
        g_drawScore := False;
        g_drawMoves := False;
        g_drawTime := False;
      end;
    end;
 
    STATE_WIN: begin
      if (g_redrawWinDialog = True) then
      begin
        drawLevelCompleteDialog;
        g_redrawWinDialog := False;
      end;
    end;

    STATE_REVERT: begin
      if (g_redrawRevertDialog = True) then
      begin
        drawRevertDialog;
        g_redrawRevertDialog := False;
      end;
    end;

    STATE_END_GAME: begin
      if (g_redrawEndGameDialog = True) then
      begin
        drawEndGameDialog;
        g_redrawEndGameDialog := False;
      end;
    end;

    STATE_END_CREDITS: begin
      if (g_redrawCreditsScreen = True) then
      begin
        drawCreditsScreen;;
        g_redrawCreditsScreen := False;
      end;
    end;
  end;
end;

end.