{ dialogs - a unit containing information about dialogs, and programatic
            descriptions of each dialog box used in the program.

            Most of this code was written October 2014 by
            Shaun Brandt / Holy Meatgoat Software.

            Uses Jason Knight's 160x100 CGA graphics library as well.
            See LOCGAED.PAS for more information about that.

            This code (along with all other source files in the project)
            are in the public domain.
}
unit dialogs;

interface
uses
   txtgraph;      { Needed for color definitions }
const
   DIALOG_WARN_RESIZE = 0;
   DIALOG_SAVE_OK = 1;
   DIALOG_WARN_REVERT = 2;
   DIALOG_WARN_QUIT = 3;
   DIALOG_WARN_ADD_FAILED = 4;
   DIALOG_WARN_CLEAR = 5;
   DIALOG_HELP_PAGE_1 = 6;
   DIALOG_HELP_PAGE_2 = 7;
   DIALOG_HELP_PAGE_3 = 8;
   DIALOG_WARN_DELETE = 9;
   DIALOG_EXPORT_OK = 10;
type
   { A pointer to a dialog box entry. }
   DialogPtr = ^Dialog;

   { A dialog box.  Contains color information, size information, and
     anywhere from 0 to 10 lines of text which can be individually positioned
     in the box.}
   Dialog = record
      innerColor:  Byte;
      borderColor: Byte;
      textColor:   Byte;
      xPos:        Integer;
      yPos:        Integer;
      width:       Integer;
      height:      Integer;
      numLines:    Integer;            { Number of lines of text used. }
      lines: Array[0..9] of String;
      lineX: Array[0..9] of Integer;
      lineY: Array[0..9] of Integer;
   end;

var
   { The 'global' dialog list.  Used in the main code to render dialogs as
     needed. }
   g_DialogList: array[0..10] of DialogPtr;

procedure InitializeDialogBoxes;
procedure DeallocateDialogBoxes;

implementation

{ InitializeDialogBoxes: Create each of the dialog boxes used in the program.
                         Someday, this may be saved out to a file and just
                         read back in.  For now, it's a bunch of
                         definitions in code.
}
procedure InitializeDialogBoxes;
var
   i: Integer;
begin

     { Null all pointers.  Used to make sure we don't deallocate unallocated
       dialog boxes later. }
     for i:=0 to 9 do
        g_DialogList[i] := nil;

     { Dialog 0: Warning about resizing sprites. }
     New(g_DialogList[0]);
     g_DialogList[0]^.innerColor := c_red;
     g_DialogList[0]^.borderColor := c_ltRed;
     g_DialogList[0]^.textColor := c_white;
     g_DialogList[0]^.xPos := 30;
     g_DialogList[0]^.yPos := 25;
     g_DialogList[0]^.width := 100;
     g_DialogList[0]^.height := 50;
     g_DialogList[0]^.lines[0] := '** WARNING **';
     g_DialogList[0]^.lineX[0] := 80;
     g_DialogList[0]^.lineY[0] := 28;
     g_DialogList[0]^.lines[1] := 'SELECTING A NEW SPRITE';
     g_DialogList[0]^.lineX[1] := 80;
     g_DialogList[0]^.lineY[1] := 40;
     g_DialogList[0]^.lines[2] := 'SIZE WILL DELETE *ALL*';
     g_DialogList[0]^.lineX[2] := 80;
     g_DialogList[0]^.lineY[2] := 47;
     g_DialogList[0]^.lines[3] := 'EXISTING SPRITE DATA!';
     g_DialogList[0]^.lineX[3] := 80;
     g_DialogList[0]^.lineY[3] := 54;
     g_DialogList[0]^.lines[4] := 'ARE YOU SURE? (Y/N)';
     g_DialogList[0]^.lineX[4] := 80;
     g_DialogList[0]^.lineY[4] := 67;
     g_DialogList[0]^.numLines := 5;

     { Dialog 1: Successful save. }
     New(g_DialogList[1]);
     g_DialogList[1]^.innerColor := c_blue;
     g_DialogList[1]^.borderColor := c_ltBlue;
     g_DialogList[1]^.textColor := c_white;
     g_DialogList[1]^.xPos := 30;
     g_DialogList[1]^.yPos := 35;
     g_DialogList[1]^.width := 100;
     g_DialogList[1]^.height := 30;
     g_DialogList[1]^.lines[0] := 'THE FILE WAS SAVED';
     g_DialogList[1]^.lineX[0] := 80;
     g_DialogList[1]^.lineY[0] := 38;
     g_DialogList[1]^.lines[1] := 'SUCCESSFULLY.';
     g_DialogList[1]^.lineX[1] := 80;
     g_DialogList[1]^.lineY[1] := 45;
     g_DialogList[1]^.lines[2] := 'PRESS ANY KEY.';
     g_DialogList[1]^.lineX[2] := 80;
     g_DialogList[1]^.lineY[2] := 57;
     g_DialogList[1]^.numLines := 3;

     { Dialog 2: Confirm revert to previous file. }
     New(g_DialogList[2]);
     g_DialogList[2]^.innerColor := c_red;
     g_DialogList[2]^.borderColor := c_ltRed;
     g_DialogList[2]^.textColor := c_white;
     g_DialogList[2]^.xPos := 30;
     g_DialogList[2]^.yPos := 25;
     g_DialogList[2]^.width := 100;
     g_DialogList[2]^.height := 50;
     g_DialogList[2]^.lines[0] := '** WARNING **';
     g_DialogList[2]^.lineX[0] := 80;
     g_DialogList[2]^.lineY[0] := 28;
     g_DialogList[2]^.lines[1] := 'REVERTING YOUR SAVE';
     g_DialogList[2]^.lineX[1] := 80;
     g_DialogList[2]^.lineY[1] := 40;
     g_DialogList[2]^.lines[2] := 'WILL CAUSE YOU TO LOSE';
     g_DialogList[2]^.lineX[2] := 80;
     g_DialogList[2]^.lineY[2] := 47;
     g_DialogList[2]^.lines[3] := 'ALL UNSAVED PROGRESS!';
     g_DialogList[2]^.lineX[3] := 80;
     g_DialogList[2]^.lineY[3] := 54;
     g_DialogList[2]^.lines[4] := 'ARE YOU SURE? (Y/N)';
     g_DialogList[2]^.lineX[4] := 80;
     g_DialogList[2]^.lineY[4] := 67;
     g_DialogList[2]^.numLines := 5;

     { Dialog 3: Confirm quit. }
     New(g_DialogList[3]);
     g_DialogList[3]^.innerColor := c_blue;
     g_DialogList[3]^.borderColor := c_ltBlue;
     g_DialogList[3]^.textColor := c_white;
     g_DialogList[3]^.xPos := 30;
     g_DialogList[3]^.yPos := 35;
     g_DialogList[3]^.width := 100;
     g_DialogList[3]^.height := 30;
     g_DialogList[3]^.lines[0] := 'ARE YOU SURE THAT YOU';
     g_DialogList[3]^.lineX[0] := 80;
     g_DialogList[3]^.lineY[0] := 38;
     g_DialogList[3]^.lines[1] := 'WANT TO QUIT?';
     g_DialogList[3]^.lineX[1] := 80;
     g_DialogList[3]^.lineY[1] := 45;
     g_DialogList[3]^.lines[2] := '(PRESS Y or N)';
     g_DialogList[3]^.lineX[2] := 80;
     g_DialogList[3]^.lineY[2] := 57;
     g_DialogList[3]^.numLines := 3;

     { Dialog 4: Unable to add new sprite to list. }
     New(g_DialogList[4]);
     g_DialogList[4]^.innerColor := c_red;
     g_DialogList[4]^.borderColor := c_ltRed;
     g_DialogList[4]^.textColor := c_white;
     g_DialogList[4]^.xPos := 30;
     g_DialogList[4]^.yPos := 25;
     g_DialogList[4]^.width := 100;
     g_DialogList[4]^.height := 50;
     g_DialogList[4]^.lines[0] := '** SORRY! **';
     g_DialogList[4]^.lineX[0] := 80;
     g_DialogList[4]^.lineY[0] := 28;
     g_DialogList[4]^.lines[1] := 'UNABLE TO ADD SPRITE.';
     g_DialogList[4]^.lineX[1] := 80;
     g_DialogList[4]^.lineY[1] := 40;
     g_DialogList[4]^.lines[2] := 'THIS MAY BE DUE TO LACK';
     g_DialogList[4]^.lineX[2] := 80;
     g_DialogList[4]^.lineY[2] := 47;
     g_DialogList[4]^.lines[3] := 'OF AVAILABLE MEMORY.';
     g_DialogList[4]^.lineX[3] := 80;
     g_DialogList[4]^.lineY[3] := 54;
     g_DialogList[4]^.lines[4] := '(PRESS ANY KEY.)';
     g_DialogList[4]^.lineX[4] := 80;
     g_DialogList[4]^.lineY[4] := 67;
     g_DialogList[4]^.numLines := 5;

     { Dialog 5: Confirm clear sprite. }
     New(g_DialogList[5]);
     g_DialogList[5]^.innerColor := c_blue;
     g_DialogList[5]^.borderColor := c_ltBlue;
     g_DialogList[5]^.textColor := c_white;
     g_DialogList[5]^.xPos := 30;
     g_DialogList[5]^.yPos := 30;
     g_DialogList[5]^.width := 100;
     g_DialogList[5]^.height := 40;
     g_DialogList[5]^.lines[0] := 'ARE YOU SURE THAT YOU';
     g_DialogList[5]^.lineX[0] := 80;
     g_DialogList[5]^.lineY[0] := 33;
     g_DialogList[5]^.lines[1] := 'WANT TO CLEAR THIS';
     g_DialogList[5]^.lineX[1] := 80;
     g_DialogList[5]^.lineY[1] := 40;
     g_DialogList[5]^.lines[2] := 'SPRITE?';
     g_DialogList[5]^.lineX[2] := 80;
     g_DialogList[5]^.lineY[2] := 47;
     g_DialogList[5]^.lines[3] := '(PRESS Y or N)';
     g_DialogList[5]^.lineX[3] := 80;
     g_DialogList[5]^.lineY[3] := 62;
     g_DialogList[5]^.numLines := 4;

     { Dialog 6: Help page 1 (key operations }
     New(g_DialogList[6]);
     g_DialogList[6]^.innerColor := c_blue;
     g_DialogList[6]^.borderColor := c_ltBlue;
     g_DialogList[6]^.textColor := c_white;
     g_DialogList[6]^.xPos := 3;
     g_DialogList[6]^.yPos := 15;
     g_DialogList[6]^.width := 154;
     g_DialogList[6]^.height := 80;
     g_DialogList[6]^.lines[0] := 'WELCOME TO LO-CGAEDIT, A SPRITE EDITOR';
     g_DialogList[6]^.lineX[0] := 80;
     g_DialogList[6]^.lineY[0] := 18;
     g_DialogList[6]^.lines[1] := 'DESIGNED FOR THE CGA 160x100 LO-RES';
     g_DialogList[6]^.lineX[1] := 80;
     g_DialogList[6]^.lineY[1] := 25;
     g_DialogList[6]^.lines[2] := 'GRAPHICS MODE.  USEFUL KEYS:';
     g_DialogList[6]^.lineX[2] := 80;
     g_DialogList[6]^.lineY[2] := 32;
     g_DialogList[6]^.lines[3] := '\CARROW KEYS\F - MOVE EDIT CURSOR';
     g_DialogList[6]^.lineX[3] := 80;
     g_DialogList[6]^.lineY[3] := 39;
     g_DialogList[6]^.lines[4] := '\CSPACE BAR\F - DRAW PIXEL AT CURSOR';
     g_DialogList[6]^.lineX[4] := 80;
     g_DialogList[6]^.lineY[4] := 52;
     g_DialogList[6]^.lines[5] := '\CX\F - DELETE PIXEL AT CURSOR';
     g_DialogList[6]^.lineX[5] := 80;
     g_DialogList[6]^.lineY[5] := 59;
     g_DialogList[6]^.lines[6] := '\C[\F / \C]\F - CYCLE THROUGH PALETTE';
     g_DialogList[6]^.lineX[6] := 80;
     g_DialogList[6]^.lineY[6] := 66;
     g_DialogList[6]^.lines[7] := '\CC\F - CLEAR CURRENT SPRITE';
     g_DialogList[6]^.lineX[7] := 80;
     g_DialogList[6]^.lineY[7] := 73;
     g_DialogList[6]^.lines[8] := 'PRESS ESC TO LEAVE, ANY KEY FOR MORE';
     g_DialogList[6]^.lineX[8] := 80;
     g_DialogList[6]^.lineY[8] := 86;
     g_DialogList[6]^.numLines := 9;

     { Dialog 7: Help page 2 (more key operations) }
     New(g_DialogList[7]);
     g_DialogList[7]^.innerColor := c_blue;
     g_DialogList[7]^.borderColor := c_ltBlue;
     g_DialogList[7]^.textColor := c_white;
     g_DialogList[7]^.xPos := 3;
     g_DialogList[7]^.yPos := 15;
     g_DialogList[7]^.width := 154;
     g_DialogList[7]^.height := 80;
     g_DialogList[7]^.lines[0] := '\CN\F - ADD A NEW SPRITE TO THE SHEET';
     g_DialogList[7]^.lineX[0] := 80;
     g_DialogList[7]^.lineY[0] := 24;
     g_DialogList[7]^.lines[1] := '\C,\F / \C.\F - CYCLE THROUGH SPRITES';
     g_DialogList[7]^.lineX[1] := 80;
     g_DialogList[7]^.lineY[1] := 31;
     g_DialogList[7]^.lines[2] := '\C\\\F - DELETE SELECTED SPRITE';
     g_DialogList[7]^.lineX[2] := 80;
     g_DialogList[7]^.lineY[2] := 38;
     g_DialogList[7]^.lines[3] := ' \C3 5 7 9\F - SET SPRITE SIZE IN PIXELS';
     g_DialogList[7]^.lineX[3] := 80;
     g_DialogList[7]^.lineY[3] := 45;
     g_DialogList[7]^.lines[4] := '\CE\F - COPY CURRENT SPRITE TO BUFFER';
     g_DialogList[7]^.lineX[4] := 80;
     g_DialogList[7]^.lineY[4] := 52;
     g_DialogList[7]^.lines[5] := '\CR\F - PASTE SPRITE FROM BUFFER';
     g_DialogList[7]^.lineX[5] := 80;
     g_DialogList[7]^.lineY[5] := 59;
     g_DialogList[7]^.lines[6] := '\CESC\F - EXIT LOCGAED';
     g_DialogList[7]^.lineX[6] := 80;
     g_DialogList[7]^.lineY[6] := 66;
     g_DialogList[7]^.lines[7] := '\CG\F - TOGGLE GRID MODE';
     g_DialogList[7]^.lineX[7] := 80;
     g_DialogList[7]^.lineY[7] := 73;
     g_DialogList[7]^.lines[8] := 'PRESS ESC TO LEAVE, ANY KEY FOR MORE';
     g_DialogList[7]^.lineX[8] := 80;
     g_DialogList[7]^.lineY[8] := 86;
     g_DialogList[7]^.numLines := 9;

     { Dialog 8: Help page 3 (final parts) }
     New(g_DialogList[8]);
     g_DialogList[8]^.innerColor := c_blue;
     g_DialogList[8]^.borderColor := c_ltBlue;
     g_DialogList[8]^.textColor := c_white;
     g_DialogList[8]^.xPos := 3;
     g_DialogList[8]^.yPos := 15;
     g_DialogList[8]^.width := 154;
     g_DialogList[8]^.height := 80;
     g_DialogList[8]^.lines[0] := '\CH\F - MARK TRANSPARENT PIXELS';
     g_DialogList[8]^.lineX[0] := 80;
     g_DialogList[8]^.lineY[0] := 22;
     g_DialogList[8]^.lines[1] := '\CF2\F - SAVE SPRITE SHEET TO FILE';
     g_DialogList[8]^.lineX[1] := 80;
     g_DialogList[8]^.lineY[1] := 29;
     g_DialogList[8]^.lines[2] := '\CF3\F - EXPORT WITH NO HEADER';
     g_DialogList[8]^.lineX[2] := 80;
     g_DialogList[8]^.lineY[2] := 36;
     g_DialogList[8]^.lines[3] := '\CF4\F - REVERT TO SAVED FILE';
     g_DialogList[8]^.lineX[3] := 80;
     g_DialogList[8]^.lineY[3] := 43;
     g_DialogList[8]^.lines[4] := 'LO-CGAEDIT V1.0 - OCTOBER 2014';
     g_DialogList[8]^.lineX[4] := 80;
     g_DialogList[8]^.lineY[4] := 54;
     g_DialogList[8]^.lines[5] := 'SHAUN BRANDT / HOLY MEATGOAT SOFTWARE';
     g_DialogList[8]^.lineX[5] := 80;
     g_DialogList[8]^.lineY[5] := 61;
     g_DialogList[8]^.lines[6] := '(\AHTTP://WWW.HOLYMEATGOAT.COM\F)';
     g_DialogList[8]^.lineX[6] := 80;
     g_DialogList[8]^.lineY[6] := 68;
     g_DialogList[8]^.lines[7] := 'THIS CODE IS PUBLIC DOMAIN!';
     g_DialogList[8]^.lineX[7] := 80;
     g_DialogList[8]^.lineY[7] := 75;
     g_DialogList[8]^.lines[8] := '** PRESS ANY KEY TO LEAVE HELP **';
     g_DialogList[8]^.lineX[8] := 80;
     g_DialogList[8]^.lineY[8] := 86;
     g_DialogList[8]^.numLines := 9;

     { Dialog 9: Confirm delete sprite. }
     New(g_DialogList[9]);
     g_DialogList[9]^.innerColor := c_blue;
     g_DialogList[9]^.borderColor := c_ltBlue;
     g_DialogList[9]^.textColor := c_white;
     g_DialogList[9]^.xPos := 30;
     g_DialogList[9]^.yPos := 30;
     g_DialogList[9]^.width := 100;
     g_DialogList[9]^.height := 40;
     g_DialogList[9]^.lines[0] := 'ARE YOU SURE THAT YOU';
     g_DialogList[9]^.lineX[0] := 80;
     g_DialogList[9]^.lineY[0] := 33;
     g_DialogList[9]^.lines[1] := 'WANT TO DELETE THIS';
     g_DialogList[9]^.lineX[1] := 80;
     g_DialogList[9]^.lineY[1] := 40;
     g_DialogList[9]^.lines[2] := 'SPRITE FROM THE LIST?';
     g_DialogList[9]^.lineX[2] := 80;
     g_DialogList[9]^.lineY[2] := 47;
     g_DialogList[9]^.lines[3] := '(PRESS Y or N)';
     g_DialogList[9]^.lineX[3] := 80;
     g_DialogList[9]^.lineY[3] := 62;
     g_DialogList[9]^.numLines := 4;

     { Dialog 10: Successful export. }
     New(g_DialogList[10]);
     g_DialogList[10]^.innerColor := c_blue;
     g_DialogList[10]^.borderColor := c_ltBlue;
     g_DialogList[10]^.textColor := c_white;
     g_DialogList[10]^.xPos := 30;
     g_DialogList[10]^.yPos := 35;
     g_DialogList[10]^.width := 100;
     g_DialogList[10]^.height := 30;
     g_DialogList[10]^.lines[0] := 'THE FILE WAS EXPORTED';
     g_DialogList[10]^.lineX[0] := 80;
     g_DialogList[10]^.lineY[0] := 38;
     g_DialogList[10]^.lines[1] := 'SUCCESSFULLY.';
     g_DialogList[10]^.lineX[1] := 80;
     g_DialogList[10]^.lineY[1] := 45;
     g_DialogList[10]^.lines[2] := 'PRESS ANY KEY.';
     g_DialogList[10]^.lineX[2] := 80;
     g_DialogList[10]^.lineY[2] := 57;
     g_DialogList[10]^.numLines := 3;
end;

{ DeallocateDialogBoxes:  Deallocates all memory used by the dialog boxes. }
procedure DeallocateDialogBoxes;
var
   i: Integer;
begin
   for i:= 0 to 10 do
   begin
      if(g_DialogList[i] <> nil) then
         dispose(g_DialogList[i]);
   end;
end;

end.
