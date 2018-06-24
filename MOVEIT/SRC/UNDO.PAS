{ undo -  a unit implementing multi-step undo by using a circular buffer of
          game states.  

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
unit undo;

interface

const 
  { The maximum number of undo steps. }
  MAX_BUF_SIZE=50;

  { A move state that didn't involve a box being moved has it's 'moved box ID' value set
    to this. }
  UNDO_NO_BOX = -1;
type

  { A state snapshot suitable for undoing a single move.  Contains the old
    player and box (if applicable) position so they can be redrawn in the correct places,
    and the current player and box positions so they can be erased from the correct
    places. }
  undoitem = record
    px, py: Integer;
    boxId: Integer;
    bx, by: Integer;
    npx, npy: Integer;
    nbx, nby: Integer;    
  end;

  { A circular buffer of undo states.  Keeps the last MAX_BUF_SIZE states, deleting
    the oldest states as new ones come in. }
  UndoBuf = object
    { Fields }
    buf: array[0..MAX_BUF_SIZE] of undoitem;
    startIdx: Integer;
    count: Integer;

    { Methods: }
    { Clears out the buffer.}
    procedure init;
    { Adds an item to the end of the circular buffer.  Will overwrite old values. }
    procedure addItem(item: undoitem);
    { Returns whether the buffer is empty. }
    procedure checkEmpty(var empty: boolean);
    { Gets the last item in the buffer }
    procedure getEnd(var item: undoitem);
  end;

implementation

{ Initializes the buffer. }
procedure UndoBuf.init;
begin
  startIdx := 0;
  count := 0;
end;

{ Returns whether the buffer has items in it. }
procedure UndoBuf.checkEmpty(var empty: boolean);
begin
  if (count = 0) then 
  begin 
    empty := True;
  end
  else begin
    empty := False;
  end;
end;

{ Adds a undo state to the end of the buffer. }
procedure UndoBuf.addItem(item: undoitem);
begin
    buf[(startIdx + count) mod MAX_BUF_SIZE] := item;
    if (count < MAX_BUF_SIZE) then
    begin
      count := count + 1
    end
    else begin
      startIdx := (startIdx + 1) mod MAX_BUF_SIZE;
    end;

end;

{ Grabs the undo state at the end of the buffer, if any are available. }
procedure UndoBuf.getEnd(var item: undoitem);
begin
    if (count > 0) then
    begin 
      item := buf[(startIdx + count - 1) mod MAX_BUF_SIZE];
      count := count - 1;
    end;
end;

end.