program Sokoban;
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
  
  LEVEL_EMPTY  = 0;
  LEVEL_WALL   = 1;
  LEVEL_FLOOR  = 2;
  LEVEL_TARGET = 3;
  LEVEL_BOX    = 4;
  LEVEL_PLAYER = 5;
  LEVEL_BOX_ON_TARGET = 6;
  
  ID_WALL = 0;
  ID_FLOOR = 1;
  ID_TARGET = 2;
  ID_BOX = 3;
  ID_PLAYER = 4;
  
  ID_BACKGROUND1 = 5;
  ID_BACKGROUND2 = 6;
  ID_BACKGROUND3 = 7;
  
  { this is the maximum size of a single level.  The size of sprite used to render
    elements depends on the size of the level.  }
  MAX_WIDTH = 64;
  MAX_HEIGHT = 64;
  
  MAX_BOXES = 512;
  
  CENTER_OF_PLAY_AREA_X = 50;
  CENTER_OF_PLAY_AREA_Y = 50;
  
  SMALL_TILE_SIZE = 3;
  LARGE_TILE_SIZE = 5;

  UNDO_BUFFER_SIZE = 10;
  UNDO_NO_BOX = -1;

type
  location = record
    x, y: Integer;
  end;
    
  dirty = record
    x, y: Integer;
    item: Integer;
  end;
  
  undoitem = record
    px, py: Integer;
    boxId: Integer;
    bx, by: Integer;
    valid: Boolean;
    npx, npy: Integer;
    nbx, nby: Integer;
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
  
  g_status: Integer;
  
  g_boxLocations: array[0..MAX_BOXES] of location;
  g_targetLocations: array[0..MAX_BOXES] of location;
  
  g_numBoxes: Integer;
  g_numTargets: Integer;
  
  g_playerX: Integer;
  g_playerY: Integer;
  
  g_redrawAll: Boolean;
  g_redrawDirtyLevelArea: Boolean;
  
  g_dirtyBlocks: array[0..399] of dirty;
  g_numDirtyBlocks: Integer;

  g_undoBuffer: undoitem;
  
procedure getStageItem(x: Integer; y: Integer; var item: Integer); forward;
procedure writeLevel; forward;

procedure addDirtyBlock(x: Integer; y: Integer; item: Integer);
begin
  g_dirtyBlocks[g_numDirtyBlocks].x := x;
  g_dirtyBlocks[g_numDirtyBlocks].y := y;
  g_dirtyBlocks[g_numDirtyBlocks].item := item;
  g_numDirtyBlocks := g_numDirtyBlocks + 1;
end;

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

procedure addToUndoBuffer(px, py, bId, bx, by, npx, npy, nbx, nby: Integer);
begin
  g_undoBuffer.px := px;
  g_undoBuffer.py := py;
  g_undoBuffer.boxId := bId;
  g_undoBuffer.bx := bx;
  g_undoBuffer.by := by;
  g_undoBuffer.valid := True;
  g_undoBuffer.npx := npx;
  g_undoBuffer.npy := npy;
  g_undoBuffer.nbx := nbx;
  g_undoBuffer.nby := nby
end;

procedure restoreUndoBuffer;
var
  isPlayerOnTarget: Boolean;
  isBoxOnTarget: Boolean;
begin
  if (g_undoBuffer.valid = True) then
  begin
    g_playerX := g_undoBuffer.px;
    g_playerY := g_undoBuffer.py;
    addDirtyBlock(g_playerX, g_playerY, ID_PLAYER);
    { Add the pre-undo player position to the dirty block list }
    isOnTarget(g_undoBuffer.npx, g_undoBuffer.npy, isPlayerOnTarget);
    if (isPlayerOnTarget = True) then
    begin
      addDirtyBlock(g_undoBuffer.npx, g_undoBuffer.npy, ID_TARGET);
    end 
    else begin
      addDirtyBlock(g_undoBuffer.npx, g_undoBuffer.npy, ID_FLOOR);
    end;

    if (g_undoBuffer.boxId <> UNDO_NO_BOX) then
    begin
      g_boxLocations[g_undoBuffer.boxId].x := g_undoBuffer.bx;
      g_boxLocations[g_undoBuffer.boxId].y := g_undoBuffer.by;
      addDirtyBlock(g_boxLocations[g_undoBuffer.boxId].x, g_boxLocations[g_undoBuffer.boxId].y, ID_BOX);
      { Add the pre-undo box position to the dirty block list }
      isOnTarget(g_undoBuffer.nbx, g_undoBuffer.nby, isBoxOnTarget);
      if (isBoxOnTarget = True) then
      begin
        addDirtyBlock(g_undoBuffer.nbx, g_undoBuffer.nby, ID_TARGET);
      end
      else begin
        addDirtyBlock(g_undoBuffer.nbx, g_undoBuffer.nby, ID_FLOOR);
      end;      
    end;
    g_redrawDirtyLevelArea := True;
    g_undoBuffer.valid := False;
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

  if (moved = True) then
  begin
    { Update the undo buffer list }
    addToUndoBuffer(oldPlayerX, oldPlayerY, movedBox, oldBoxX, oldBoxY, g_playerX, g_playerY, adjX, adjY);
  end
  
end;

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
    
    if (keyRead = #0) then begin
      extended := True;
      keyRead := readkey;
    end;

    if (keyRead = #27) then begin
      g_ExitProgram := True;
    end;
    
    if (keyRead = #72) and (extended = True) then begin
      processMovement(DIR_UP, isSuccess);
    end;

    if (keyRead = #80) and (extended = True) then begin
      processMovement(DIR_DOWN, isSuccess);
    end;

    if (keyRead = #75) and (extended = True) then begin
      processMovement(DIR_LEFT, isSuccess)
    end;

    if (keyRead = #77) and (extended = True) then begin
      processMovement(DIR_RIGHT, isSuccess);
    end;

    if (keyRead = 'u') and (extended = False) then begin
      restoreUndoBuffer;
    end;

  end;
end;
         
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

procedure getStageItem(x: Integer; y: Integer; var item: Integer);
var
  offset: Integer;
begin
  getStageOffset(x, y, offset);
  item := g_levelData[offset];
end;

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
    writeln('There are ', numLevels, ' levels in this file.');
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
      writeln('Found level ', stageNumber, ' - there are ', numLines, ' lines in it');
      readln(stageFile, g_levelWidth, g_levelHeight);
      writeln('Size of level is ', g_levelWidth, 'x', g_levelHeight);
      
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
      
      { Try printing out the array to make sure things look OK }
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
  end;
     
  close(stageFile);
  status := 0;
end;

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

procedure drawUi;
var
  i, j: Integer;
  tileId: Integer;
begin
  { draw background squares }
  for i := 0 to 19 do
  begin
    for j := 0 to 19 do
    begin
      case (j mod 3) of
        0: begin 
             case (i mod 3) of
               0: tileId := ID_BACKGROUND1;
               1: tileId := ID_BACKGROUND2;
               2: tileId := ID_BACKGROUND3;
             end;
           end; 
        1: begin
             case (i mod 3) of
               0: tileId := ID_BACKGROUND2;
               1: tileId := ID_BACKGROUND3;
               2: tileId := ID_BACKGROUND1;
             end;
           end;
        2: begin
             case (i mod 3) of
               0: tileId := ID_BACKGROUND3;
               1: tileId := ID_BACKGROUND1;
               2: tileId := ID_BACKGROUND2;
             end;
           end;        
      end;
      {tg_tile5(i * LARGE_TILE_SIZE, j * LARGE_TILE_SIZE, tileId);}
    end;
  end;
  tg_rectangle(100, 0, 159, 99, c_white);
end;

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

procedure drawLevel;
var 
  startX, startY: Integer;
  i, j, item: Integer;
  useSmallTiles: Boolean;
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
  
  tg_tile5(g_levelStartX + g_playerX * LARGE_TILE_SIZE, 
           g_levelStartY + g_playerY * LARGE_TILE_SIZE, 
           ID_PLAYER);
end;

{ MAIN }
begin
  { Replace the default DOS keyboard handler with a custom one. }
  { GetIntVec($09, g_DefKBInt);}
  {getIntVec($09, @KBInt);}

  { Replace the default DOS timer handler with a custom one too. }
  startTimer;
  g_TimerInterval := getUserClockInterval(60);

  g_GameTiles.init('SOKOBAN');
  tileSource := g_GameTiles.dataStart;
  
  processStageFile('LEVELS.DAT', 9, g_status);
     
  g_ExitProgram := False;
  g_redrawAll := True;
  g_redrawDirtyLevelArea := False;
  g_numDirtyBlocks := 0;

  { These two lines need to be re-initialized every time a new stage is loaded }  
  g_levelStartX := Trunc(CENTER_OF_PLAY_AREA_X - (LARGE_TILE_SIZE * g_levelWidth / 2));
  g_levelstartY := Trunc(CENTER_OF_PLAY_AREA_Y - (LARGE_TILE_SIZE * g_levelHeight / 2)); 
  
  tg_init;
  tg_clear(0);
 
  drawUi;
  drawLevel;
  
  { Game loop. }
  while (g_ExitProgram = False) do
  begin
    if (userTimerExpired(g_TimerInterval)) then
    begin
      processInput;
      if (g_redrawAll = True) then
      begin
        drawUi;
        drawLevel;     
        g_redrawAll := False;
      end;
      if (g_redrawDirtyLevelArea = True) then
      begin
        redrawDirtyLevelArea;
        g_redrawDirtyLevelArea := False;
      end;
    end;  
  end;
  
  tg_clear(0);
  tg_term;
  
  { Replace the original interrupt handlers. }
  killTimer;
  {SetIntVec($09, g_DefKBInt);}
end.