{ utils -  miscellaneous utility functions.

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
unit utils;

interface

procedure calculateScore(var timeBase: Integer; var moveBase: Integer; var timeBonus: Longint; var moveBonus: Longint);
procedure changeState(targetState: Integer);
procedure checkWinState;
procedure decodePassword(password: String; var score: Longint; var stage: Longint; var valid: Boolean);
procedure encodePassword(score: Longint; stage: Longint; var password: String);
procedure secondsToMinSecString(time: Integer; var timeString: String);
procedure Wait(ticks: Integer);

implementation

uses globals,
     levelops,
     timer,
     jfunc; 

const
  characters: array[0..15] of Char =('A','B','C','E',
                                      'H','L','P','T',
                                      'Y','Z','3','4',
                                      '6','7','9','0');
  xorVal = $CACAD0D0;

{ Transition to a new state in the state machine, and perform any state-specific initialization. }
procedure changeState(targetState: Integer);
var
  timeBase, moveBase: Integer;
  timeBonus, moveBonus: Longint;
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
    STATE_LEVEL_DIALOG: begin
      g_redrawAll := True;
      g_redrawLevelDialog := True;
      levelInit(g_currentLevel);     { Need to load level here so we can draw the dialog over it. }   
    end;
    STATE_INGAME: begin
      g_redrawAll := True;
    end;
    STATE_WIN: begin
      g_redrawWinDialog := True;
      calculateScore(timeBase, movebase, timeBonus, moveBonus);
      g_score := g_score + timeBonus + moveBonus;
    end;
    STATE_NEXT_LEVEL: begin
      { If past max level, then exit game, otherwise move to the next level. }
      if (g_currentLevel = g_numLevels) then
      begin
        g_ExitProgram := True;
      end
      else begin
        g_currentLevel := g_currentLevel + 1;
        changeState(STATE_LEVEL_DIALOG);
      end;
    end;    
    STATE_REVERT: begin
      g_redrawRevertDialog := True;
    end;
    STATE_END_GAME: begin
      g_redrawEndGameDialog := True;
    end;
  end;
end;

procedure checkWinState;
begin
  if (g_currentState = STATE_INGAME) then
  begin
    { If all boxes are on targets, then change to the win state. }
    if (g_numBoxesOnTargets = g_numBoxes) then
    begin
      changeState(STATE_WIN);
    end;  
  end;
end;

procedure calculateScore(var timeBase:Integer; var moveBase: Integer; var timeBonus: Longint; var moveBonus: Longint);
begin
    if (g_elapsedTime <= g_parTime) then
    begin
      timeBase := (g_parTime - g_elapsedTime);
      timeBonus := timeBase * POINTS_PER_SECOND;
    end else 
    begin
      timeBase := 0;
      timeBonus := 0;
    end;
    if (g_numMoves <= g_targetMoves) then
    begin
      moveBase := (g_targetMoves - g_numMoves);
      moveBonus := moveBase * POINTS_PER_MOVE;
    end else
    begin
      moveBase := 0;
      moveBonus := 0;
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

procedure secondsToMinSecString(time: Integer; var timeString: String);
var
  minutes, seconds: Integer;
  minString, secString: String;
begin
  minutes := Trunc(time / 60.0);
  seconds := time mod 60;
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
end;

{ Password decoding algorithm:
    - Given a 10 character string, do the following:
      - Confirm that each character is a valid one from 'characters'.
      - For each group of 2 (0-1, 2-3, 4-5, 6-7, 8-9)
        - Convert the character back to it's numeric value
        - Assign the first value to the upper 4 bites of a value byte
        - Assign the second value to the lower 4 bits of a value byte
      - Add the first, second, fourth and fifth values together and confirm
        they sum to the checksum (the third value)
      - Stick the first value in the top 8 bits of a longint
      - Stick the second value in the next 8 bits of a longint
      - Stick the fourth value in the next 8 bits of a longint
      - Stick the fifth value in the lowest 8 bits of a longint
      - XOR the result by $CACAD0D0
      - Extract the top 8 bits as the stage
      - Extract the bottom 24 bits as the score
}
procedure decodePassword(password: String; var score: Longint; var stage: Longint; var valid: Boolean);
var
  charVals: array[0..10] of Byte;
  bytes: array[0..5] of Byte;
  i, j: Integer;
  inPassword: Boolean;
  checksum: Byte;
  fullValue, tempValue: Longint;
begin
  valid := True;
  
  if (Length(password) <> 10) then
  begin
    valid := False;
  end;

  { If length check passes, continue. }
  if (valid = True) then
  begin
    for i := 1 to 10 do
    begin
      inPassword := False;
      for j := 0 to 15 do
      begin
        if (password[i] = characters[j]) then
        begin
          inPassword := True;
          charVals[i-1] := j;
        end;
      end;
      if (inPassword = False) then
      begin
        valid := False;
      end;
    end;
  end;

  { If character check passes, continue. }
  if (valid = True) then
  begin
    for i:= 0 to 4 do
    begin
      bytes[i] := charVals[2*i];
      bytes[i] := bytes[i] or ((charVals[2*i+1] and $F) shl 4);
    end;
    checksum := bytes[0] + bytes[1] + bytes[3] + bytes[4];
    if (checksum <> bytes[2]) then
    begin
      valid := False;
    end;
  end;

  { If checksum check passes, continue. }
  if (valid = True) then
  begin    
    fullValue := bytes[4];
    tempValue := bytes[3];
    tempValue := tempValue shl 8;
    fullValue := fullValue or tempValue;
    
    tempValue := bytes[1];
    tempValue := tempValue shl 16;
    fullValue := fullValue or tempValue;
    
    tempValue := bytes[0];
    tempValue := tempValue shl 24;
    fullValue := fullValue or tempValue;

    fullValue := fullValue xor xorVal;

    stage := (fullValue shr 24) and $FF;
    score := fullValue and $FFFFFF;
  end;
    
end;

{ Password generation algorithm:
  Given a stage number from 0-255 and a score from 0-2^24:
    - Take a 4 byte integer
    - Set the top 8 bits to the value of the stage
    - Set the bottom 24 bits to the value of the score
    - XOR the result by $CACAD0D0
    - Cut off each of the 4 bytes into their own values
    - Calculate a 5th checksum byte as the sum of the other 4 bytes
    - In the following order - byte 1, byte 2, checksum, byte 3, byte 4:
      - Extract the top 4 bytes, get the character in 'characters'
         associated with it
      - Extract the bottom 4 bytes, get the character in 'characters'
         associated with it
      - Append the two characters to the password string

    - The result should be a 10 character string.
}
procedure encodePassword(score: Longint; stage: Longint; var password: String);
var
  temp: Longint;
  bytes: array[0..5] of Byte;
  topHalf, bottomHalf: Byte;
  tempString: String;
  i, j: Integer;
begin

  writeln('Score is ', score);
  writeln('Stage is ', stage);

  stage := stage shl 24;
  score := score or stage;
  temp  := score xor xorVal;

  bytes[0] := (temp shr 24) and $FF;
  bytes[1] := (temp shr 16) and $FF;
  bytes[2] := 0;
  bytes[3] := (temp shr 8) and $FF;
  bytes[4] := temp and $FF;
  
  bytes[2] := bytes[0] + bytes[1] + bytes[3] + bytes[4];

  for i := 0 to 4 do
  begin
    topHalf := (bytes[i] shr 4) and $F;
    bottomHalf := bytes[i] and $F;
    password := Concat(password,
                       characters[bottomHalf],
                       characters[topHalf]);
  end;
end;

end.
