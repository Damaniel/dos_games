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
  jfunc,    { 3rd party }
  timer,    { 3rd party }
  txtgraph, { 3rd party }
  globals,
  input,
  levelops,
  renderer,
  title,
  undo,
  utils;

{ Forward declarations. }
procedure gameInit; forward;
procedure gameCleanup; forward;

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
  g_redrawAll := True;  
  g_ExitProgram := False;
  g_redrawLevelDialog := False;

  { Reset the level and score counter. }
  g_currentLevel := 1;
  g_score := 0;
end;

{ Stuff to do right before the game shuts down. }
procedure gameCleanup;
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
      checkWinState;
      render;
    end;  
  end;

  gameCleanup;

end.