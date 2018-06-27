{ levelops -  functions related to loading and manipulating levels.  

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
unit levelops;

interface

procedure getStageItem(x: Integer; y: Integer; var item: Integer);
procedure getStageOffset(x: Integer; y: Integer; var offset: Integer);
procedure isOnTarget(x: Integer; y: Integer; var result: Boolean);
procedure setStageItem(x: Integer; y: Integer; item: Integer);
procedure processMovement(dir: Integer; var moved: Boolean);
procedure processStageFile(fileName: string; stage: Integer; var status: Integer);
procedure levelInit(level: Integer);
procedure writeLevel;

implementation

uses globals, 
     renderer, 
     undo;

{ Checks to see if the specified (x,y) position in the stage grid is
  a target square (as opposed to a standard floor). }
procedure isOnTarget(x: Integer; y: Integer; var result: Boolean);
var
  i: Integer;
begin
  result := False;
  {Scan the list of targets and see if one exists at the current position}
  for i:= 0 to g_numTargets-1 do
  begin
    if (g_targetLocations[i].x =x) and (g_targetLocations[i].y = y) then
    begin
      result := True;
    end
  end;
end;

{ Gets the item (wall, floor, etc) at the given x and y position in the level. }
procedure getStageItem(x: Integer; y: Integer; var item: Integer);
var
  offset: Integer;
begin
  getStageOffset(x, y, offset);
  item := g_levelData[offset];
end;

{ Given an x and y position in a level of size g_levelWidth * g_levelHeight,
  find the appropriate offset in the array that represents it. }
procedure getStageOffset(x: Integer; y: Integer; var offset: Integer);
begin
  offset := (y * g_levelWidth) + x;
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
  g_numBoxesOnTargets := 0;

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
      readln(stageFile, g_levelWidth, g_levelHeight, g_parTime, g_targetMoves);
      
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
                    g_numBoxesOnTargets := g_numBoxesOnTargets + 1;
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
  g_elapsedTime := 0;

  { This code will need to be moved to the end of whatever animations or notifications are done
    before the player gets control. }
  g_frameTarget := g_frameCounter + FRAME_RATE;

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
  isBoxOnTarget: Boolean; { Is this box sitting on a target square?}
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
  for i := 0 to g_numBoxes-1 do
  begin
    if (g_boxLocations[i].x = targetX) and (g_boxLocations[i].y = targetY) then
    begin
      isBoxByPlayer := True;
      boxByPlayerIdx := i;  
    end; 
  end;
  
  { Found an adjacent box.  Now find out if there's a box adjacent to *it*. }
  for i := 0 to g_numBoxes-1 do
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
    getStageItem(adjX, adjY, item);
    if(item = LEVEL_FLOOR) then
    begin
      { The target space is a box, and no box is adjacent, so the box will move. }
      isOnTarget(targetX, targetY, isBoxOnTarget);
      { If the box is already on a target, reduce the 'box on target' counter. }
      if (isBoxOnTarget = True) then
      begin
        g_numBoxesOnTargets:= g_numBoxesOnTargets - 1;
      end;    
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

      { If the box is now on a target, increment the 'box on target' counter. }
      isOnTarget(adjX, adjY, isBoxOnTarget);
      if (isBoxOnTarget = True) then
      begin
        g_numBoxesOnTargets := g_numBoxesOnTargets + 1;
      end;
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
  end;

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
end.