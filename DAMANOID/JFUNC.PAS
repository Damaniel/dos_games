{
	JFUNC - Jason's TP7 Library

	Contains common functions and some CRT replacements...
	Which is nice given it means I don't have to patch CRT for that stupid
	overflow bug or bloat out my code with write handlers I'm not even going
	to use.
}

unit jfunc;

interface

const
	CRLF=#13+#10;
  continueString='Press any key to continue';

type
{
	a few pointer types I use a lot
}
	pChar=^char;
	pByte=^byte;
{
	I like to shorthand my string declarations too
}
	dosExt=string[3];
	dosName=string[8];
	st2=string[2];
	st4=string[4];
	st8=string[8];
	st10=string[10];
	st20=string[20];
	st40=string[40];
	st80=string[80];
	st255=string[255];

	pDynamicData=^tDynamicData;
	tDynamicData=object
		size:word;
		name:dosName;
		dataStart,
		dataEnd:pByte;
		constructor init(dataFile:dosName);
		destructor term;
	end;

function stackAvail:word;
function dosAvail:longint;
function strint(n:longint; digits:byte):st10;
function strLead(n:longint; digits:byte):st10;
function longToSt8(n:longint):st8;
function byte2Hex(v:byte):st2;
function word2Hex(v:word):st4;
function long2Hex(v:longint):st8;
function str2Upper(str:string):string;
function str2Lower(str:string):string;
function keypressed:boolean;
function readkey:char;
procedure flushKeyBuffer;
procedure waitkey;
procedure sWrite(str:string);
procedure writeText(section:string);
function paramExists(str:string):boolean;

implementation

constructor tDynamicData.init(dataFile:dosName);
var
	f:file;
begin
	name:=datafile;
	assign(f,name+'.DAT');
	reset(f,1);
	size:=filesize(f);
	getmem(dataStart,size);
	dataEnd:=dataStart;
	inc(dataEnd,size);
	blockread(f,dataStart^,size);
	close(f);
end;

destructor tDynamicData.term;
begin
	freemem(dataStart,size);
end;


{
	memory functions that SHOULD have been included in the
	system library for Turbo IMHO
}

function stackAvail:word; assembler;
asm
	mov  ax,sp
end;

function dosAvail:longint; assembler;
asm
	mov  ax,$4800
	mov  bx,$FFFF { 1 meg, impossible }
	int  $21
	xor  dx,dx
	mov  ax,bx
	mov  bx,16
	mul  bx
end;

procedure wordToESDI; assembler;
asm
@loop:
	xor  dx,dx
	div  bx
	xchg al,dl
	or   al,$30
	stosb
	mov  al,dl
	loop @loop
end;

function longToSt8(n:longint):st8; assembler;
asm
	les  di,@result
	mov  ax,$0008
	mov  es:[di],al
	add  di,ax
	mov  dx,word ptr n+2
	mov  ax,word ptr n
	mov  bx,10000
	div  bx { ax=high 0..9999, dx=low 0..9999 }
	mov  si,ax
	mov  ax,dx
	mov  cx,4
	mov  bx,10
	std
	call wordToESDI
	mov  ax,si
	mov  cx,4
	call wordToESDI
	cld
end;

function strint(n:longint; digits:byte):st10;
var
  st:string;
begin
  str(n:digits,st);
  strint:=st;
end;

{ Typically I use strLead to format score totals in games }

function strLead(n:longint; digits:byte):st10;
var
	st:string;
	ln:longint;
	t:byte;
begin
	st[0]:=chr(digits);
	ln:=n;
	for t:=digits downto 1 do begin
		st[t]:=chr((n mod 10)+48);
		n:=n div 10;
	end;
	strLead:=st;
end;

procedure byteHex; near; assembler;
{
	parameters
		bx:$9040
		cx:$0F04
		dh:byte to encode
		es:di:pointer to buffer
}

asm
	mov  al,dh
	and  al,ch
	or   al,bh
	daa
	adc  al,bl
	daa
	mov  ah,al
	mov  al,dh
	shr  al,cl
	add  al,bh
	daa
	adc  al,bl
	daa
	stosw
end;

function byte2Hex(v:byte):st2; assembler;
asm
	les  di,@result
	mov  al,$02
	stosb
	mov  bx,$9040
	mov  cx,$0F04
	mov  dh,v
	call byteHex
end;

function word2Hex(v:word):st4; assembler;
asm
	les  di,@result
	mov  al,$04
	stosb
	mov  bx,$9040
	mov  cx,$0F04
	mov  dx,v
	call byteHex
	mov  dh,dl
	call byteHex
end;

function long2Hex(v:longint):st8; assembler;
asm
	les  di,@result
	mov  al,$08
	stosb
	mov  bx,$9040
	mov  cx,$0F04
	mov  dx,WORD PTR v+2
	call byteHex
	mov  dh,dl
	call byteHex
	mov  dx,WORD PTR v
	call byteHex
	mov  dh,dl
	call byteHex
end;

procedure strConvert; near; assembler;
asm
	{
		strConvert loosely based on the str2Upper inline
		assembler example in the TP7 manual.

		BH=start character
		BL=end character
		CH=amount
		DS:SI=input string
		ES:DI=output string

		for example:
			BX = $617A, CH = -$20 == a..z becomes A..Z
			BX = $415A, CH =  $20 == A..Z becomes a..z

	}
	cld
	lodsb
	stosb
	xor  ah,ah
	xchg cx,ax
	jcxz @noAction
@loop:
	lodsb
	cmp  al,bh
	jb   @noChange
	cmp  al,bl
	ja   @noChange
	add  al,ah
@noChange:
	stosb
	loop @loop
@noAction:

end; {strConvert}

function str2Upper(str:string):string; assembler;
asm
	mov  bx,$617A
	mov  ch,-$20
	mov  dx,ds
	les  di,@result
	lds  si,str
	call strConvert
	mov  ds,dx
end;

function str2Lower(str:string):string; assembler;
asm
	mov  bx,$415A
	mov  ch,$20
	mov  dx,ds
	les  di,@result
	lds  si,str
	call strConvert
	mov  ds,dx
end;

{
	Calling DOS for keyboard handling, avoids headaches trying
	to support the PcJr/Tandy1K as well as the differences between
	AT and XT on the keymaps.
}

function readkey:char; assembler;
asm
	mov  ah,$07
	int  $21
end;

function keypressed:boolean; assembler;
asm
	mov  ah,$0B
	int  $21
end;

procedure flushKeyBuffer; assembler;
asm
@loop:
	mov  ah,$0B
	int  $21
	xor  al,al
	jz   @done
	mov  ah,$07
	int  $21
	loop @loop
@done:
end;

procedure waitkey; assembler;
asm
	call flushKeyBuffer
	mov  ah,$07
	int  $21
end;

procedure sWrite(str:string); assembler;
asm
	push ds
	lds  si,str
	xor  ch,ch
	lodsb
	mov  cl,al
	mov  ah,$02
@loop:
	lodsb
	mov  dl,al
	int  $21 { call DOS instead of BIOS, slower but better compatibility }
	loop @loop
	pop  ds
end;

procedure writeText(section:string);
var
	f:text;
	tst:st80;
	compareSt:st20;
	t:word;
begin
	assign(f,'TEXTS.DAT');
	reset(f);
	compareSt:='@'+str2Lower(section);
	repeat
		readln(f,tst);
	until eof(f) or (tst=compareSt);
	if not(eof(f)) then repeat
		readln(f,tst);
		if not(tst='@end') then begin
			if (tst='****') then begin
				sWrite(continueString);
				waitkey;
				for t:=1 to length(continueString) do write(#8,#32,#8);
			end else sWrite(tst+CRLF);
		end;
	until eof(f) or (tst='@end');
	close(f);
end;

function paramExists(str:string):boolean;
var
	tst:string;
	t:word;
	result:boolean;
begin
	tst:=str2Upper(str);
	for t:=1 to paramCount do begin
		if (str2Upper(paramStr(t))=tst) then begin
			paramExists:=true;
			exit;
		end
	end;
	paramExists:=false;
end;

end.