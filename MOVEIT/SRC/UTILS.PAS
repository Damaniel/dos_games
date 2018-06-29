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
procedure secondsToMinSecString(time: Integer; var timeString: String);
procedure Wait(ticks: Integer);

implementation

uses globals,
     levelops,
     timer,
     jfunc; 

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

end.
