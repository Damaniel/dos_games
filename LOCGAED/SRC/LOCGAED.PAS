{  LoCGAEd - a sprite editor for CGA low resolution (160x100) graphics mode.
             The editor can save strips of sprite tiles to a format similar,
             but not identical to the format used by Paku Paku - the color
             and transparency data is stored in the same manner, but there
             are a few bytes of header information to support multiple sprite
             sizes.  To use these files directly with txtgraph.pas, strip the
             first 12 bytes of the file beforehand.
             
             No mouse support (currently) - everything is keyboard
             controlled.  Press F1 to get a list of commands from the
             program.

             Most of this code was written in October 2014 by
             Shaun Brandt / Holy Meatgoat Software.

             Uses Jason Knight's CGA (and later!) 160x100 'graphics' mode
             library. It must have taken a lot of work, and since he released
             it into the public domain, why reinvent the wheel?  Go to his
             website and check out Paku Paku, the game that made him write
             this library in the first place!

                        (http://www.deathshadow.com/pakuPaku)

             As such, my code falls under the same (ultra-broad) public
             domain license - use it for any purpose whatsoever.
}
program LoCGAEd;
uses
   dialogs,
   jfunc,
   txtgraph;
const
   COLOR_BOX_X = 73;
   COLOR_BOX_Y = 23;
   COLOR_BOX_WIDTH = 21;
   COLOR_BOX_HEIGHT = 43;
   SIZES_BOX_X = 100;
   SIZES_BOX_Y = 23;
   SIZES_BOX_WIDTH = 23;
   SIZES_BOX_HEIGHT = 43;
   TOGGLES_BOX_X = 128;
   TOGGLES_BOX_Y = 33;
   TOGGLES_BOX_WIDTH = 23;
   TOGGLES_BOX_HEIGHT = 23;
   SPRITE_LIST_BOX_X = 25;
   SPRITE_LIST_BOX_Y = 82;
   SPRITE_LIST_BOX_WIDTH = 111;
   SPRITE_LIST_BOX_HEIGHT = 11;
   TITLE_X = 47;
   TITLE_Y = 2;
type
{ ========================================================================= }
{  STRUCTURE DEFINITIONS                                                    }
{ ========================================================================= }
   { A pointer used to access sprites in the sprite list (and to assemble
     the list itself. }
   SpriteEntryPtr = ^SpriteEntry;

   { In order to be able to treat colorData and transData like arrays,
     I have to create a special array type and create a pointer to it just to
     make the Pascal type checker happy.  Never change, Turbo Pascal. }
   ByteArray = Array[0..80] of byte;
   ByteArrayPtr = ^ByteArray;

   { A single record representing a sprite in the edit list, including color
     and transparency data.  This is a singly-linked list, so there's an
     additional pointer as well. }
   SpriteEntry = record
      colorData: ByteArrayPtr;
      transData: ByteArrayPtr;
      next: SpriteEntryPtr;
   end;

var
{ ========================================================================= }
{  GLOBAL VARIABLES                                                         }
{ ========================================================================= }
   { Used to loop out of the game when ESC is pressed. }
   g_ExitProgram: Boolean;

   { A global set when memory isn't available to create a new sprite. }
   g_OutOfMemory: Boolean;

   { The list of sprites in the sprite sheet. The color and transparency data
     must be allocated when creating a new sprite, and each will be equal
     to (g_SpriteSize x g_SpriteSize) bytes. }
   g_SpriteList:  SpriteEntryPtr;

   { A pointer to the currently active sprite to edit. }
   g_CurrentSprite: SpriteEntryPtr;

   { The copy buffer.  If the size of the sprites is changed between copy
     and paste, the copy buffer is cleared! }
   g_CopyBuffer: SpriteEntryPtr;

   { A count of the total number of sprites in the sprite sheet. }
   g_TotalSprites:     Integer;

   { The index of the currently active sprite within the sprite sheet. }
   g_currentSpriteIdx: Integer;

   { The name of the file to save and load from. }
   g_SpriteFileName: String;

   { The name of the headerless export file.  Has the same prefix as
     g_SpriteFileName, but has a '.DAT' extension instead. }
   g_exportFileName: String;

   { Holds the position of the cursor in the edit window }
   g_EditXPos:    Integer;
   g_EditYPos:    Integer;

   g_SpriteSize:  Integer;       { The size of each sprite in the strip. }
   g_ActiveColor: Integer;       { The currently selected draw color. }
   g_ShowGrid:    Boolean;       { 'Show Grid' toggle option. }
   g_ShowTrans:   Boolean;       { 'Show transparency' toggle option. }

   { In the edit box, each 'pixel' is square and has the same size as the
     others, but the actual size depends on the size of the sprite.  Those
     sizes are as follows:

       3x3 sprite:   17 pixels (total box size is 58x58)
       5x5 sprite:   9 pixels  (total box size is 54x54)
       7x7 sprite:   7 pixels  (total box size is 60x60)
       9x9 sprite:   5 pixels  (total box size is 58x58)

     g_EditBoxSize contains the size of these individual 'pixels'.  The
     entire edit box itself is centered at approximately (36,44), but the
     actual start positions are calculated using a function and held in the
     globals g_EditBoxStartX and g_EditBoxStartY.
   }
   g_EditBoxSize: Integer;
   g_EditBoxStartX: Integer;
   g_EditBoxStartY: Integer;
   g_EditBoxWidth: Integer;
   g_EditBoxHeight: Integer;

   { Note that g_RedrawAll is essentially the same as setting every other
     boolean in this group to True - it's an easy way to draw everything
     at once.  Alternately, individual components below can be set to true,
     and a call to DrawUI will only update those.
   }
   g_RedrawAll:        Boolean;  { Flag to force redraw of everything. }
   g_DrawBackground:   Boolean;  { Flag to force draw of the background. }
   g_DrawColorBox:     Boolean;  { Flag to force draw of the color box. }
   g_DrawSizesBox:     Boolean;  { Flag to force draw of the sprite size box. }
   g_DrawTogglesBox:   Boolean;  { Flag to force draw of the toggles box. }
   g_DrawEditWindow:   Boolean;  { Flag to force draw of the edit space. }
   g_DrawFramesWindow: Boolean;  { Flag to force draw of the frame strip. }
   g_DrawSpriteList:   Boolean;  { Flag to force draw of the sprite list. }

   { All sprite data used by the program itself. }
   g_SpriteTiles:      tDynamicData;

   { The font used to display dialog boxes. }
   g_font: pFontSet;

   { The status returned from file save/load operations.  If set to zero,
     the operation was successful, otherwise it was unsuccessful. }
   g_status: Integer;

   { Dialog status flags.  Used to limit input at appropriate times. }
   { g_LockInputFromDialog is set when a dialog is on the screen.  If this
     flag is set, then the next keypress is used, based on g_DialogLockID,
     to determine what action to take. }
   g_LockInputFromDialog: Boolean;

   { g_DialogLockID contains the index of the dialog responsible for
     the current lock.  This is used to make sure that the original operation
     associated with the dialog can be informed that a key was pressed to
     clear the dialog, and take action accordingly.
   }
   g_DialogLockID: Integer;

   { Temporarily holds the 'new' sprite size to be used if the resize
     warning dialog is approved. }
   g_NewSpriteSize: Integer;

{ ========================================================================= }
{  STUFF REQUIRED BY TURBO PASCAL FOR MEMORY MANAGEMENT                     }
{ ========================================================================= }

{ HeapFunc:  A modified heap function that makes GetMem return nil. }
function HeapFunc(Size: Word): Integer; far;
begin
   HeapFunc :=1;
end;

{ ========================================================================= }
{  FORWARD DECLARATIONS                                                     }
{   I like my functions in alphabetical order.  Pascal doesn't unless I     }
{   foward declare them.  So there.                                         }
{ ==========================================================================}
procedure AddEmptySprite; forward;
procedure CalculateEditWindowExtents; forward;
procedure ClearSpriteData(var ptr:SpriteEntryPtr); forward;
procedure CopySpriteBuffer(var src: SpriteEntryPtr); forward;
procedure CountSpriteListSize; forward;
procedure CreateEmptySprite(var head:SpriteEntryPtr); forward;
procedure DeallocateSprite(var ptr:SpriteEntryPtr); forward;
procedure DeleteAllSprites; forward;
procedure DeleteSpriteAtIndex(idx:Integer); forward;
procedure DrawActiveSpriteListEntry; forward;
procedure DrawColorBox; forward;
procedure DrawEditWindow; forward;
procedure DrawEditWindowHighlight; forward;
procedure DrawSizesBox; forward;
procedure DrawSpriteList; forward;
procedure DrawSpriteListEntry(spriteIdx, x, y: Integer); forward;
procedure DrawTogglesBox; forward;
procedure DrawUI; forward;
procedure GetPointerToSprite(idx: Integer; var ptr: SpriteEntryPtr); forward;
procedure HighlightActiveColor; forward;
procedure HighlightActiveSize; forward;
procedure HighlightToggles; forward;
procedure InitNewSpriteSize(size: Integer); forward;
procedure LoadSpriteListFromFile(var status: Integer); forward;
procedure PasteSpriteBuffer(var dest: SpriteEntryPtr); forward;
procedure ProcessInput; forward;
procedure SaveSpriteListToFile(var status: Integer; addHeader: Boolean); forward;

{ ========================================================================= }
{  FUNCTIONS / PROCEDURES                                                   }
{ ==========================================================================}

{ LoadSpriteListFromFile:  Loads a file with the name specified in
                           g_SpriteFileName.  Since the color and
                           transparency data are interleaved, we need to
                           account for that when loading.  Also, each sprite
                           has two representations in the file (one for
                           odd pixel offsets, one for even), but only the
                           first is used to reconstruct the sprite list since
                           all the data is present in both forms.
}
procedure LoadSpriteListFromFile(var status: Integer);
var
     spriteFile: file;
     header: array[0..5] of Integer;
     i, j, k: Integer;
     tmp: Integer;
     curSprite: SpriteEntryPtr;
     interleaveBuf: array[0..9] of Byte;
     interleaveOff: Integer;
begin
     assign(spriteFile, g_SpriteFileName);

     {$I-}
     reset(spriteFile, 1);
     {$I+}

     { If the file can't be opened, complain.  Otherwise, process it. }
     if (IOResult <> 0) then
     begin
          writeln('Unable to open file ', g_SpriteFileName, '!');
          status := -1;
     end
     else begin
        blockread(spriteFile, header, 6 * sizeof(integer));

        { Compare the first three integers to the magic values.  If they
          pass, then use the other three values to populate the appropriate
          parts of the relevant data structures. }
        if (header[0] <> 77) or (header[1] <> 88) or (header[2] <> 99) then
        begin
          writeln('Not a sprite file!');
          status := -1;
        end
        else begin

          InitNewSpriteSize(header[3]);

          for k:=0 to header[5] - 1 do
          begin
             { Create a new sprite on the sprite list, except for the first
               time around (InitNewSpriteSize created a single blank one
               for us already). }
             if (k <> 0) then
                 AddEmptySprite;

             GetPointerToSprite(g_totalSprites - 1, curSprite);

             { Looping through the sprite data on file and loading it
               happens here. }
             for i:= 0 to g_spriteSize - 1 do
             begin
               interleaveOff := 0;
               j := 0;

               { Read the first two bytes.  Their upper nibbles contain
                 the data for the first pixel in the row. }
               blockread(spriteFile, interleaveBuf, g_SpriteSize + 1);
               tmp := interleaveBuf[0];
               curSprite^.colorData^[i * g_SpriteSize + j] := (tmp shr 4);
               tmp := interleaveBuf[1];
               tmp := tmp and $F0;
               curSprite^.transData^[i * g_SpriteSize + j] := (tmp shr 4);
               j := j + 1;
               interleaveOff := interleaveOff + 2;
               while (j < (g_SpriteSize - 1)) do
               begin
                 tmp := interleaveBuf[interleaveOff];
                 tmp := tmp and $0F;
                 curSprite^.colorData^[i * g_SpriteSize + j] := tmp;
                 tmp := interleaveBuf[interleaveOff];
                 tmp := (tmp shr 4) and $0F;
                 curSprite^.colorData^[i * g_SpriteSize + j + 1] := tmp;
                 tmp := interleaveBuf[interleaveOff+1];
                 tmp := tmp and $0F;
                 curSprite^.transData^[i * g_SpriteSize + j] := tmp;
                 tmp := interleaveBuf[interleaveOff+1];
                 tmp := (tmp shr 4) and $0F;
                 curSprite^.transData^[i * g_SpriteSize + j + 1] := tmp;
                 interleaveOff :=  interleaveOff + 2;
                 j := j + 2;
               end;
             end;

             { Read the other data for the sprite.  We don't need it, since
               the previous data contained everything needed to reconstruct
               the sprite.
             }
             for i:=0 to g_spriteSize - 1 do
             begin
               blockread(spriteFile, interleaveBuf, g_SpriteSize + 1);
             end;
          end;
        end;
        close(spriteFile);
        { Set the current sprite to the first one in the list.}
        g_currentSpriteIdx := 0;
        status := 0;
     end;
end;

{ SaveSpriteListToFile: Creates a file with the name specified in
                        g_SpriteFileName containing (optionally) a short
                        header (12 bytes: 6 assorted bytes to identify 
                        the file (77d, 88d, 99d), 2 for x width, 2 for
                        y width, 2 for sprite count) and the raw 
                        sprite/transparency data.  The color and data 
                        transparancies for pairs of pixels are
                        interleaved.  Additionally, two versions of each
                        sprite are written to the file: one for blitting at
                        odd pixel offsets, and one for blitting at even
                        offsets.

                        Note: interleaveBuf needs to be large enough to
                              hold the largest sprite size supported by the
                              program, since it's not dynamically allocated.
}
procedure SaveSpriteListToFile(var status: Integer; addHeader: Boolean);
var
   spriteFile: file;
   header:  array[0..5] of Integer;     { Holds the header bytes }
   i, j: Integer;
   tmp: Integer;                        { Used to hold intermediate values.}
   curSprite: SpriteEntryPtr;           { The sprite being processed. }
   interleaveBuf: array[0..9] of Byte;
   interleaveOff: Integer;              { An offset within the interleave
                                          buffer.  Note that this is not
                                          always the same as the offset
                                          within the source buffer! }
begin
     header[0] := 77;
     header[1] := 88;
     header[2] := 99;
     header[3] := g_SpriteSize;
     header[4] := g_SpriteSize;
     header[5] := g_TotalSprites;

     { Open the file. }
     if (addHeader = True) then
     begin
       assign(spriteFile, g_SpriteFileName);
     end
     else begin
       assign(spriteFile, g_exportFileName);
     end;

     { Delete it if it already exists. }
     rewrite(spriteFile, 1);

     { Write the header out first. }
     if (addHeader = True) then
     begin
        blockwrite(spriteFile, header, 6 * sizeof(Integer));
     end;

     { Now, for each sprite in the sheet, write one byte of color data,
       then one byte of transparency data.  Because this data needs to be
       written twice (once for odd-pixel offset versions of the sprite,
       and once for even-pixel versions, we will pre-interleave an entire
       row of sprite data before writing it out. }
     GetPointerToSprite(0, curSprite);

     while (curSprite <> nil) do
     begin
       { Note that this is some highly kludgy code in that it repeats itself
         a lot.  It's because the sprite format is kind of ugly already,
         being optimized for quick rendering.

         Handle the odd offset version of the sprite first. This version of
         the sprite is of size x+1, y and contains an initial black pixel
         at the start of each row. }
       for i := 0 to g_spriteSize - 1 do
       begin
         j := 0;
         interleaveOff := 0;
         while (j < (g_spriteSize - 1)) do
         begin
              { If we're at the beginning of the row, the byte consists
                of a black pixel and the first pixel of the sprite row.
                Be sure to offset the interleave by 2 bytes, even though the
                source buffer is only offset by one byte!

                For the black pixel, the transparency buffer needs to be
                set to $F (hence the assignment to $F0 at first). }
              if (j = 0) then
              begin
                 tmp := $00;
                 tmp := tmp or (curSprite^.colorData^[i * g_spriteSize + j] shl 4);
                 interleaveBuf[0] := tmp;
                 tmp := $0F;
                 tmp := tmp or (curSprite^.transData^[i * g_spriteSize + j] shl 4);
                 interleaveBuf[1] := tmp;
                 interleaveOff := interleaveOff + 2;
                 j := j + 1;
              end else
              begin
                 { Each byte consists of the color attribute for the two
                   adjacent pixels, or the transparency attribute for the
                   two adjacent pixels.  In this case, both the interleave
                   and source pixel data are offset by 2 after this cycle. }
                 tmp := $00;
                 tmp := (curSprite^.colorData^[i * g_spriteSize + j + 1] shl 4);
                 tmp := tmp or curSprite^.colorData^[i * g_spriteSize + j];
                 interleaveBuf[interleaveOff] := tmp;
                 tmp := $00;
                 tmp := (curSprite^.transData^[i * g_spriteSize + j + 1] shl 4);
                 tmp := tmp or curSprite^.transData^[i * g_spriteSize + j];
                 interleaveBuf[interleaveOff+1] := tmp;
                 interleaveOff := interleaveOff + 2;
                 j := j + 2;
              end;
         end;
         blockwrite(spriteFile, interleaveBuf, g_spriteSize + 1);
       end;

       { Then handle the even part.  Even sprites are of size x+1, y and
         contain an extra transparent black pixel at the end of each row. }
       for i := 0 to g_spriteSize - 1 do
       begin
         j := 0;
         interleaveOff := 0;
         while (j <= (g_spriteSize - 1)) do
         begin
              { If we're at the last pixel of the row, then pad it with an
                extra black, transparent pixel (the bitwise AND of $0F to
                the pixel data will always set the last pixel to zero, and
                the bitwise OR of $0F will always set the transparency mask
                for the last pixel to $F.}
              if (j = (g_spriteSize - 1)) then
              begin
                 tmp := (curSprite^.colorData^[i * g_spriteSize + j]);
                 tmp := tmp and $0F;
                 interleaveBuf[interleaveOff] := tmp;
                 tmp := (curSprite^.transData^[i * g_spriteSize + j]);
                 tmp := tmp or $F0;
                 interleaveBuf[interleaveOff+1] := tmp;
                 interleaveOff := interleaveOff + 2;
                 j := j + 1;
              end else
              begin
                 tmp := $00;
                 tmp := (curSprite^.colorData^[i * g_spriteSize + j + 1] shl 4);
                 tmp := tmp or curSprite^.colorData^[i * g_spriteSize + j];
                 interleaveBuf[interleaveOff] := tmp;
                 tmp := $00;
                 tmp := (curSprite^.transData^[i * g_spriteSize + j + 1] shl 4);
                 tmp := tmp or curSprite^.transData^[i * g_spriteSize + j];
                 interleaveBuf[interleaveOff+1] := tmp;
                 interleaveOff := interleaveOff + 2;
                 j := j + 2;
              end;
         end;
         blockwrite(spriteFile, interleaveBuf, g_spriteSize + 1);
       end;

       { Then, finally move to the next sprite if there is one. }
       curSprite := curSprite^.next;
     end;

     { Finally, close the file.  Phew! }
     close(spriteFile);

     { Everything was good, return 0 status. }
     status := 0;
end;


{ GetPointerToSprite: Returns a pointer to the sprite specified by the
                      provided index.  Does no direct range checking; the
                      calling functions should range check beforehand.
}
procedure GetPointerToSprite(idx: Integer; var ptr: SpriteEntryPtr);
var
   tmpPtr: SpriteEntryPtr;
   i: Integer;
begin
     tmpPtr := g_SpriteList;

     if (idx = 0) then begin
        ptr := tmpPtr;
     end else
     begin
        for i :=0 to (idx-1) do
        begin
           tmpPtr := tmpPtr^.next;
        end;
        ptr := tmpPtr;
     end;
end;

{ ClearSpriteData:   Sets all pixels of a sprite to black/transparent.  This
                     is effectively equal to 'clearing' a sprite.
}
procedure ClearSpriteData(var ptr:SpriteEntryPtr);
begin
    FillChar(ptr^.colorData^, g_SpriteSize * g_SpriteSize, $00);
    FillChar(ptr^.transData^, g_SpriteSize * g_SpriteSize, $0f);
end;

procedure DrawDialogBox(idx: Integer);
var
   i: Integer;
   d: DialogPtr;
begin

    { Get the global dialog associated with this ID. }
    d := g_DialogList[idx];

    { Draw the border of the dialog and fill it with the appropriate color. }
    tg_rectangle(d^.xPos - 1,
                 d^.yPos - 1,
                 d^.xPos + d^.width,
                 d^.yPos + d^.height,
                 c_white);
    tg_rectangle(d^.xPos,
                 d^.yPos,
                 d^.xPos + d^.width - 1,
                 d^.yPos + d^.height - 1,
                 d^.borderColor);
    tg_bar(d^.xPos + 1,
           d^.yPos + 1,
           d^.xPos + d^.width - 2,
           d^.yPos + d^.height - 2,
           d^.innerColor);

    { Draw any text lines in the dialog. }
    if (d^.numLines > 0) then
    begin
       for i:=0 to d^.numLines - 1 do
       begin
            g_font^.outTextCentered(d^.lineX[i],
                                    d^.lineY[i],
                                    d^.lines[i],
                                    d^.textColor);
       end;
    end;
end;

{ DrawEditWindowHighlight:  Highlights (draws a yellow rectangle around) the
                            currently active pixel in the edit window.
}
procedure DrawEditWindowHighlight;
var
   sx, sy: Integer;
begin

   { Calculate the starting point of the highlight box... }
   sx := g_EditBoxStartX + 1 + ((g_EditBoxSize + 1) * g_EditXPos);
   sy := g_EditBoxStartY + 1 + ((g_EditBoxSize + 1) * g_EditYPos);

   { ...and then draw it. }
   tg_rectangle(sx,
                sy,
                sx + g_EditBoxSize + 1,
                sy + g_EditBoxSize + 1,
                c_yellow);
end;

{ CalculateEditWindowExtents - determines the size and position of the
                               edit window (and the size of each constituent
                               'pixel'), depending on what the current sprite
                               size is.  These values are used to draw the
                               edit window in the correct position and
                               orientation.
}
procedure CalculateEditWindowExtents;
begin
   { 3x3 sprites use a box with really huge 'pixels'. }
   if (g_SpriteSize = 3) then
   begin
      g_EditBoxStartX := 8;
      g_EditBoxStartY := 16;
      g_EditBoxWidth := 58;
      g_EditBoxHeight := 58;
      g_EditBoxSize := 17;
   end;

   if (g_SpriteSize = 5) then
   begin
      g_EditBoxStartX := 10;
      g_EditBoxStartY := 18;
      g_EditBoxWidth := 54;
      g_EditBoxHeight := 54;
      g_EditBoxSize := 9;
   end;

   if (g_SpriteSize = 7) then
   begin
      g_EditBoxStartX := 7;
      g_EditBoxStartY := 15;
      g_EditBoxWidth := 60;
      g_EditBoxHeight := 60;
      g_EditBoxSize := 7;
   end;

   if (g_SpriteSize = 9) then
   begin
      g_EditBoxStartX := 8;
      g_EditBoxStartY := 16;
      g_EditBoxWidth := 58;
      g_EditBoxHeight := 58;
      g_EditBoxSize := 5;
   end;
end;

{  DrawEditWindow - draws the edit window and the currently active sprite
                    being worked on inside of it.  Also draws transparency
                    dots and the grid if the appropriate toggle is set.
}
procedure DrawEditWindow;
var
   i, j: Integer;
   sx, sy: Integer;
   ptr: SpriteEntryPtr;
begin
   { Draw the outer, lighter colored box. }
   tg_rectangle(g_EditBoxStartX,
                g_EditBoxStartY,
                g_EditBoxStartX + g_EditBoxWidth - 1,
                g_EditBoxStartY + g_EditBoxHeight - 1,
                c_ltgrey);

   { Draw the inner, darker colored box. }
   tg_rectangle(g_EditBoxStartX + 1,
                g_EditBoxStartY + 1,
                g_EditBoxStartX + g_EditBoxWidth - 2,
                g_EditBoxStartY + g_EditBoxHeight - 2,
                c_dkgrey);

   { If the grid is enabled, draw the lines of the grid. }
   if (g_ShowGrid = true) then
   begin
        for i:= 1 to g_SpriteSize do
        begin
             tg_bar(g_EditBoxStartX + 2,
                    g_EditBoxStartY + (i * (g_EditBoxSize + 1)) + 1,
                    g_EditBoxStartX + g_EditBoxWidth - 3,
                    g_EditBoxStartY + (I * (g_EditBoxSize + 1)) + 1,
                    c_ltgrey);
             tg_bar(g_EditBoxStartX + (i * (g_EditBoxSize + 1)) + 1,
                    g_EditBoxStartY + 2,
                    g_EditBoxStartX + (i * (g_EditBoxSize + 1)) + 1,
                    g_EditBoxStartY + g_EditBoxHeight - 3,
                    c_ltgrey);
        end;
   end;

   GetPointerToSprite(g_CurrentSpriteIdx, ptr);

   { Draw the contents of each 'pixel' in the edit window. If the grid
     is disabled, the 'pixels' will each be one pixel wider and one pixel
     taller (taking up the space normally used by the grid). }
   for i:=0 to (g_SpriteSize-1) do
   begin
        for j:=0 to (g_SpriteSize - 1) do
        begin
           sx := g_EditBoxStartX + 2 + ((g_EditBoxSize + 1) * j);
           sy := g_EditBoxStartY + 2 + ((g_EditBoxSize + 1) * i);
           if (g_showGrid = True) then begin
              tg_bar(sx,
                     sy,
                     sx + (g_EditBoxSize - 1),
                     sy + (g_EditBoxSize - 1),
                     ptr^.colorData^[(i*g_SpriteSize) + j]);
           end
           else begin
              tg_bar(sx,
                     sy,
                     sx + (g_EditBoxSize),
                     sy + (g_EditBoxSize),
                     ptr^.colorData^[(i*g_SpriteSize) + j]);
           end;

           { Finally, draw transparency dots if requested. }
           if (g_showTrans = True) and
              (ptr^.transData^[(i*g_SpriteSize) + j] = $0f) then
           begin
              tg_putPixel(sx + (g_EditBoxSize div 2),
                          sy + (g_EditBoxSize div 2),
                          c_white);
           end;
        end;
   end;

   { Draw the highlight around the currently active pixel. }
   DrawEditWindowHighlight;
end;

{ DrawColorBox - renders the box displaying the palette of user-selectable
                 drawing colors.  Creates a small (8x4) swatch of each color
                 in two columns of 8 colors each.
}
procedure DrawColorBox;
var
   i, j: Integer;
begin
   { Draw the outer, lighter colored box. }
   tg_rectangle(COLOR_BOX_X,
                COLOR_BOX_Y,
                COLOR_BOX_X + COLOR_BOX_WIDTH - 1,
                COLOR_BOX_Y + COLOR_BOX_HEIGHT - 1,
                c_ltGrey);

   { Draw lines to separate each of the 8 rows (9 lines total). }
   for i := 0 to 8 do
        tg_rectangle(COLOR_BOX_X + 1,
                     COLOR_BOX_Y + (i * 5) + 1,
                     COLOR_BOX_X + COLOR_BOX_WIDTH - 2,
                     COLOR_BOX_Y + (i * 5) + 1,
                     c_dkGrey);

   { Draw lines to separate each of the columns (3 lines total). }
   for i := 0 to 2 do
        tg_rectangle(COLOR_BOX_X + (i * 9) + 1,
                     COLOR_BOX_Y + 1,
                     COLOR_BOX_X + (i * 9) + 1,
                     COLOR_BOX_Y + COLOR_BOX_HEIGHT - 2,
                     c_dkGrey);

   { Create the color swatches inside the box, one row (2 colors) at a time.
     Given any row from 0 to 7, the colors in that row will be (row #) and
     (row # + 8). }
   for i := 0 to 7 do
   begin
        tg_bar(COLOR_BOX_X + 2,
               COLOR_BOX_Y + (i * 5) + 2,
               COLOR_BOX_X + 9,
               COLOR_BOX_Y + (i * 5) + 5,
               i);
        tg_bar(COLOR_BOX_X + 11,
               COLOR_BOX_Y + (i * 5) + 2,
               COLOR_BOX_X + 18,
               COLOR_BOX_Y + (i * 5) + 5,
               i + 8);
   end;

end;

{ HighlightActiveColor - draws a highlight (yellow box) around the currently
                         selected draw color.
}
procedure HighlightActiveColor;
var
     x, y: Integer;
begin
     { If the color is in the second column, adjust the x position
       appropriately.  Select the y position as well, adjusting for
       relative position if the color is in the second column. }
     if (g_ActiveColor > 7) then begin
        x := COLOR_BOX_X + 10;
        y := COLOR_BOX_Y + (5 * (g_ActiveColor - 8)) + 1;
     end
     else begin
        x := COLOR_BOX_X + 1;
        y := COLOR_BOX_Y + (5 * g_ActiveColor) + 1;
     end;

     { Draw the highlight. }
     tg_rectangle(x, y, x+9, y+5, c_yellow);

end;

{ DrawSizesBox - renders the box that contains the user-selectable sprite
                 sizes, including the text inside each box (which is
                 currently drawn using sprites instead of text rendering
                 functions.
}
procedure DrawSizesBox;
var i: Integer;
begin
     { Draw the lighter outer border of the box. }
     tg_rectangle(SIZES_BOX_X,
                  SIZES_BOX_Y,
                  SIZES_BOX_X + SIZES_BOX_WIDTH - 1,
                  SIZES_BOX_Y + SIZES_BOX_HEIGHT - 1,
                  c_ltgrey);

     { Draw the darker inner border of the box. }
     tg_rectangle(SIZES_BOX_X + 1,
                  SIZES_BOX_Y + 1,
                  SIZES_BOX_X + SIZES_BOX_WIDTH - 2,
                  SIZES_BOX_Y + SIZES_BOX_HEIGHT - 2,
                  c_dkgrey);

     { Draw a line separating each button in the box -
       (3 lines, 4 boxes total) }
     for i:=1 to 3 do
        tg_rectangle(SIZES_BOX_X + 2,
                     SIZES_BOX_Y + (i * 10) + 1,
                     SIZES_BOX_X + SIZES_BOX_WIDTH - 3,
                     SIZES_BOX_Y + (i * 10) + 1,
                     c_dkgrey);

     { Draw the sprites that represent the insides of the four size
       buttons. }
     for i:=0 to 3 do
     begin
        tg_tile5(SIZES_BOX_X + 4, SIZES_BOX_Y + (i * 10) + 4, (i * 3));
        tg_tile5(SIZES_BOX_X + 9, SIZES_BOX_Y + (i * 10) + 4, (i * 3) + 1);
        tg_tile5(SIZES_BOX_X + 14, SIZES_BOX_Y + (i * 10) + 4, (i * 3) + 2);
     end;
end;

{ HighlightActiveSize - draws a highlight (yellow box) around the currently
                        selected sprite size.
}
procedure HighlightActiveSize;
var
   y: Integer;  { The y position of the start of the higlighted box. }
begin
     { The 'default' - applies to sprite size of 3 and any other size not
       officially supported, mainly as a sanity assignment. }
     y := SIZES_BOX_Y + 1;

     { Otherwise, determine the y position based on the sprite size. }
     if (g_SpriteSize = 5) then
        y := SIZES_BOX_Y + 11;
     if (g_SpriteSize = 7) then
        y := SIZES_BOX_Y + 21;
     if (g_SpriteSize = 9) then
        y := SIZES_BOX_Y + 31;

     { Draw the highlight rectangle in the correct place. }
     tg_rectangle(SIZES_BOX_X + 1,
                  y,
                  SIZES_BOX_X + SIZES_BOX_WIDTH - 2,
                  y + 10,
                  c_yellow);
end;

{ DrawTogglesBox - displays the 'toggles' box, showing options that can be
                   turned off and on by the user.  Currently, there are two
                   such options: 'Show Grid' and 'Enable Transparency'.
}
procedure DrawTogglesBox;
begin
     { Draw the outer border of the box. }
     tg_rectangle(TOGGLES_BOX_X,
                  TOGGLES_BOX_Y,
                  TOGGLES_BOX_X + TOGGLES_BOX_WIDTH - 1,
                  TOGGLES_BOX_Y + TOGGLES_BOX_HEIGHT - 1,
                  c_ltgrey);

     { Draw the darker inner border of the top toggle option. }
     tg_rectangle(TOGGLES_BOX_X + 1,
                  TOGGLES_BOX_Y + 1,
                  TOGGLES_BOX_X + TOGGLES_BOX_WIDTH - 2,
                  TOGGLES_BOX_Y + 11,
                  c_dkgrey);

     { Draw the darker inner border of the bottom toggle option. }
     tg_rectangle(TOGGLES_BOX_X + 1,
                  TOGGLES_BOX_Y + 11,
                  TOGGLES_BOX_X + TOGGLES_BOX_WIDTH - 2,
                  TOGGLES_BOX_Y + 21,
                  c_dkgrey);

     { Tiles 12-15 contain the 'grid' toggle sprite.  Tiles 16-21
       contain the 'transparency' toggle sprite. }
     tg_tile5(TOGGLES_BOX_X+8, TOGGLES_BOX_Y+3, 12);
     tg_tile5(TOGGLES_BOX_X+13, TOGGLES_BOX_Y+3, 13);
     tg_tile5(TOGGLES_BOX_X+8, TOGGLES_BOX_Y+8, 14);
     tg_tile5(TOGGLES_BOX_X+13, TOGGLES_BOX_Y+8, 15);

     tg_tile5(TOGGLES_BOX_X+4, TOGGLES_BOX_Y+13, 16);
     tg_tile5(TOGGLES_BOX_X+9, TOGGLES_BOX_Y+13, 17);
     tg_tile5(TOGGLES_BOX_X+14, TOGGLES_BOX_Y+13, 18);
     tg_tile5(TOGGLES_BOX_X+4, TOGGLES_BOX_Y+18, 19);
     tg_tile5(TOGGLES_BOX_X+9, TOGGLES_BOX_Y+18, 20);
     tg_tile5(TOGGLES_BOX_X+14, TOGGLES_BOX_Y+18, 21);
end;

{ HighlightToggles - displays highlights (yellow boxes) around any options
                     that are selected in the toggles box.
}
procedure HighlightToggles;
begin
     { Draw the highlight box around the 'Show Grid' option if selected. }
     if (g_ShowGrid = true) then
        tg_rectangle(TOGGLES_BOX_X + 1,
                     TOGGLES_BOX_Y + 1,
                     TOGGLES_BOX_X + TOGGLES_BOX_WIDTH - 2,
                     TOGGLES_BOX_Y + 11,
                     c_yellow);

     { Draw the highlight box around the 'Show Transparency' option if
       selected. }
     if (g_ShowTrans = true) then
        tg_rectangle(TOGGLES_BOX_X + 1,
                     TOGGLES_BOX_Y + 11,
                     TOGGLES_BOX_X + TOGGLES_BOX_WIDTH - 2,
                     TOGGLES_BOX_Y + 21,
                     c_yellow);
end;

{ DrawSpriteListEntry - draws a single sprite (the one represented by
                        'spriteIdx' at the specified location.  If no
                        sprite exists at the desired index, draw a 'no
                        sprite' placeholder instead.
}
procedure DrawSpriteListEntry(spriteIdx, x, y: Integer);
var
   i, j: Integer;
   sx, sy: Integer;
   ptr: SpriteEntryPtr;
begin
     if (spriteIdx < 0) or (spriteIdx >= g_TotalSprites) then
     begin
          { No valid sprite data exists at this index; draw the appropriate
            'no sprite' symbol here. }
          tg_bar(x, y, x+8, y+8, c_dkgrey);
          tg_putpixel(x, y, c_ltgrey);
          tg_putpixel(x+8, y, c_ltgrey);
          tg_putpixel(x, y+8, c_ltgrey);
          tg_putpixel(x+8, y+8, c_ltgrey);
     end else
     begin
          sx := x;
          sy := y;

          if (g_spriteSize = 7) then begin
             sx := x + 1;
             sy := y + 1;
          end;
          if (g_spriteSize = 5) then begin
             sx := x + 2;
             sy := y + 2;
          end;
          if (g_spriteSize = 3) then begin
             sx := x + 3;
             sy := y + 3;
          end;

          { Get a pointer to the appropriate sprite data so we can draw it. }
          GetPointerToSprite(spriteIdx, ptr);

          { Clear the entire background of the sprite to grey. }
          tg_bar(x, y, x+8, y+8, c_dkgrey);

          { Loop through the pixels and draw the sprite data. }
          for i := 0 to g_SpriteSize - 1 do
          begin
               for j := 0 to g_SpriteSize - 1 do
               begin
                    tg_putpixel(sx + j, sy + i,
                                ptr^.colorData^[i * g_SpriteSize + j]);
               end;
          end;

     end;
end;

{ DrawSpriteList - draws the preview panel for sprites that have already
                   been added to the sprite sheet.  This is a horizontal
                   panel of sprites, with the currently active sprite in
                   the center of the panel.  All sprites are shown in
                   actual size, though the panel frames are 9x9 no matter
                   what size the sprites are, and smaller sprites are drawn
                   in the center of each panel.

                   A separate function (DrawSpriteListEntry) is responsible
                   for drawing each separate sprite in the panel at the
                   desired location.  This allows single sprites to be
                   easily updated, an important operation for the active
                   sprite, which is updated each time a pixel is changed.
}
procedure DrawSpriteList;
var
   i, j: Integer;
begin
   { Draw the outer border of the box. }
   tg_rectangle(SPRITE_LIST_BOX_X,
                SPRITE_LIST_BOX_Y,
                SPRITE_LIST_BOX_X + SPRITE_LIST_BOX_WIDTH - 1,
                SPRITE_LIST_BOX_Y + SPRITE_LIST_BOX_HEIGHT - 1,
                c_ltgrey);

   { Draw the lines between each box in the list - there are 10 such
     lines. }
   for i := 1 to 10 do
   begin
        tg_rectangle(SPRITE_LIST_BOX_X + (10 * i),
                     SPRITE_LIST_BOX_Y + 1,
                     SPRITE_LIST_BOX_X + (10 * i),
                     SPRITE_LIST_BOX_Y + 9,
                     c_ltgrey);
   end;

   { Draw the decorative effects around the central box; the 'active'
     sprite is always drawn here. }
   tg_rectangle(SPRITE_LIST_BOX_X + 50,
                SPRITE_LIST_BOX_Y,
                SPRITE_LIST_BOX_X + 60,
                SPRITE_LIST_BOX_Y + 10,
                c_yellow);
   tg_rectangle(SPRITE_LIST_BOX_X + 51,
                SPRITE_LIST_BOX_Y - 1,
                SPRITE_LIST_BOX_X + 59,
                SPRITE_LIST_BOX_Y - 1,
                c_yellow);
   tg_rectangle(SPRITE_LIST_BOX_X + 53,
                SPRITE_LIST_BOX_Y - 2,
                SPRITE_LIST_BOX_X + 57,
                SPRITE_LIST_BOX_Y - 2,
                c_yellow);
   tg_rectangle(SPRITE_LIST_BOX_X + 51,
                SPRITE_LIST_BOX_Y + 11,
                SPRITE_LIST_BOX_X + 59,
                SPRITE_LIST_BOX_Y + 11,
                c_yellow);
   tg_rectangle(SPRITE_LIST_BOX_X + 53,
                SPRITE_LIST_BOX_Y + 12,
                SPRITE_LIST_BOX_X + 57,
                SPRITE_LIST_BOX_Y + 12,
                c_yellow);

   { Now, for each of the 11 active sprite areas, determine the indexes
     for the sprites that should be drawn there.  The DrawSpriteListEntry
     function will determine whether a sprite or an 'empty' item is drawn
     in the specified location. }
   for i := 0 to 10 do
   begin
        DrawSpriteListEntry(g_CurrentSpriteIdx + i - 5,
                            SPRITE_LIST_BOX_X + (i * 10) + 1,
                            SPRITE_LIST_BOX_Y + 1);
   end;
end;

{ DrawActiveSpriteListEntry - a helper function to draw the active sprite
                              in the sprite list on demand.
}
procedure DrawActiveSpriteListEntry;
begin
     DrawSpriteListEntry(g_CurrentSpriteIdx,
                         SPRITE_LIST_BOX_X + 51,
                         SPRITE_LIST_BOX_Y + 1);
end;

{ DrawUI - renders all UI components, including static and dynamic ones.
           A global boolean (g_RedrawAll) will cause all UI elements to be
           redrawn if set to true.  Otherwise, individual elements will be
           drawn depending on whether global flags controlling those are set.
           The booleans are defined as follows:

           g_DrawBackground
           g_DrawColorBox
           g_DrawSizesBox
           g_DrawTogglesBox
           g_DrawEditWindow
           g_DrawFramesWindow
}
procedure DrawUI;
var i: Integer;
begin
     if (g_RedrawAll = True) or (g_DrawBackground = True) then begin
       tg_clear(c_black);                     { Clear the screen }
       tg_rectangle(0, 0, 159, 99, c_ltBlue); { Create a light blue border }
       tg_bar(1, 1, 158, 7, c_blue);          { Create a dark region on top }
       tg_bar(1, 8, 158, 8, c_ltBlue);        { Create a single line right }
                                              { below it. }
       { The 14 tiles starting from offset 22 in the file contain the title
         data.  Read through them and paste them in the correct place. }
       for i:= 0 to 13 do
       begin
         tg_tile5(TITLE_X + (i*5), TITLE_Y, i+22);
       end;
       g_DrawBackground := false;
     end;

     if (g_RedrawAll = True) or (g_DrawColorBox = True) then begin
       DrawColorBox;                          { Draw the color picker box }
       HighlightActiveColor;                  { Show which color is selected }
       g_DrawColorBox := false;
     end;

     if (g_RedrawAll = True) or (g_DrawSizesBox = True) then begin
       DrawSizesBox;
       HighlightActiveSize;
       g_DrawSizesBox := false;
     end;

     if (g_RedrawAll = True) or (g_DrawTogglesBox = True) then begin
       DrawTogglesBox;
       HighlightToggles;
       g_DrawTogglesBox := false;
     end;

     if (g_RedrawAll = True) or (g_DrawEditWindow = True) then begin
        DrawEditWindow;
        g_DrawEditWindow := false;
     end;

     if (g_RedrawAll = True) or (g_DrawSpriteList = True) then begin
        DrawSpriteList;
        g_DrawSpriteList := false;
     end;

     g_RedrawAll := false;
end;

{ CreateEmptySprite: Creates a structure to hold a single sprite.  Includes
                     dynamic allocation of the color and transparency data.
                     Returns the new sprite data at the memory location
                     pointed to by 'head'.  If memory allocation fails
                     at any point, stop the creation process and set
                     g_OutOfMemory, so we can eventually display something
                     to the user.
}
procedure CreateEmptySprite(var head:SpriteEntryPtr);
begin
     New(head);
     if(head <> nil) then
     begin
        getmem(head^.colorData, g_SpriteSize * g_SpriteSize);
        if(head^.colorData <> nil) then
        begin
           getmem(head^.transData, g_SpriteSize * g_SpriteSize);
           if(head^.transData <> nil) then
           begin
                ClearSpriteData(head);
                head^.next := nil;
           end else
           begin
                g_OutOfMemory := True;
                freemem(head^.colorData, g_SpriteSize * g_SpriteSize);
                dispose(head);
           end;
        end else
        begin
             g_OutOfMemory := True;
             dispose(head);
        end;
     end else
     begin
          g_OutOfMemory := True;
     end;
end;

{ DeallocateSprite: Deletes memory associated with a sprite.  This should be
                    called before resizing the sprite (or g_SpriteSize will
                    contain an incorrect size value.)
}
procedure DeallocateSprite(var ptr:SpriteEntryPtr);
begin
     freemem(ptr^.colorData, g_SpriteSize * g_SpriteSize);
     freemem(ptr^.transData, g_SpriteSize * g_SpriteSize);
     dispose(ptr);
end;

{ DeleteSpriteAtIndex:  Deletes a sprite at the specified index,
                        assuming the index is in the range
                        0..(g_TotalSprites - 1)

  This function will adjust g_CurrentSprite and g_CurrentSpriteIdx as
  necessary to point to a valid sprite.

  If there is only one sprite (g_TotalSprites = 1), then this function
  does nothing.
}
procedure DeleteSpriteAtIndex(idx:Integer);
var
    tmpPtr, tmpPtr2, tmpPtr3: SpriteEntryPtr;
    i: Integer;
begin
    { Only delete the element if there's more than one. }
    if (g_TotalSprites > 1) then
    begin
         { Special case, where idx to delete is zero }
         if (idx = 0) then
         begin
             tmpPtr := g_SpriteList;
             tmpPtr2 := g_SpriteList^.next;
             DeallocateSprite(tmpPtr);
             g_SpriteList := tmpPtr2;
             if (g_CurrentSpriteIdx > 0) then
             begin
                g_CurrentSpriteIdx := g_CurrentSpriteIdx - 1;
             end;
             g_TotalSprites := g_TotalSprites - 1;
         end;

         if (idx > 0) and (idx < g_TotalSprites) then
         begin
             tmpPtr := g_SpriteList;
             for i:= 0 to (idx - 2) do
             begin
                  tmpPtr := tmpPtr^.next;
             end;
             tmpPtr2 := tmpPtr^.next;
             if (tmpPtr2^.next = nil) then
             begin
                tmpPtr3 := nil;
             end else
             begin
                tmpPtr3 := tmpPtr2^.next;
             end;
             DeallocateSprite(tmpPtr2);
             tmpPtr^.next := tmpPtr3;

             {Now, resolve the value of g_CurrentSpriteIdx...}
             if (idx = g_CurrentSpriteIdx) then
             begin
                g_CurrentSpriteIdx := idx;
             end
             else if (idx < g_CurrentSpriteIdx) then
             begin
                g_CurrentSpriteIdx := g_CurrentSpriteIdx -1;
             end;
             g_TotalSprites := g_TotalSprites - 1;
         end;

         { Clamp the resulting value to the range of valid sprites.  This
           is more of a sanity check than anything. }
         if (g_CurrentSpriteIdx < 0) then g_CurrentSpriteIdx := 0;
         if (g_CurrentSpriteIdx >= g_TotalSprites) then
             g_CurrentSpriteIdx := g_TotalSprites - 1;

         { Finally, move the pointer to the offset implied by
           g_CurrentSpriteIdx. }
         GetPointerToSprite(g_CurrentSpriteIdx, g_CurrentSprite);
    end;
end;

{ DeleteAllSprites:  A helper function that deallocates the memory for all
                     saved sprites.  Used before loading new sprite sheets.
}
procedure DeleteAllSprites;
begin
     if (g_SpriteList <> nil) then
     begin
        while (g_TotalSprites > 1) do
            DeleteSpriteAtIndex(0);

        DeallocateSprite(g_SpriteList);
        g_SpriteList := nil;
        g_TotalSprites := 0;
        g_CurrentSpriteIdx := 0;
     end;
end;

{ AddEmptySprite: Adds an empty sprite to the editable sprite list.  This
                  sprite is added to the end of the list.  The list is
                  assumed to be the global g_SpriteList.  The total number
                  of sprites (held in g_TotalSprites) is incremented by one.
}
procedure AddEmptySprite;
var
   tmpPtr, newItemPtr: SpriteEntryPtr;
begin
   CreateEmptySprite(newItemPtr);

   { As long as the create succeeded, attach the new sprite to the list. }
   if (newItemPtr <> nil) then
   begin
      if (g_SpriteList = nil) then
      begin
            g_SpriteList := newItemPtr;
      end
      else begin
         tmpPtr := g_SpriteList;
         while (tmpPtr^.next <> nil) do
         begin
            tmpPtr := tmpPtr^.next;
         end;
         tmpPtr^.next := newItemPtr;
      end;

      { Add one to the total sprite count.  Don't move the pointer or index
        of the current sprite, though. }
      g_TotalSprites := g_TotalSprites + 1;
   end
   else begin
      g_LockInputFromDialog := True;
      g_DialogLockID := DIALOG_WARN_ADD_FAILED;
      DrawDialogBox(DIALOG_WARN_ADD_FAILED);
   end;
end;

{ CountSpriteListSize: a debug function to print lengths and offsets of
                       linked lists used for sprite indexes.
}
procedure CountSpriteListSize;
var
   i: Integer;
   tmpPtr: SpriteEntryPtr;
begin
   i:=1;
   tmpPtr := g_SpriteList;
   while (tmpPtr^.next <> nil) do
   begin
        tmpPtr := tmpPtr^.next;
        i := i + 1;
   end;

   writeln('The list element count is ', i, '.');
   writeln('The stored element count is ', g_TotalSprites, '.');
   writeln('The list index is ', g_CurrentSpriteIdx, '.');
   writeln;
end;

{ CopySpriteBuffer: Allocates a buffer to hold a copy of the current sprite,
                    to be used for the purpose of pasting.  The sprite
                    pointed to by 'src' will be copied into this temporary
                    buffer.  Note that when the sprite size is changed, any
                    current contents of the copy buffer will be cleared!
}
procedure CopySpriteBuffer(var src: SpriteEntryPtr);
var
   i, j: Integer;
begin
     if (g_CopyBuffer = nil) then
     begin
          CreateEmptySprite(g_CopyBuffer);
          ClearSpriteData(g_CopyBuffer);
     end;

     if (g_CopyBuffer <> nil) then
     begin
         for i:= 0 to g_SpriteSize - 1 do
         begin
            for j:= 0 to g_SpriteSize - 1 do
            begin
                 g_CopyBuffer^.colorData^[i * g_SpriteSize + j] :=
                    src^.colorData^[i * g_SpriteSize + j];
                 g_CopyBuffer^.transData^[i * g_SpriteSize + j] :=
                    src^.transData^[i * g_SpriteSize + j];
            end;
         end;
     end;
end;

{ PasteSpriteBuffer:  Takes the contents of the paste buffer (if any) and
                      copies them to the sprite pointed to by 'dest'.  This
                      buffer will be nil if a copy buffer hasn't been created
                      yet, or if a sprite size change caused the existing
                      paste buffer to be removed.
}
procedure PasteSpriteBuffer(var dest: SpriteEntryPtr);
var
   i, j: Integer;
begin
     if (g_CopyBuffer <> nil) then
     begin
          for i:= 0 to g_SpriteSize - 1 do
          begin
               for j:= 0 to g_SpriteSize - 1 do
               begin
                    dest^.colorData^[i * g_SpriteSize + j] :=
                       g_CopyBuffer^.colorData^[i * g_SpriteSize + j];
                    dest^.transData^[i * g_SpriteSize + j] :=
                       g_CopyBuffer^.transData^[i * g_SpriteSize + j];
               end;
          end;
     end;
end;

{ InitNewSpriteSize: Performs all the housekeeping necessary to switch the
                     current sprite size to a new value.  Deletes all old
                     sprites and the copy buffer, creates a new sprite list
                     with a single sprite in the new size, and resets the
                     editor window.
}
procedure InitNewSpriteSize(size: Integer);
begin
   { Delete the existing sprite sheet. }
   DeleteAllSprites;

   { Clear the copy buffer, if it exists. }
   if (g_CopyBuffer <> nil) then begin
      DeallocateSprite(g_CopyBuffer);
      g_CopyBuffer := nil;
   end;

   { Now, set the new size. The previous steps are dependent on the global
     sprite size being set to the old value. }
   g_SpriteSize := size;

   { Create a single empty sprite in the sheet and make it the current one. }
   AddEmptySprite;
   g_CurrentSprite := g_SpriteList;
   g_CurrentSpriteIdx := 0;

   { Reset the cursor position in the editor to (0,0) }
   g_EditXPos := 0;
   g_EditYPos := 0;

   { Recalculate grid and editor window spacing for the new sprite size. }
   CalculateEditWindowExtents;
end;

{ ProcessInput - Determine if a key is pressed.  Do things accordingly. }
procedure ProcessInput;
var
  keyRead: Char;
  extended: Boolean;
  status: Integer;
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

      { Process all actions related to dialogs on-screen. }
      if (g_LockInputFromDialog = True) then
      begin
         { Confirm sprite resize. }
         if (g_DialogLockID = DIALOG_WARN_RESIZE) then
         begin
            if (keyRead = 'y') then
            begin
               InitNewSpriteSize(g_NewSpriteSize);
            end;
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Confirm successful save. }
         if (g_DialogLockID = DIALOG_SAVE_OK) then
         begin
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Confirm revert. }
         if (g_DialogLockID = DIALOG_WARN_REVERT) then
         begin
            if (keyRead = 'y') then
            begin
               LoadSpriteListFromFile(status);
            end;
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Confirm exit. }
         if (g_DialogLockID = DIALOG_WARN_QUIT) then
         begin
            if (keyRead = 'y') then
            begin
               g_ExitProgram := True;
            end
            else begin
               g_LockInputFromDialog := False;
               g_DialogLockID := -1;
               g_RedrawAll := True;
               DrawUI;
            end;
         end;

         { Confirm sprite add failed. }
         if (g_DialogLockID = DIALOG_WARN_ADD_FAILED) then
         begin
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Confirm warn of sprite clear. }
         if (g_DialogLockID = DIALOG_WARN_CLEAR) then
         begin
            if(keyRead = 'y') then
            begin
                ClearSpriteData(g_currentSprite);
            end;
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Check to see if the user wants more help or to return to the
           program.  Check the help pages in reverse order, so that the
           user wanting to see more help doesn't cause us to fall through
           to the next if statement.  I don't want to make this code a huge
           if/else block, and I'm too lazy to look up Pascal case statements,
           so there. }
         if (g_DialogLockID = DIALOG_HELP_PAGE_3) then
         begin
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Check to see if the user wants more help or to return to the
           program. }
         if (g_DialogLockID = DIALOG_HELP_PAGE_2) then
         begin
            if (keyRead = #27) then
            begin
               g_LockInputFromDialog := False;
               g_DialogLockID := -1;
               g_RedrawAll := True;
               DrawUi;
            end
            else begin
               g_LockInputFromDialog := True;
               g_DialogLockID := DIALOG_HELP_PAGE_3;
               DrawDialogBox(DIALOG_HELP_PAGE_3);
            end;
         end;

         if (g_DialogLockID = DIALOG_WARN_DELETE) then
         begin
            if(keyRead = 'y') then
            begin
               DeleteSpriteAtIndex(g_currentSpriteIdx);
            end;
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

         { Check to see if the user wants more help or to return to the
           program. }
         if (g_DialogLockID = DIALOG_HELP_PAGE_1) then
         begin
            if (keyRead = #27) then
            begin
               g_LockInputFromDialog := False;
               g_DialogLockID := -1;
               g_RedrawAll := True;
               DrawUi;
            end
            else begin
               g_LockInputFromDialog := True;
               g_DialogLockID := DIALOG_HELP_PAGE_2;
               DrawDialogBox(DIALOG_HELP_PAGE_2);
            end;
         end;

         { Confirm successful export. }
         if (g_DialogLockID = DIALOG_EXPORT_OK) then
         begin
            g_LockInputFromDialog := False;
            g_DialogLockID := -1;
            g_RedrawAll := True;
            DrawUi;
         end;

      end
      else begin
         { Check for quit. }
         if (keyRead = #27) then
         begin
            g_LockInputFromDialog := True;
            g_DialogLockID := DIALOG_WARN_QUIT;
            DrawDialogBox(DIALOG_WARN_QUIT);
         end;

         { Check for help. }
         if (keyRead = ';') and (extended = True) then
         begin
            g_LockInputFromDialog := True;
            g_DialogLockID := DIALOG_HELP_PAGE_1;
            DrawDialogBox(DIALOG_HELP_PAGE_1);
         end;

         { Toggle grid mode }
         if (keyRead = 'g') and (extended = False) then begin
            if(g_ShowGrid = True) then g_ShowGrid := False
                                  else g_ShowGrid := True;
            g_DrawEditWindow := True;
            g_DrawTogglesBox := True;
            DrawUi;
         end;

         { Toggle transparency display }
         if (keyRead = 'h') and (extended = False) then begin
            if(g_ShowTrans = True) then g_ShowTrans := False
                                else g_ShowTrans := True;
            g_DrawEditWindow := True;
            g_DrawTogglesBox := True;
            DrawUi;
         end;

         { Cycle through the color palette }
         if (keyRead = '[') and (extended = False) then begin
            g_ActiveColor := g_ActiveColor - 1;
            if (g_ActiveColor < 0) then g_ActiveColor := 15;
            g_DrawColorBox := True;
            DrawUi;
         end;
         if (keyRead = ']') and (extended = False) then begin
            g_ActiveColor := g_ActiveColor + 1;
            if (g_ActiveColor > 15) then g_ActiveColor := 0;
            g_DrawColorBox := True;
            DrawUi;
         end;

         { Move the cursor in the edit window UP }
         if (keyRead = #72) and (extended = True) then begin
            g_EditYPos := g_EditYPos - 1;
            if (g_EditYPos < 0) then g_EditYPos := (g_SpriteSize - 1);
            g_DrawEditWindow := True;
            DrawUi;
         end;

         { Move the cursor in the edit window DOWN }
         if (keyRead = #80) and (extended = True) then begin
            g_EditYPos := g_EditYPos + 1;
            if (g_EditYPos >= g_SpriteSize) then g_EditYPos := 0;
            g_DrawEditWindow := True;
            DrawUi;
         end;

         { Move the cursor in the edit window LEFT }
         if (keyRead = #75) and (extended = True) then begin
            g_EditXPos := g_EditXPos - 1;
            if (g_EditXPos < 0) then g_EditXPos := (g_SpriteSize - 1);
            g_DrawEditWindow := True;
            DrawUi;
         end;

         { Move the cursor in the edit window RIGHT }
         if (keyRead = #77) and (extended = True) then begin
            g_EditXPos := g_EditXPos + 1;
            if (g_EditXPos >= g_SpriteSize) then g_EditXPos := 0;
            g_DrawEditWindow := True;
            DrawUi;
         end;

         { Draw a pixel in the current edit location }
         if (keyRead = ' ') and (extended = False) then begin
            g_CurrentSprite^.colorData^[g_EditYPos * g_SpriteSize + g_EditXPos] := g_ActiveColor;
            g_CurrentSprite^.transData^[g_EditYPos * g_SpriteSize + g_EditXPos] := $00;
            g_DrawEditWindow := True;
            DrawUi;
            DrawActiveSpriteListEntry;
         end;

         { Erase a pixel in the current edit location }
         if (keyRead = 'x') and (extended = False) then begin
            g_CurrentSprite^.colorData^[g_EditYPos * g_SpriteSize + g_EditXPos] := $00;
            g_CurrentSprite^.transData^[g_EditYPos * g_SpriteSize + g_EditXPos] := $0f;
            g_DrawEditWindow := True;
            DrawUi;
            DrawActiveSpriteListEntry;
         end;

         { Create a new sprite (if memory permits) in the sprite sheet. }
         if (keyRead = 'n') and (extended = False) then begin
            AddEmptySprite;
            g_DrawSpriteList := True;
            DrawUi;
         end;

         { Delete the sprite at the specified cursor location. }
         if (keyRead = '\') and (extended = False) then begin
            g_LockInputFromDialog := True;
            g_DialogLockID := DIALOG_WARN_DELETE;
            DrawDialogBox(DIALOG_WARN_DELETE);
         end;

         { Cycle through to the next sprite in the sprite sheet. }
         if (keyRead = '.') and (extended = False) then begin
            if (g_currentSpriteIdx < g_TotalSprites - 1) then
            begin
               g_currentSpriteIdx := g_currentSpriteIdx + 1;
               GetPointerToSprite(g_currentSpriteIdx, g_currentSprite);
               g_DrawSpriteList := True;
               g_DrawEditWindow := True;
               DrawUi;
            end;
         end;

         { Cycle through to the previous sprite in the sprite sheet. }
         if (keyRead = ',') and (extended = False) then begin
            if (g_currentSpriteIdx > 0) then
            begin
               g_currentSpriteIdx := g_currentSpriteIdx - 1;
               GetPointerToSprite(g_currentSpriteIdx, g_currentSprite);
               g_DrawSpriteList := True;
               g_DrawEditWindow := True;
               DrawUi;
            end;
         end;

         { Clear the contents of the current sprite }
         if (keyRead = 'c') and (extended = False) then begin
            g_LockInputFromDialog := True;
            g_DialogLockID := DIALOG_WARN_CLEAR;
            DrawDialogBox(DIALOG_WARN_CLEAR);
         end;

         { Copy the contents of the current sprite into a temporary location.}
         if (keyRead = 'e') and (extended = False) then begin
            CopySpriteBuffer(g_currentSprite);
         end;

         { Paste the contents of the copy buffer into a temporary location.}
         if (keyRead = 'r') and (extended = False) then begin
            PasteSpriteBuffer(g_currentSprite);
            g_DrawEditWindow := True;
            DrawUi;
            DrawActiveSpriteListEntry;
         end;

         { Cycle through the valid sprite sizes.  Deletes all sprites in
           the sprite list.  Displays a dialog first. }
         if (keyRead = '3') or (keyRead = '5') or (keyRead = '7') or (keyRead = '9') then begin
            g_NewSpriteSize := ord(keyRead) - 48;
            g_LockInputFromDialog := True;
            g_DialogLockID := DIALOG_WARN_RESIZE;
            DrawDialogBox(DIALOG_WARN_RESIZE);
         end;

         { Save the current sprite list. }
         if (keyRead = '<') and (extended = True) then
         begin
            SaveSpriteListToFile(status, True);
            if (status = 0) then
            begin
                 g_LockInputFromDialog := True;
                 g_DialogLockID := DIALOG_SAVE_OK;
                 DrawDialogBox(DIALOG_SAVE_OK);
            end;
         end;

         { Export a headerless version of the current sprite list. }
         if (keyRead = '=') and (extended = True) then
         begin
            SaveSpriteListToFile(status, False);
            if (status = 0) then
            begin
                 g_LockInputFromDialog := True;
                 g_DialogLockID := DIALOG_EXPORT_OK;
                 DrawDialogBox(DIALOG_EXPORT_OK);
            end;
         end;

         { Reload the current sprite list. }
         if (keyRead = '>') and (extended = True) then
         begin
            g_LockInputFromDialog := True;
            g_DialogLockID := DIALOG_WARN_REVERT;
            DrawDialogBox(DIALOG_WARN_REVERT);
         end;
      end;
   end;
end;

begin
     { Set the default behavior for heap errors. }
     HeapError := @HeapFunc;

     { Initialize globals.  Set color to black, force a redraw of everything,
       and load the sprite data used for some of the screen elements. }
     g_ExitProgram := False;
     g_SpriteList := nil;
     g_CurrentSprite := nil;
     g_CopyBuffer := nil;
     g_TotalSprites := 0;
     g_CurrentSpriteIdx := -1;
     g_SpriteFileName := 'NEW.SPR';
     g_EditXPos := 0;
     g_EditYPos := 0;
     g_SpriteSize := 9;
     g_ActiveColor := 0;
     g_ShowGrid := False;
     g_ShowTrans := False;
     g_RedrawAll := True;
     g_LockInputFromDialog := False;
     g_DialogLockID := -1;

     g_SpriteTiles.init('LOCGAED');
     new(g_font, init('FONTS'));

     tileSource := g_SpriteTiles.dataStart;

     if (paramCount > 0) then begin
        g_SpriteFileName := Concat(paramStr(1), '.SPR');
        g_ExportFileName := Concat(paramStr(1), '.DAT');
     end;

     { Determine the initial size and position of the edit box. }
     CalculateEditWindowExtents;

     { Attempt to load the specified file.  If it doesn't exist, create a
       new empty sprite file and save it to disk (so reloads will work). }
     LoadSpriteListFromFile(g_status);
     if (g_status <> 0) then
     begin
        AddEmptySprite;
        g_CurrentSprite := g_SpriteList;
        g_CurrentSpriteIdx := 0;
        SaveSpriteListToFile(g_status, True);
     end;

     { Set up all the dialog definitions. }
     InitializeDialogBoxes;

     { Change into the 160x100 graphics mode and draw everything. }
     tg_init;

     DrawUI;

     while (g_ExitProgram = False) do
        ProcessInput;

     { Clean up and exit. }
     DeallocateDialogBoxes;
     DeleteAllSprites;

     tg_clear(0);
     tg_term;
end.
