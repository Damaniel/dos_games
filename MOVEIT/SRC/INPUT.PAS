{ input -  functions that process keyboard input.

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
unit input;

interface

procedure processInput;

implementation
uses globals, 
     levelops, 
     utils, 
     renderer, 
     undo,
     jfunc;

{ Get and handle keypresses. }
procedure processInput;
var
  keyRead: Char;
  extendedKey: Boolean;
  status: Integer;
  isSuccess: Boolean;
begin
  { If a key press (normal or extended) is detected, figure out what it
    is and process appropriately. }
  if keypressed then begin
    keyRead := readkey;
    extendedKey := False;
    
    { Detect extended keypress. }
    if (keyRead = #0) then begin
      extendedKey := True;
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
          changeState(STATE_MAIN_MENU);
        end;
      end;

      STATE_MAIN_MENU: begin
        { ESC - clear the menu }
        if (keyRead = #27) then begin
           changeState(STATE_TITLE);
        end;
        
        { Left arrow }
        if ((keyRead = #75) or (keyRead = #77)) and (extendedKey = True) then begin
          if (g_mainMenuOption = OPTION_NEW_GAME) then
          begin
            g_mainMenuOption := OPTION_CONTINUE;
            g_redrawMenuOptions := True;
          end else 
          begin
            g_mainMenuOption := OPTION_NEW_GAME;
            g_redrawMenuOptions := True;       
          end;   
        end;

        { Enter - execute the highlighted option }
        if (keyRead = #13) and (extendedKey = False) then begin   
          { If 'New Game' is selected, then make sure that the data loaded from the
            save file is ignored. }
          if (g_mainMenuOption = OPTION_NEW_GAME) then
          begin
            newGameInit;     
          end;
          changeState(STATE_LEVEL_DIALOG);        
        end;
      end;

      STATE_LEVEL_DIALOG: begin
        if (keyRead = #13) and (extendedKey = False) then begin
          changeState(STATE_INGAME);
        end;
      end;      
      
      STATE_INGAME: begin
        { ESC - exit program }
        if (keyRead = #27) then begin
          changeState(STATE_END_GAME);
        end;
        { Up arrow }
        if (keyRead = #72) and (extendedKey = True) then begin
          processMovement(DIR_UP, isSuccess);
        end;

        { Down arrow }
        if (keyRead = #80) and (extendedKey = True) then begin
          processMovement(DIR_DOWN, isSuccess);
        end;

        { Left arrow }
        if (keyRead = #75) and (extendedKey = True) then begin
          processMovement(DIR_LEFT, isSuccess)
        end;

        { Right arrow }
        if (keyRead = #77) and (extendedKey = True) then begin
          processMovement(DIR_RIGHT, isSuccess);
        end;

        { u - undo }
        if (keyRead = 'u') and (extendedKey = False) then begin
          restoreUndoBuffer;
        end;

        { n - debug, load next stage }
        if (keyRead = 'n') and (extendedKey = False) then begin
          if (g_currentLevel < g_numLevels) then 
          begin
            g_currentLevel := g_currentLevel + 1;
          end else
          begin
            g_currentLevel := 1;
          end;
          changeState(STATE_LEVEL_DIALOG);          
        end;

        { r - reset level }
        if (keyRead = 'r') and (extendedKey = False) then begin
            changeState(STATE_REVERT);
        end;

        { p - debug, load previous stage }
        if (keyRead = 'p') and (extendedKey = False) then begin
          if(g_currentLevel > 1) then
          begin
            g_currentLevel := g_currentLevel - 1;
          end else
          begin
            g_currentLevel := g_numLevels;
          end;
          changeState(STATE_LEVEL_DIALOG);          
        end;
      end;
     
      STATE_WIN: begin
        if (keyRead = #13) and (extendedKey = False) then begin      
          changeState(STATE_NEXT_LEVEL);
        end;
      end;

      STATE_REVERT: begin
        if (keyRead = 'y') and (extendedKey = False) then begin        
            levelInit(g_currentLevel);           
            changeState(STATE_INGAME);
        end ;
        if (keyRead = 'n') and (extendedKey = False) then begin
           changeState(STATE_INGAME);
        end;
      end;

      STATE_END_GAME: begin
        if (keyRead = 'y') and (extendedKey = False) then begin
            changeState(STATE_TITLE);
        end ;
        if (keyRead = 'n') and (extendedKey = False) then begin
           changeState(STATE_INGAME);
        end;
      end;

      STATE_END_CREDITS: begin
        if (keyRead = #13) and (extendedKey = False) then begin      
          changeState(STATE_TITLE);
        end;
      end;

    end;
  end;
end;      

end.

