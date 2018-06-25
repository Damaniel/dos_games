{ MOVEIT - an implementation of the generic 'move boxes onto targets' puzzle game
            that uses the 160x100, 16 color CGA graphics mode.  .

            Uses Jason Knight's CGA (and later!) 160x100 'graphics' mode
            library. It must have taken a lot of work, and since he released
            it into the public domain, why reinvent the wheel?  Go to his
            website and check out Paku Paku, the game that made him write
            this library in the first place!

                    (http://www.deathshadow.com/pakuPaku)

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
program Moveit;
uses
  Dos,                  { for GetIntVec, SetIntVec }
  globals,
  jfunc,
  timer,
  title,
  txtgraph,
  undo;

{ Forward declarations. }
procedure addDirtyBlock(x: Integer; y: Integer; item: Integer); forward;  
procedure addToUndoBuffer(px, py, bId, bx, by, npx, npy, nbx, nby: Integer); forward;
procedure changeState(targetState: Integer); forward;
procedure cleanup; forward;
procedure drawInGameText(drawLevel: Boolean; drawScore: Boolean; drawMoves: Boolean; drawTime: Boolean); forward;
procedure drawLevel; forward;
procedure drawUi; forward;
procedure gameInit; forward;
procedure getStageItem(x: Integer; y: Integer; var item: Integer); forward;
procedure getStageOffset(x: Integer; y: Integer; var offset: Integer); forward;
procedure isOnTarget(x: Integer; y: Integer; var result: Boolean); forward;
procedure levelInit(level: Integer); forward;
procedure processInput; forward;
procedure processMovement(dir: Integer; var moved: Boolean); forward;
procedure processStageFile(fileName: string; stage: Integer; var status: Integer); forward;
procedure redrawDirtyLevelArea; forward;
procedure render; forward;
procedure restoreUndoBuffer; forward;
procedure Wait(ticks: Integer); forward;
procedure writeLevel; forward;

{ Add an item (position, item ID) to the dirty block list. }
procedure addDirtyBlock(x: Integer; y: Integer; item: Integer);
begin
  g_dirtyBlocks[g_numDirtyBlocks].x := x;
  g_dirtyBlocks[g_numDirtyBlocks].y := y;
  g_dirtyBlocks[g_numDirtyBlocks].item := item;
  g_numDirtyBlocks := g_numDirtyBlocks + 1;
end;

{ Checks to see if the specified (x,y) position in the stage grid is
  a target square (as opposed to a standard floor). }
procedure isOnTarget(x: Integer; y: Integer; var result: Boolean);
var
  i: Integer;
begin
  result := False;
  {Scan the list of targets and see if one exists at the current position}
  for i:= 0 to g_numTargets do
  begin
    if (g_targetLocations[i].x =x) and (g_targetLocations[i].y = y) then
    begin
      result := True;
    end
  end;
end;

{ Add a state (old/new positions) to the undo buffer. }
procedure addToUndoBuffer(px, py, bId, bx, by, npx, npy, nbx, nby: Integer);
var
  u: undoitem;
begin
  u.px := px;
  u.py := py;
  u.boxId := bId;
  u.bx := bx;
  u.by := by;
  u.npx := npx;
  u.npy := npy;
  u.nbx := nbx;
  u.nby := nby; 
  g_undoBuffer.addItem(u);
end;

{ Restores a state from the undo buffer. }
procedure restoreUndoBuffer;
var
  isPlayerOnTarget: Boolean;
  isBoxOnTarget: Boolean;
  u: undoitem;
  isEmpty: Boolean;
begin

  g_undoBuffer.checkEmpty(isEmpty);
  if (isEmpty = False) then
  begin
    g_undoBuffer.getEnd(u);

    { Set the player position to the previous position in the undo buffer }
    g_playerX := u.px;
    g_playerY := u.py;
    addDirtyBlock(g_playerX, g_playerY, ID_PLAYER);

    { Add the pre-undo player position to the dirty block list }
    isOnTarget(u.npx, u.npy, isPlayerOnTarget);
    if (isPlayerOnTarget = True) then
    begin
      addDirtyBlock(u.npx, u.npy, ID_TARGET);
    end 
    else begin
      addDirtyBlock(u.npx, u.npy, ID_FLOOR);
    end;

    { If a box was moved, set the box position to the previous position in the undo buffer. }
    if (u.boxId <> UNDO_NO_BOX) then
    begin
      g_boxLocations[u.boxId].x := u.bx;
      g_boxLocations[u.boxId].y := u.by;
      addDirtyBlock(g_boxLocations[u.boxId].x, g_boxLocations[u.boxId].y, ID_BOX);

      { Add the pre-undo box position to the dirty block list }
      isOnTarget(u.nbx, u.nby, isBoxOnTarget);
      if (isBoxOnTarget = True) then
      begin
        addDirtyBlock(u.nbx, u.nby, ID_TARGET);
      end
      else begin
        addDirtyBlock(u.nbx, u.nby, ID_FLOOR);
      end;      
    end;
    
    { Redraw everything that was changed as part of the restore. }
    g_redrawDirtyLevelArea := True;    

    { Increment the move counter - undo is still a move! }
    g_numMoves := g_numMoves + 1;
    g_drawMoves := True;    
  end;
end;

{ Processes potential player movement (and possible box movement).  This consists of:
  - Is the target space empty?  Make move, return True
  - Is the target space a wall?  Don't make move, return False
  - Is the target space a box?
    - Is the space beyond the box empty?  Move box, move player, return True
    - Is the space beyond the box another box or a wall? Don't make move, return False.
    
  - Note: The way that the boxes are stored requires that all boxes be checked to find
    the one (if any) in the path of the player (or in the path of a box being moved
    by a player).  This scan may need to be done twice depending on what is being moved.
    
  - Also note that if the space the player was on was a target location, it needs to be
    added correctly to the dirty list.
}
procedure processMovement(dir: Integer; var moved: Boolean);
var
  isPlayerOnTarget: Boolean; { Is the player standing on a target square? }
  isBoxByPlayer: Boolean;   { True if a box is in the direction of movement }
  isBoxByBox: Boolean;      { True if a box is adjacent to the box in the direction of movement}
  boxByPlayerIdx: Integer;  { The index in the box list of the offending box}
  boxByBoxIdx: Integer;     { The index in the box list of the offending adjacent box}
  targetX, targetY, adjX, adjY: Integer;
  i, j, item: Integer;
  oldPlayerX, oldPlayerY: Integer;
  movedBox: Integer;
  oldBoxX, oldBoxY: Integer;
begin
  targetX := g_playerX;
  targetY := g_playerY;
  adjX := g_playerX;    { These spots are one further away than the place the player is going}
  adjY := g_playerY;
  
  isBoxByPlayer := False;
  isBoxByBox := False;
  isPlayerOnTarget := False;
  
  case dir of
    DIR_UP: begin
              targetY := targetY - 1;
              adjY := targetY - 1;
            end;
    DIR_DOWN: begin
                targetY := targetY + 1;
                adjY := targetY + 1;
              end;
    DIR_LEFT: begin
                targetX := targetX - 1;
                adjX := targetX - 1;
              end;
    DIR_RIGHT: begin
                 targetX := targetX + 1;
                 adjX := targetX + 1;
               end;
  end;
  
  { Scan the list of boxes and see if any are in the target or adjacent locations. }
  for i := 0 to g_numBoxes do
  begin
    if (g_boxLocations[i].x = targetX) and (g_boxLocations[i].y = targetY) then
    begin
      isBoxByPlayer := True;
      boxByPlayerIdx := i;  
    end; 
  end;
  
  { Found an adjacent box.  Now find out if there's a box adjacent to *it*. }
  for i := 0 to g_numBoxes do
  begin
    if (isBoxByPlayer = True) and (g_boxLocations[i].x = adjX) and (g_boxLocations[i].y = adjY) then
    begin
      isBoxByBox := True;
      boxByBoxIdx := i;       
    end;
  end;  
  
  { Check to see if the player is standing on a target }
  isOnTarget(g_playerX, g_playerY, isPlayerOnTarget);
  
  { If the target space is empty, make the move. }
  getStageItem(targetX, targetY, item);
  if (item = LEVEL_WALL) then
  begin
    moved := False;
  end 
  else if (item = LEVEL_FLOOR) and (isBoxByPlayer = False) then
  begin
    addDirtyBlock(targetX, targetY, ID_PLAYER);
    if (isPlayerOnTarget = True) then
    begin
      addDirtyBlock(g_playerX, g_playerY, ID_TARGET);       
    end 
    else begin
      addDirtyBlock(g_playerX, g_playerY, ID_FLOOR);    
    end;
    
    { Prepare the undo buffer }
    oldPlayerX := g_playerX;
    oldPlayerY := g_playerY;
    movedBox := UNDO_NO_BOX;
    oldBoxX := 0;
    oldBoxY := 0;

    { Process the move }
    g_playerX := targetX;
    g_playerY := targetY;
    g_redrawDirtyLevelArea := True;
    moved := true;  

  end 
  else if (isBoxByPlayer = True) and (isBoxByBox = False) then
  begin
    { The target space is a box, and no box is adjacent, so the box will move. }
    getStageItem(adjX, adjY, item);
    if(item = LEVEL_FLOOR) then
    begin
      addDirtyBlock(targetX, targetY, ID_PLAYER);      
      addDirtyBlock(adjX, adjY, ID_BOX);     
      if (isPlayerOnTarget = True) then
      begin
        addDirtyBlock(g_playerX, g_playerY, ID_TARGET); 
      end
      else begin
        addDirtyBlock(g_playerX, g_playerY, ID_FLOOR);      
      end;

      { Prepare the undo buffer }
      oldPlayerX := g_playerX;
      oldPlayerY := g_playerY;
      movedBox := boxByPlayerIdx;
      oldBoxX := g_boxLocations[boxByPlayerIdx].x;
      oldBoxY := g_boxLocations[boxByPlayerIdx].y;

      { Process the move }
      g_playerX := targetX;  
      g_playerY := targetY;
      g_boxLocations[boxByPlayerIdx].x := adjX;
      g_boxLocations[boxByPlayerIdx].y := adjY;
      g_redrawDirtyLevelArea := True;
      moved := true;    
    end
    else begin
      moved := false;
    end;
  end;

  { If a move was made, then add it to the undo buffer. }
  if (moved = True) then
  begin
    { Increment the move counter }
    g_numMoves := g_numMoves + 1;
    g_drawMoves := True;
    { Update the undo buffer list }
    addToUndoBuffer(oldPlayerX, oldPlayerY, movedBox, oldBoxX, oldBoxY, g_playerX, g_playerY, adjX, adjY);
  end
  
end;

{ Get and handle keypresses. }
procedure processInput;
var
  keyRead: Char;
  extended: Boolean;
  status: Integer;
  isSuccess: Boolean;
begin
  { If a key press (normal or extended) is detected, figure out what it
    is and process appropriately. }
  if keypressed then begin
    keyRead := readkey;
    extended := False;
    
    { Detect extended keypress. }
    if (keyRead = #0) then begin
      extended := True;
      keyRead := readkey;
    end;

    case g_currentState of
      STATE_TITLE: begin
        { ESC - exit program }
        if (keyRead = #27) then begin
          g_ExitProgram := True;
        end
        else begin
          { Any other key - go ingame }
          changeState(STATE_INGAME);
        end;
      end;
      STATE_INGAME: begin
        { ESC - exit program }
        if (keyRead = #27) then begin
          changeState(STATE_TITLE);
        end;
        { Up arrow }
        if (keyRead = #72) and (extended = True) then begin
          processMovement(DIR_UP, isSuccess);
        end;

        { Down arrow }
        if (keyRead = #80) and (extended = True) then begin
          processMovement(DIR_DOWN, isSuccess);
        end;

        { Left arrow }
        if (keyRead = #75) and (extended = True) then begin
          processMovement(DIR_LEFT, isSuccess)
        end;

        { Right arrow }
        if (keyRead = #77) and (extended = True) then begin
          processMovement(DIR_RIGHT, isSuccess);
        end;

        { u - undo }
        if (keyRead = 'u') and (extended = False) then begin
          restoreUndoBuffer;
        end;

        { n - debug, load next stage }
        if (keyRead = 'n') and (extended = False) then begin
          if (g_currentLevel < g_numLevels) then 
          begin
            g_currentLevel := g_currentLevel + 1;
            levelInit(g_currentLevel);
          end;
        end;

        { p - debug, load previous stage }
        if (keyRead = 'p') and (extended = False) then begin
          if(g_currentLevel > 1) then
          begin
            g_currentLevel := g_currentLevel - 1;
            levelInit(g_currentLevel);
          end;
        end;
      end;
    end;
  end;
end;
         
{ Wait function.  ticks = the number of frames worth of delay.  For example,
  with a 60 FPS render rate, a value of 60 for ticks will delay one second. }
procedure Wait(ticks: Integer);
begin
  repeat
    if userTimerExpired(g_TimerInterval) then
      Dec(ticks);
  until ticks <= 0;
end;

{ Given an x and y position in a level of size g_levelWidth * g_levelHeight,
  find the appropriate offset in the array that represents it. }
procedure getStageOffset(x: Integer; y: Integer; var offset: Integer);
begin
  offset := (y * g_levelWidth) + x;
end;

{ Gets the item (wall, floor, etc) at the given x and y position in the level. }
procedure getStageItem(x: Integer; y: Integer; var item: Integer);
var
  offset: Integer;
begin
  getStageOffset(x, y, offset);
  item := g_levelData[offset];
end;

{ Sets an item at the given x and y position in the level. }
procedure setStageItem(x: Integer; y: Integer; item: Integer);
var
  offset: Integer;
begin
  getStageOffset(x, y, offset);
  g_levelData[offset] := item;
end;

{ Takes a file of levels and sets the global stage array to the nth level in the file. }
procedure processStageFile(fileName: string; stage: Integer; var status: Integer);
var
  stageFile: Text;
  line: string;
  numLevels, numToSkip: Integer;
  i, j, stageNumber, numLines, stageItem: Integer;
  item: Char;
begin

  g_numBoxes := 0;
  g_numTargets := 0;
  
  assign(stageFile, fileName);
  
  {$I-}
  reset(stageFile);
  {$I+}

  { If the file can't be opened, complain.  Otherwise, process it. }
  if (IOResult <> 0) then
  begin
    writeln('Unable to open file ', fileName , '!');
    status := -1;
  end
  else begin
    readln(stageFile, numLevels);
    g_numLevels := numLevels;
    if (stage > numLevels) or (stage <= 0) then
    begin
      writeln('There is no stage ', stage, ' in this file!');
      status := -1;
    end
    else begin
      { Skip the first n-1 stages of data }
      numToSkip := stage - 1;
      for i:=0 to numToSkip - 1 do
      begin
        readln(stageFile, stageNumber, numLines);
        for j:=0 to numLines - 1 do
        begin
          readln(stageFile, line);
        end;
      end;
      
      { Process this stage of data }
      readln(stageFile, stageNumber, numLines);
      readln(stageFile, g_levelWidth, g_levelHeight);
      
      { Square types:
          'X' - empty space (no level data)
          '#' - wall
          ' ' - floor
          '.' - target location
          '$' - box sitting on the floor
          '@' - player sitting on the floor
          '*' - box sitting on a target location }
      for i := 0 to g_levelHeight - 1 do
      begin
        for j := 0 to g_levelWidth - 1 do
        begin
          read(stageFile, item);
          case item of
            'X' : setStageItem(j, i, LEVEL_EMPTY);
            '#' : setStageItem(j, i, LEVEL_WALL);
            ' ' : setStageItem(j, i, LEVEL_FLOOR);
            '.' : begin 
                    setStageItem(j, i, LEVEL_FLOOR);
                    g_targetLocations[g_numTargets].x := j;
                    g_targetLocations[g_numTargets].y := i;
                    g_numTargets := g_numTargets + 1;
                  end;
            '$' : begin
                    setStageItem(j, i, LEVEL_FLOOR);;
                    g_boxLocations[g_numBoxes].x := j;
                    g_boxLocations[g_numBoxes].y := i;
                    g_numBoxes := g_numBoxes + 1;
                  end;
            '@' : begin
                    setStageItem(j, i, LEVEL_FLOOR);
                    g_playerX := j;
                    g_playerY := i;
                  end;
            '*' : begin
                    setStageItem(j, i, LEVEL_FLOOR);
                    g_boxLocations[g_numBoxes].x := j;
                    g_boxLocations[g_numBoxes].y := i;
                    g_targetLocations[g_numTargets].x := j;
                    g_targetLocations[g_numTargets].y := i;                    
                    g_numTargets := g_numTargets + 1;        
                    g_numBoxes := g_numBoxes + 1;
                  end;
          end;
        end;
        { Clear the newline and carriage return from the file }
        read(stageFile, item);
        read(stageFile, item);
      end;      
    end;
  end;
     
  close(stageFile);
  status := 0;
end;

{ Debug - prints out stage data to the console. }
procedure writeLevel;
var i, j, stageItem: Integer;
begin
  for i := 0 to g_levelHeight - 1 do
  begin
    for j := 0 to g_levelWidth - 1 do
    begin
      getStageItem(j, i, stageItem);
      write(stageItem);
    end;
    writeln('');
  end;
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
    g_font^.outText(LEVEL_VAL_X, LEVEL_VAL_Y, renderString, c_white);
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
    minutes := Trunc(g_elapsedTime / 60.0);
    seconds := g_elapsedTime mod 60;
    Str(minutes, minString);
    Str(seconds, secString);  
    { Add a leading zero for seconds counts from 0-9 }  
    if (seconds < 10) then
    begin
      timeString := Concat(minString, ':0', secString);
    end
    else begin
      timeString := Concat(minString, ':', secString);
    end;
    tg_bar(TIME_VAL_X-15, TIME_VAL_Y, TIME_VAL_X+15, TIME_VAL_Y+4, c_black);
    g_font^.outTextCentered(TIME_VAL_X, TIME_VAL_Y, timeString, c_white);
  end;
end;


{ UI rendering function.  Draws all parts of the screen that aren't the play area. }
procedure drawUi;
var
  i, j: Integer;
  tileId: Integer;
begin
  { draw background squares The background consists of a 2x2 grid of tiles containing 
    4 different tile types, allowing seamless tiling across the background. }
  for i := 0 to 19 do
  begin
    for j := 0 to 19 do
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
  tg_rectangle(100, 0, 159, 99, c_white);

  { Draw the logo in the upper corner. }
  for i:= 0 to 3 do
  begin
    for j := 0 to 9 do
    begin
      tg_tile5(105 + j*5, 5 + i*5, g_gameLogo[i*10+j]);
    end;
  end;

  drawInGameText(True, True, True, True);

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

{ Transition to a new state in the state machine, and perform any state-specific initialization. }
procedure changeState(targetState: Integer);
begin
  g_previousState := g_currentState;
  g_currentState := targetState;

  { Do any state-specific initialization here. }
  case g_currentState of 
    STATE_TITLE: begin
      { Tell the renderer to draw the title screen. }
      g_redrawAll := True;
      g_currentLevel := 1;
    end;
    STATE_INGAME: begin
      levelInit(g_currentLevel);
    end;
  end;
end;

{ One-time initialization for when the game first starts. }
procedure gameInit;
begin
  { Replace the default DOS timer handler with a custom one. }
  startTimer;
  g_TimerInterval := getUserClockInterval(FRAME_RATE);
  g_frameCounter := 0;

  { Set up tiles and font }
  g_GameTiles.init('MOVEIT');
  tileSource := g_GameTiles.dataStart;
  new(g_font, init('FONTS'));

  { Set up the graphics mode }
  tg_init;

  { Initialize the state machine. }
  g_currentState := STATE_TITLE;
  g_previousState := STATE_TITLE;

  { Set input and render flags }
  g_ExitProgram := False;
  g_redrawAll := True;

  { Reset the level and score counter. }
  g_currentLevel := 1;
  g_score := 0;
end;

{ Initialization done once per stage. }
procedure levelInit(level: Integer);
begin
  { Read the stage data. }
  processStageFile('LEVELS.DAT', level, g_status);
  
  { Determine where to draw the upper left corner of the stage }
  g_levelStartX := Trunc(CENTER_OF_PLAY_AREA_X - (LARGE_TILE_SIZE * g_levelWidth / 2));
  g_levelstartY := Trunc(CENTER_OF_PLAY_AREA_Y - (LARGE_TILE_SIZE * g_levelHeight / 2)); 

  { Clear the undo buffer }
  g_undoBuffer.init;  

  { Prep the renderer }
  g_redrawAll := True;
  g_redrawDirtyLevelArea := False;
  g_numDirtyBlocks := 0;  

  { Reset the move counter }
  g_numMoves := 0;

  { Reset the timer }
  g_frameCounter := 0;  
  g_elapsedTime := 0;

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
    STATE_INGAME: begin
      g_frameCounter := g_frameCounter + 1;
      if (g_frameCounter >= FRAME_RATE) then
      begin
        g_elapsedTime := g_elapsedTime + 1;
        g_frameCounter := g_frameCounter mod FRAME_RATE;
        g_drawTime := True;
      end;    
      if (g_redrawAll = True) then
      begin
        tg_clear(0);
        drawUi;
        drawLevel;     
        g_redrawAll := False;
      end;
      if (g_redrawDirtyLevelArea = True) then
      begin
        redrawDirtyLevelArea;
        g_redrawDirtyLevelArea := False;
      end;
      drawInGameText(g_drawLevel, g_drawScore, g_drawMoves, g_drawTime);
      g_drawLevel := False;
      g_drawScore := False;
      g_drawMoves := False;
      g_drawTime := False;
    end;
  end;
end;

{ Stuff to do right before the game shuts down. }
procedure cleanup;
begin
  tg_clear(0);
  tg_term;
  
  { Replace the original interrupt handler. }
  killTimer;
end;

{ MAIN }
begin
  gameInit;

  { Game loop. }
  while (g_ExitProgram = False) do
  begin
    if (userTimerExpired(g_TimerInterval)) then
    begin
      processInput;
      render;
    end;  
  end;

  cleanup;

end.