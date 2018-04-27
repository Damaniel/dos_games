unit txtGraph;

interface

uses
	jfunc;

const
	videoCard_mda=0;
	videoCard_cga=1;
	videoCard_pcJr=2;
	videoCard_tandy1000=3;
	videoCard_tandySLTL=4;
	videoCard_ega=5;
	videoCard_vga=6;
	videoCard_mcga=7;
	videoCardName:packed array[0..videoCard_mcga] of string[11]=(
		'MDA','CGA','PCJr','Tandy 1000','Tandy TL/SL','EGA','VGA','MCGA'
	);

	c_black       = $00;
	c_blue        = $01;
	c_green       = $02;
	c_cyan        = $03;
	c_red         = $04;
	c_magenta     = $05;
	c_brown       = $06;
	c_ltGrey      = $07;
	c_dkGrey      = $08;
	c_ltBlue      = $09;
	c_ltGreen     = $0A;
	c_ltCyan      = $0B;
	c_ltRed       = $0C;
	c_ltMagenta   = $0D;
	c_yellow      = $0E;
	c_white       = $0F;
	c_transparent = $80; { fontSet.outText functions bgColor only }

type

	writeTileProc=procedure(x,y,tile:byte);
	backgroundProc=procedure(x,y:byte; data:pByte);
	bufferShowProc=procedure(x,y:byte);

	pFontData=^tFontData;
	tFontData=array[0..127,0..7] of byte;
	tKernData=array[0..127] of byte;
	tWriteCharProc=procedure(x,y,chr,color,bgColor:byte; data:pFontData);

	pfontSet=^fontSet;
	fontSet=object
		fontData:tFontData;
		kernData:tKernData;
		kernSlash:word;
		bgColor:byte;
		writeProc:tWriteCharProc;
		constructor init(dataFile:dosName);
		function textLength(st:st40):word;
		procedure setBackground(color:byte);
		procedure outText(x,y:byte; st:st40; color:byte);
		procedure outTextRight(x,y:byte; st:st40; color:byte);
		procedure outTextCentered(x,y:byte; st:st40; color:byte);
	end;

	pSprite=^tSprite;
	tSprite=object
		{ these values the user can change directly }
		currentX,currentY,currentTile,
		bufferX,bufferY:integer;

		{ from here out this should be left to the code to manage }
		oldX,oldY,sTileSize:integer;

		{ function handlers, prevents needing a 'case' statement inside the loop }
		bufferWriteTile:writeTileProc;
		bufferShow:bufferShowProc;

	  { pointers for list control }
		next:pSprite;

		constructor init(x,y:byte; tile,tileSize:integer);
		procedure copySourceDestOld;
		procedure copySourceDestCurrent;
		procedure bufShow;
		procedure bufWriteTile;
		procedure updateThenBufShow;


{*********************************************************}

		destructor term;
	end;

	pUpdateBlock=^updateBlock;
	updateBlock=object
		x,y:byte;
		update:boolean;
		next:pUpdateBlock;
		constructor init(ix,iy:integer);
		procedure updateToScreen;
		procedure updateToDest;
		destructor term;
	end;

	updateBlockList=object
		first,last:pUpdateBlock;
		constructor init;
		procedure add(ix,iy:integer);
		destructor term;
	end;

	pPackedBuffer=^tPackedBuffer;
	tPackedBuffer=array[0..7999] of byte;


var
	tileSource:pointer;
	sourceBuffer,destBuffer,
	backgroundBuffer,renderBuffer:pPackedBuffer;
	updateList:updateBlockList;
	videoCard:word;

procedure tg_init;
procedure tg_term;

procedure tg_clear(color:byte);

procedure tg_putPixel(x,y,color:byte);
function tg_getPixel(x,y:byte):byte;

procedure tg_bar(sx,sy,ex,ey:integer; color:byte);
procedure tg_rectangle(sx,sy,ex,ey:integer; color:byte);

procedure tg_tile3(x,y,tile:byte);
procedure tg_tile5(x,y,tile:byte);

procedure tg_writeChar(x,y,chr,color,bgColor:byte; data:pFontData);
procedure tg_writeCharBg(x,y,digit,color,bgColor:byte; data:pFontData);

procedure tg_waitRetrace;

function buffer_addSprite(x,y,tile,tileSize:integer):pSprite;

procedure buffer_sourceBackground; { also sets dest to render }
procedure buffer_sourceRender;     { also sets dest to background }
procedure buffer_sourceClear;
procedure buffer_copySource2Dest;
procedure buffer_copyDest2Screen;
procedure buffer_copySourceDest8x8(x,y:byte); { source -> display, 8x8 byte-bound }
procedure buffer_show4x3(x,y:byte);           { dest -> display, 8x8 byte-bound }
procedure buffer_show8x6(x,y:byte);            { dest -> display, 8x8 byte-bound }
procedure buffer_tile3(x,y,tile:byte);  { tile -> dest }
procedure buffer_tile5(x,y,tile:byte);  { tile -> dest }
procedure buffer_nullTile4x3(x,y:byte);        { erase source buffer section }
procedure buffer_hideSprites;

procedure buffer_writeSpritesToBackBuffer;
procedure buffer_copySpritesToScreen;
procedure buffer_eraseSpritesFromBuffer;

procedure buffer_destPutPixel(x,y,color:byte);
function buffer_sourceGetPixel(x,y:byte):byte;


implementation

type

	regData=packed record
		register:word;
		data:string[26];
	end;

{
	Why store regData.data in a string? Because it gives
	us a size byte BEFORE the content in the data stream,
	and it's the only dynamic size data type you can use
	in a constant.

	Too bad we can't just read this crap from the EGA/VGA
	data tables, but for some reason DosBox doesn't provide
	those... and it would be nice to have this work on real
	hardware AND dosBox.
}

const
	pageSize=$3E80;
	pageSizeWords=pageSize shr 1;

	CGA_Regs:regData=(
		register:$3D4;
		data:#04+#$7F + #06+#$64 + #07+#$70 + #09+#$01
	);

	EGA_Sequencer:regData=(
		register:$3C4;
		data:
			#03+#01+#03+#00 + #03
	);
	EGA_CRTC:regData=(
		register:$3D4;
		data:
			#$70+#$4F+#$5C+#$2F + #$5F+#$07+#$04+#$11 +
			#$00+#$01+#$06+#$07 + #$00+#$00+#$00+#$00 +
			#$E1+#$24+#$C7+#$28 + #$08+#$E0+#$F0+#$A3 +
			#$FF+#$00
	);

var
	firstSprite,lastSprite:pSprite;
	textSegment:word;
	oldMode:byte;
	oldTgExitProc:pointer;

	safeMode,
	textGraphOn:boolean;

	tg_sx,tg_sy,tg_ex,tg_ey:integer;

	pCGA_Regs,
	pEGA_Sequencer,
	pEGA_CRTC:pointer;

constructor updateBlock.init(ix,iy:integer);
begin
	x:=ix;
	y:=iy;
	update:=false;
	next:=nil;
end;

procedure updateBlock.updateToScreen;
begin
	buffer_Show4x3(x,y);
	if not(next=nil) then next^.updateToScreen;
end;

procedure updateBlock.updateToDest;
begin
	buffer_copySourceDest8x8(x,y);
	if not(next=nil) then next^.updateToDest;
end;

destructor updateBlock.term;
begin
	if not(next=nil) then dispose(next,term);
end;

constructor updateBlockList.init;
begin
	first:=nil;
end;

procedure updateBlockList.add(ix,iy:integer);
begin
	if (first=nil) then begin
		new(first,init(ix,iy));
		last:=first;
	end else begin
		new(last^.next,init(ix,iy));
		last:=last^.next;
	end;
end;

destructor updateBlockList.term;
begin
	if not(first=nil) then dispose(first,term);
end;

constructor tSprite.init(x,y:byte; tile,tileSize:integer);
begin
	currentX:=x;
	oldX:=x;
	currentY:=y;
	oldY:=y;
	currentTile:=tile;
	sTileSize:=tileSize;
	case sTileSize of
		3:begin
			bufferWriteTile:=buffer_tile3;
			bufferShow:=buffer_show4x3;
		end;
		5:begin
			bufferWriteTile:=buffer_tile5;
			bufferShow:=buffer_show8x6;
		end;
	end;
	next:=nil;
end;

procedure tSprite.copySourceDestOld;
begin
	buffer_copySourceDest8x8(oldX,oldY);
	if not(next=nil) then next^.copySourceDestOld;
end;

procedure tSprite.copySourceDestCurrent;
begin
	buffer_copySourceDest8x8(currentX,currentY);
	if not(next=nil) then next^.copySourceDestCurrent;
end;

procedure tSprite.bufShow;
begin
	bufferShow(oldX,oldY);
	if not(next=nil) then next^.bufShow;
end;

procedure tSprite.bufWriteTile;
begin
	bufferWriteTile(currentX,currentY,currentTile);
	if not(next=nil) then next^.bufWriteTile;
end;

procedure tSprite.updateThenBufShow;
begin
	if currentX<oldX then bufferX:=currentX else bufferX:=oldX;
	oldX:=currentX;
	if currentY<oldY then bufferY:=currentY else bufferY:=oldY;
	oldY:=currentY;
	bufferShow(bufferX,bufferY);
	if not(next=nil) then next^.updateThenBufShow;
end;

destructor tSprite.term;
begin
	if not(next=nil) then dispose(next,term);
end;

function buffer_addSprite(x,y,tile,tileSize:integer):pSprite;
begin
	if firstSprite=nil then begin
		new(firstSprite,init(x,y,tile,tileSize));
		lastSprite:=firstSprite;
	end else begin
		new(lastSprite^.next,init(x,y,tile,tileSize));
		lastSprite:=lastSprite^.next;
	end;
	buffer_addSprite:=lastSprite;
end;

procedure discardSprites;
begin
	if not(firstSprite=nil) then dispose(firstSprite,term);
end;

constructor fontSet.init(dataFile:dosName);
var
	f:file;
begin
	assign(f,dataFile+'.DAT');
	reset(f,1);
	blockread(f,fontData,sizeof(tFontData));
	blockread(f,kernData,sizeof(tKernData));
	close(f);
	kernSlash:=kernData[ord('\')];
	setBackground(c_transparent);
end;

procedure fontSet.setBackground(color:byte);
var
	p:pointer;
begin
	bgColor:=color;
	if bgColor=c_transparent then p:=@tg_writeChar else p:=@tg_writeCharBg;
	writeProc:=tWriteCharProc(p);
end;

function fontSet.textLength(st:st40):word;
var
	size,t:word;
	b:byte;
begin
	size:=0;
	t:=0;
	while (t<length(st)) do begin
		inc(t);
		b:=ord(st[t]);
		if (b=$5C) then begin
			inc(t);
			if (st[t]='\') then inc(size,kernSlash);
		end else begin
			inc(size,kernData[b and $7F]);
		end;
	end;
	textLength:=size;
end;

procedure fontSet.outText(x,y:byte; st:st40; color:byte);
var
	b:byte;
	t:word;
label
	writeit;
begin
	t:=0;
	while (t<length(st)) do begin
		inc(t);
		b:=ord(st[t]);
		if b=$5C then begin
			inc(t);
			b:=ord(st[t]);
			if b=$5C then goto writeit;
			case chr(b) of
				'0'..'9':color:=b and $0F;
				'A'..'F':color:=b-55;
				'a'..'f':color:=b-87;
			end;
		end	else begin
writeit:
			writeProc(x,y,b,color,bgColor,@fontData);
			inc(x,kernData[b and $7F]);
		end;
	end;
end;

procedure fontSet.outTextCentered(x,y:byte; st:st40; color:byte);
begin
	outText(x-textLength(st) div 2,y,st,color);
end;

procedure fontSet.outTextRight(x,y:byte; st:st40; color:byte);
begin
	outText(x-textLength(st),y,st,color);
end;

procedure tg_waitRetrace; assembler;
asm
	mov   dx,$3DA
	mov   ah,$08
	mov   cx,$1FFF
@loop:
	in    al,dx
	and   al,ah
	loopz @loop
end;

{
	Detecting which video card is present is kinda tricky...
	but thankfully they did something smart with int $10.
	Calls to unknown subfunctions just RET leaving registers
	intact, so if you call a VGA function that you know changes
	a register, and the register doesn't change, it's not a VGA.
	Call a EGA function ditto, ditto... finally check if we're in
	a monochrome display mode, that's MDA.

	Unfortunately there's no known reliable check for a CGA since
	newer cards pretend to be one -- but if we eliminate
	'everything else' from the list, it must be CGA.
}

function detectCard:byte; assembler;
asm
	mov  ax,$1200
	mov  bl,$32       { VGA only enable video }
	int  $10
	cmp  al,$12       { VGA returns $12, all others leave it unmodified! }
	jne  @notVGA      { not a vga, test for EGA }
	                  { VGA, or is it? test for MCGA }
	xor  bl,bl        { null BL so it's set up for non-PS/2 }
	mov  ax,$1A00
	int  $10
	cmp  bl,$0A       { MCGA returns $0A..$0C }
	jb   @isVGA
	cmp  bl,$0C
	jg   @isVGA
	mov  al,videoCard_mcga
	ret
@isVGA:
	mov  al,videoCard_vga
	ret
@notVGA:           { We eliminated VGA, so an EGA/VGA true must be EGA }
	mov  ah,$12
	mov  bl,$10       { EGA/VGA get configuration info }
	int  $10
	and  bl,$03       { EGA/VGA returns a 0..3 value here }
	jz   @notEGA      { not a VGA, test for MDA }
	mov  al,videoCard_ega
	ret
@notEGA:            { MDA all we need to detect is video mode 7 }
	mov  ah,$0F       { get Video mode }
	int  $10
	cmp  al,$07
	jne  @notMDA
	mov  al,videoCard_mda
	ret
@notMDA:            { not MDA, check for Jr. }
	mov  ax,$FFFF
	mov  es,ax
	mov  di,$000E     { second to last byte PCjr/Tandy BIOS info area }
	mov  al,$FD       { ends up $FD only on the Jr. }
	cmp  es:[di],al
	jne  @notJr
	mov  al,videoCard_pcJr
	ret
@notJr:             { not junior, test for tandy }
	mov  al,$FF       { all tandy's return $FF here }
	cmp  es:[di],al
	jne  @notTandy
	mov  ax,$FC00
	mov  es,ax
	xor  di,di
	mov  al,$21
	cmp  es:[di],al
	jne  @notTandy
	mov  ah,$C0       { test for SL/TL }
	int  $15          { Get System Environment }
	jnc  @tandySLTL     { early Tandy's leave the carry bit set, TL/SL does not }
	mov  al,videoCard_tandy1000
	ret
@tandySLTL:
	mov  al,videoCard_tandySLTL
	ret
@notTandy:
	mov  al,videoCard_cga { all other cards eliminated, must be CGA }
end;

procedure tg_putPixel(x,y,color:byte); assembler;
asm
	xor  al,al
	mov  ah,y
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax

	xor  dh,dh
	mov  dl,x
	add  di,dx
	or   di,1

	mov  al,color
	mov  bx,textSegment
	mov  es,bx
	mov  bl,es:[di]

	and  dx,1
	jnz  @xOdd

	and  bl,$F0
	jmp  @xDone

@xOdd:
	and  bl,$0F
	mov  cl,4
	shl  al,cl

@xDone:
	or   al,bl
	mov  es:[di],al
end;

function tg_getPixel(x,y:byte):byte; assembler;
asm
	xor  al,al
	mov  ah,y
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax

	xor  dh,dh
	mov  dl,x
	add  di,dx
	or   di,1

	mov  bx,textSegment
	mov  es,bx
	mov  al,es:[di]

	and  dx,$01
	jz   @xEven

	mov  cl,4
	shr  al,cl

@xEven:
	and  al,$0F
end;

procedure buffer_destPutPixel(x,y,color:byte); assembler;
asm
	les  di,destBuffer
	xor  al,al
	mov  ah,y
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax

	xor  dh,dh
	mov  dl,x
	add  bx,dx
	shr  bx,1
	add  di,bx

	mov  al,color
	mov  bl,es:[di]

	and  dx,1
	jnz  @xOdd

	and  bl,$F0
	jmp  @xDone

@xOdd:
	and  bl,$0F
	mov  cl,4
	shl  al,cl

@xDone:
	or   al,bl
	mov  es:[di],al
end;

function buffer_sourceGetPixel(x,y:byte):byte; assembler;
asm
	les  di,sourceBuffer
	xor  al,al
	mov  ah,y
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax

	xor  dh,dh
	mov  dl,x
	add  bx,dx
	shr  bx,1
	add  di,bx

	mov  al,es:[di]

	and  dx,$01
	jz   @xEven

	mov  cl,4
	shr  al,cl

@xEven:
	and  al,$0F
end;


procedure hLine(x,y,disX:byte; color:byte); assembler;
asm
	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax

	xor  bh,bh
	mov  bl,x
	add  di,bx
	or   di,1

	mov  ax,textSegment
	mov  es,ax

	xor  dh,dh
	mov  dl,disX

	mov  bh,color
	mov  al,bh

	mov  cl,4
	shl  al,cl

	and  bl,1
	jz   @hLongRun

	mov  bl,es:[di]
	and  bl,$0F
	or   bl,al
	mov  es:[di],bl
	add  di,2
	dec  dx

@hLongRun:
	cmp  dx,2
	jl   @hFinalPixel
	mov  ah,al
	or   ah,bh
	mov  al,$DD
	mov  cx,dx
	shr  cx,1
	dec  di
	rep  stosw
	inc  di

@hFinalPixel:
	test dx,1
	jz   @hLineDone
	mov  al,es:[di]
	and  al,$F0
	or   al,bh
	mov  es:[di],al
@hLineDone:
end;

procedure vLine(x,y,disY:byte; color:byte); assembler;
asm
	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax
	xor  dh,dh
	mov  dl,x
	add  di,dx
	or   di,1

	mov  ax,textSegment
	mov  es,ax

	mov  bl,color
	mov  bh,$F0 {
		It's faster to set BH and then change it on condition
		than it is to add all sorts of test and jump logic to
		only set $F0 as needed
	}

	and  dx,1
	jz   @vSetDistance

	mov  cl,4
	shl  bl,cl
	mov  bh,$0F

@vSetDistance:
	xor  ch,ch
	mov  cl,disY

@vLineLoop:
	mov  al,es:[di]
	and  al,bh
	or   al,bl
	mov  es:[di],al
	add  di,$A0
	loop @vLineLoop
end;

procedure tg_bar(sx,sy,ex,ey:integer; color:byte);
var
	y,distance:integer;
begin
	distance:=(ex-sx);
	if (distance=0) then begin
		vLine(sx,sy,(ey-sy)+1,color);
	end else begin
		y:=sy;
		inc(distance);
		while (y<=ey) do begin
			hLine(sx,y,distance,color);
			inc(y);
		end;
	end;
end;

procedure tg_rectangle(sx,sy,ex,ey:integer; color:byte);
var
	distance:integer;
begin
	distance:=(ex-sx)+1;
	hLine(sx,sy,distance,color);
	hLine(sx,ey,distance,color);
	distance:=(ey-sy)+1;
	vLine(sx,sy,distance,color);
	vLine(ex,sy,distance,color);
end;

procedure tg_tile3(x,y,tile:byte); assembler;
asm
	{ set video address for STOSW }
	mov  ax,$B800
	mov  es,ax
	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax

	mov  cl,x
	xor  ch,ch
	add  di,cx
	or   di,1

	{ calculate tile offset }
	xor  ax,ax
	mov  al,tile
	shl  ax,1
	shl  ax,1
	shl  ax,1
	mov  bx,ax
	add  ax,bx
	add  ax,bx
	and  cx,1
	jnz  @oddtile
	add  ax,12
@oddTile:
	mov  dx,ds
	lds  si,tileSource
	add  si,ax

	mov  cx,3
	mov  bx,157

@tileLoop:

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb
	inc  di

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	add  di,bx

	loop @tileLoop

	mov ds,dx
end;

procedure tg_tile5(x,y,tile:byte); assembler;
asm
	{ set video address for STOSW }
	mov  ax,$B800
	mov  es,ax

	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax

	mov  cl,x
	xor  ch,ch
	add  di,cx
	or   di,1

	{ calculate tile offset }
	xor  ax,ax
	mov  al,tile
	mov  bx,60
	mul  bx
	and  cx,1
	jnz  @oddtile
	add  ax,30
@oddTile:
	mov  dx,ds
	lds  si,tileSource
	add  si,ax

	mov  cx,5
	mov  bx,155

@tileLoop:

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb
	inc di

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb
	inc di

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	add  di,bx

	loop @tileLoop

	mov ds,dx
end;

procedure tg_writeChar(x,y,chr,color,bgColor:byte; data:pFontData); assembler;
{ bgColor IGNORED! }
asm
	mov  dx,$B800
	mov  es,dx { es == vid segment }

	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax
	mov  cl,x
	xor  ch,ch
	mov  di,cx
	mov  ch,cl  { ch now holds x for later }
	add  di,bx
	or   di,1

	xor  ah,ah
	mov  al,chr
	shl  ax,1
	shl  ax,1
	shl  ax,1   { ax = chr * 8 }


	mov  bl,color
	and  bl,$0F
	mov  bh,bl
	mov  cl,4
	shl  bh,cl { bx == cccc:0000:0000:cccc }

	push ds
	lds  si,data
	add  si,ax  { now pointing at font map offset }

	mov  cl,ch
	and  cl,1   { amount to shift the bitplanes }
	mov  ch,$08
	mov  dx,$F00F

@loopRow:
	lodsb
	shr  al,cl { align to bitplane }
	mov  ah,al { move to ah so al is free for stosb's }

	mov  al,es:[di]
	rcl  ah,1
	jnc  @skip1
	and  al,dh
	or   al,bl
@skip1:
	rcl  ah,1
	jnc  @skip2
	and  al,dl
	or   al,bh
@skip2:
	stosb
	inc  di

	mov  al,es:[di]
	rcl  ah,1
	jnc  @skip3
	and  al,dh
	or   al,bl
@skip3:
	rcl  ah,1
	jnc  @skip4
	and  al,dl
	or   al,bh
@skip4:
	stosb
	inc  di

	mov  al,es:[di]
	rcl  ah,1
	jnc  @skip5
	and  al,dh
	or   al,bl
@skip5:
	rcl  ah,1
	jnc  @skip6
	and  al,dl
	or   al,bh
@skip6:
	stosb
	inc  di

	mov  al,es:[di]
	rcl  ah,1
	jnc  @skip7
	and  al,dh
	or   al,bl
@skip7:
	rcl  ah,1
	jnc  @skip8
	and  al,dl
	or   al,bh
@skip8:
	stosb
	add  di,153

	dec  ch
	jnz  @loopRow

	pop  ds

end; {tg_writeChar color only }

procedure tg_writeCharBg(x,y,digit,color,bgColor:byte; data:pFontData); assembler;
asm
	mov  ax,$B800
	mov  es,ax { es == vid segment }

	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax
	mov  al,x
	xor  ah,ah
	mov  di,ax
	add  di,bx
	or   di,1

	xor  ah,ah
	mov  al,digit
	shl  ax,1
	shl  ax,1
	shl  ax,1   { ax = chr * 8 }

	mov  dl,bgcolor
	and  dl,$0F
	mov  dh,dl
	mov  cl,4
	shl  dh,cl
	or   dl,dh

	mov  bl,color
	and  bl,$0F
	mov  bh,bl
	mov  cl,4
	shl  bh,cl { bx == cccc:0000:0000:cccc }

	push ds
	lds  si,data
	add  si,ax  { now pointing at font map offset }

	mov  cl,dl
	mov  ch,$05
	mov  dx,$F00F

@loopRow:
	lodsb
	mov  ah,al

	mov  al,cl
	rcl  ah,1
	jnc  @skip1
	and  al,dh
	or   al,bl
@skip1:
	rcl  ah,1
	jnc  @skip2
	and  al,dl
	or   al,bh
@skip2:
	stosb
	inc  di

	mov  al,cl
	rcl  ah,1
	jnc  @skip3
	and  al,dh
	or   al,bl
@skip3:
	rcl  ah,1
	jnc  @skip4
	and  al,dl
	or   al,bh
@skip4:
	stosb
	add  di,157

	dec  ch
	jnz  @loopRow

	pop  ds

end; { tg_writecharBg color,bgcolor }

procedure buffer_sourceClear; assembler;
asm
	xor  ax,ax
	les  di,sourceBuffer
	mov  cx,4000
	rep  stosw
end;

procedure buffer_copySource2Dest; assembler;
asm
	mov  dx,ds { faster than a push/pop to just use dx }
	mov  cx,4000
	les  di,destBuffer
	lds  si,sourceBuffer
	rep  movsw
	mov  ds,dx
end;

procedure buffer_copyDest2Screen; assembler;
asm
	mov  ax,$B800
	mov  es,ax
	mov  di,1
	mov  dx,ds { faster than a push/pop to just use dx }
	lds  si,destBuffer
	mov  cx,8000
@loop:
	movsb
	inc  di
	loop @loop
	mov  ds,dx
end;

{
	restore copies a 8x8 section from sourceBuffer to destBuffer
	ON THE BYTE BOUNDARY!!!
}
procedure buffer_copySourceDest8x8(x,y:byte); assembler;
asm
	mov  ah,y
	xor  al,al
	shr  ax,1  { =y*128 }
	mov  bx,ax
	shr  ax,1
	shr  ax,1  { =y*32 }
	add  bx,ax { 32+128 = 160 }
	shr  bx,1  { =y*80! }

	mov  al,x
	xor  ah,ah
	shr  ax,1
  add  bx,ax { bx now holds starting offset }

  les  di,destBuffer
  mov  dx,ds
	lds  si,sourceBuffer
	add  di,bx
	add  si,bx

	mov  bx,$004C
	mov  cx,8

@loop:
	movsw
	movsw
	add  si,bx
	add  di,bx
	loop @loop

	mov  ds,dx
end;

{
	show copies an 4x3 section from destBuffer to display
	ON THE BYTE BOUNDARY!!!
}
procedure buffer_show4x3(x,y:byte); assembler;
asm
	mov  ax,$B800
	mov  es,ax

	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  di,ax
	shr  ax,1
	shr  ax,1
	add  di,ax

	mov  al,x
	xor  ah,ah
	add  di,ax
	or   di,1
	mov  bx,di
	shr  bx,1 { holds renderBuffer offset }

	mov  dx,ds
	lds  si,destBuffer
	add  si,bx

	movsb
	inc di
	movsb
	add  si,$004E
	add  di,$009D

	movsb
	inc di
	movsb
	add  si,$004E
	add  di,$009D

	movsb
	inc di
	movsb

	mov  ds,dx
end;

{
	show copies an 8x6 section from destBuffer to display
	ON THE BYTE BOUNDARY!!!
}
procedure buffer_show8x6(x,y:byte); assembler;
asm
	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax

	mov  al,x
	xor  ah,ah
	add  bx,ax

	mov  di,bx
	or   di,1

	shr  bx,1

	mov  ax,$B800
	mov  es,ax

	mov  dx,ds
	lds  si,destBuffer
	add  si,bx
	mov  cx,8
	mov  bx,$0099
	mov  ax,$004C

@loop:
	movsb
	inc  di
	movsb
	inc  di
	movsb
	inc  di
	movsb
	add  si,ax
	add  di,bx
	loop @loop

	mov  ds,dx
end;

{
	draws a 3x3 tile to the dest buffer
}
procedure buffer_tile3(x,y,tile:byte); assembler;
asm
	{ set video address for STOSW }
	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax
	mov  al,x
	xor  ah,ah
	mov  cx,ax { cx holds x }
	add  bx,ax
	shr  bx,1  { bx holds *80 }

	les  di,destBuffer
	add  di,bx

	{ calculate tile offset }
	mov  al,tile
	xor  ah,ah
	shl  ax,1
	shl  ax,1
	shl  ax,1
	mov  bx,ax
	add  ax,bx
	add  ax,bx

	and  cx,1
	jnz  @oddtile
	add  ax,12
@oddTile:
	mov  dx,ds
	lds  si,tileSource
	add  si,ax

	mov  bx,$004E
{
	Unrolling this may have been silly, but it's fast
}
	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	add di,bx

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	add di,bx

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	mov ds,dx
end;

{
	draws a 5x5 tile to the dest buffer
}
procedure buffer_tile5(x,y,tile:byte); assembler;
asm
	{ set video address for STOSW }
	mov  ah,y
	xor  al,al
	shr  ax,1  { =*128 }
	mov  bx,ax
	shr  ax,1
	shr  ax,1  { =*32 }
	add  bx,ax
	mov  al,x
	xor  ah,ah
	mov  cx,ax { cx holds x }
	add  bx,ax
	shr  bx,1  { bx holds *80 }
	les  di,destBuffer
	add  di,bx

	{ calculate tile offset }
	xor  ax,ax
	mov  al,tile
	mov  bx,60
	mul  bx
	and  cx,1
	jnz  @oddtile
	add  ax,30
@oddTile:
	mov  dx,ds
	lds  si,tileSource
	add  si,ax

	mov  cx,5
	mov  bx,$004D

{ I'm NOT unrolling this one! }

@loop:

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	lodsw
	and  ah,es:[di]
	or   al,ah
	stosb

	add  di,bx

	loop @loop

	mov ds,dx
end;

{
	erases a 4x3 area on the byte boundary from sourceBuffer
	byte-bound
}
procedure buffer_nullTile4x3(x,y:byte); assembler;
asm
	{ set video address for STOSW }
	les  di,sourceBuffer

	mov  ah,y
	xor  al,al
	shr  ax,1
	mov  bx,ax
	shr  ax,1
	shr  ax,1
	add  bx,ax

	mov  al,x
	xor  ah,ah
	add  bx,ax

	shr  bx,1  { bx holds *80 }
	add  di,bx

	xor  ax,ax
	mov  es:[di],ax
	mov  es:[di+$50],ax
	mov  es:[di+$A0],ax
end;

procedure buffer_hideSprites;
begin
	buffer_sourceBackground;
	firstSprite^.copySourceDestOld;
	firstSprite^.bufShow;
end;

procedure buffer_writeSpritesToBackBuffer;
begin
	buffer_sourceBackground;
	firstSprite^.bufWriteTile;
end;

procedure buffer_copySpritesToScreen;
var
	tSprite:pSprite;
begin
	firstSprite^.updateThenBufShow;
	updateList.first^.updateToScreen;
end;

procedure buffer_eraseSpritesFromBuffer;
var
	tSprite:pSprite;
begin
	firstSprite^.copySourceDestCurrent;
	updateList.first^.updateToDest;
end;

procedure buffer_sourceBackground;
begin
	sourceBuffer:=backgroundBuffer;
	destBuffer:=renderBuffer;
end;

procedure buffer_sourceRender;
begin
	sourceBuffer:=renderBuffer;
	destBuffer:=backgroundBuffer;
end;

procedure tg_clear(color:byte); assembler;
asm
	mov  al,$DD
	mov  ah,color
	and  ah,$0F
	mov  bl,ah
	mov  cl,4
	shl  bl,cl
	or   ah,bl
	mov  dx,$B800
	mov  es,dx
	xor  di,di
	mov  cx,pageSize
	shr  cx,1
	rep  stosw
	xor  al,al
end;

procedure outEGAVGAPort; assembler;
asm
	mov  dx,es:[di]
	add  di,2
	xor  cx,cx
	mov  cl,es:[di]
	inc  di
	xor  al,al
@loopEGAVGA:
	mov  ah,es:[di]
	out  dx,ax
	inc  di
	inc  al
	loop @loopEGAVGA
end;

procedure tg_init; assembler;
asm
	mov  ax,$0F00;
	int  $10;
	mov  oldMode,al
	mov  ax,videoCard
	cmp  ax,videoCard_vga
	jne  @modeSet
	mov  al,safeMode
	and  al,$FF
	jnz  @modeSet
{
	set 200 scanline 80x25 text mode on VGA,
	eliminates 9 pixel wide text bug
}
	mov ax,$1200
	mov bl,$30
	int $10
@modeSet:
	mov  ax,$0002
	int $10;
	call buffer_sourceBackground
	call buffer_sourceClear
	call buffer_copySource2Dest
	call buffer_copyDest2Screen

	{ video off first }
	mov  dx,$03D8
	mov  al,$01
	out  dx,al
	{ disable normal interrupts }
	cli
	{ disable NMI too -- can't have CRTC interuppted! }
	in   al,$70
	and  al,$7F
	out  $70,al

	mov  cx,videoCard
	cmp  cx,videoCard_cga
	je   @cga
	cmp  cx,videoCard_tandy1000
	je   @cga
	cmp  cx,videoCard_tandySLTL
	je   @cga
	cmp  cx,videoCard_mcga
	je   @cga
	cmp  cx,videoCard_pcJr
	je   @cga
	cmp  cx,videoCard_ega
	je   @ega
	cmp  cx,videoCard_vga
	jne  @checkTweaks
@egaVgaSetup:
	mov  ax,$0309
	mov  dx,$3D4
	out  dx,ax
	mov  ax,$1003
	xor  bl,bl
	int  $10
	jmp  @checkTweaks
@ega:
	mov  ah,$02
	mov  al,safeMode
	cmp  al,0
	je   @egaUnsafe
	mov  ax,$0209
	mov  dx,$3D4
	out  dx,ax
	mov  ax,$1003
	xor  bl,bl
	int  $10
	mov  ax,$1000
	mov  bx,$1406
	int  $10
	jmp  @checkTweaks
@egaUnsafe:
	mov  ax,$1102 { load 8x8 font }
	xor  bl,bl
	int  $10
	mov  ax,$1003
	xor  bl,bl
	int  $10
	mov  ax,$1000
	mov  bx,$1406
	int  $10
	mov  dx,$3C2
	mov  al,$23
	out  dx,al
	les  di,pEGA_Sequencer
	call outEGAVGAPort
	les  di,pEGA_CRTC
	call outEGAVGAPort
	jmp  @checkTweaks
@cga:
	les  di,pCGA_Regs
	mov  dx,es:[di]
	add  di,2
	xor  ch,ch
	mov  cl,es:[di]
	shr  cl,1
	mov  bx,2
	inc  di
@loopCGARegData:
	mov  ax,es:[di]
	out  dx,ax
	add  di,bx
	loop @loopCGARegData
@checkTweaks:

	{ enable NMI }
	in   al,$70
	or   al,$80
	out  $70,al
	{ enable normal interrupts }
	sti
	{ video on }
	mov  dx,$03D8
	mov  al,$9
	out  dx,al

	mov  ax,videoCard
	cmp  ax,videoCard_mcga
	je   @vgaMcgaTweak
	cmp  ax,videoCard_vga
	je   @vgaMcgaTweak
	cmp  ax,videoCard_pcJr
	jne  @done

	{ the Jr keeps blink at an entirely different port }
	mov  dx,$3DA
	in   al,dx  { reset flip-flop }
	mov  al,$03 { mode control 2 }
	out  dx,al
	xor  al,al  { bit 1 off == no blink! }
	out  dx,al
	jmp  @done

@vgaMcgaTweak:
	{ for some weird reason some VGA/MCGA need this called a second time }
	mov  ax,$1003 { disable Blink }
	xor  bl,bl
	int  $10
@done:
	mov  textGraphOn,1

end;

procedure tg_term; assembler;
asm
	mov  ax,videoCard
	cmp  ax,videoCard_vga
	jne  @modeSet
	mov  al,safeMode
	and  al,$FF
	jnz  @modeSet
	{
		if vga set back to 400 line mode, since I've never
		seen ANYONE use	that setting by choice. WISH there
		was a way to detect how the VGA is set on this...
	}
	mov ax,$1202
	mov bl,$30
	int $10
@modeSet:
	xor ah,ah
	mov al,oldMode
	int $10;
	mov textGraphOn,0
end;

{
	adding our own custom exitproc is a safeguard so that
	if someone using this library forgets to call tg_term
	the program will still exit to text mode. I WISH that
	execution breaks or failures would call exitproc.

	A more robust unit would add a custom error handler.
}

procedure tg_exitProc; far;
begin
	if textGraphOn then tg_term;
	discardSprites;
	dispose(renderBuffer);
	dispose(backgroundBuffer);
	updateList.term;
	exitProc:=oldTgExitProc;
end;

begin
	safeMode:=paramExists('/safe');

	{ tp won't let you LEA constants? store as ptr... }
	pCGA_Regs:=@CGA_Regs;
	pEGA_Sequencer:=@EGA_Sequencer;
	pEGA_CRTC:=@EGA_CRTC;

	sWrite('Video card detected: ');
	videoCard:=detectCard;
	sWrite(videoCardName[videoCard]+CRLF);
	if (videoCard=videoCard_MDA) then begin
		sWrite('This program does not work on a monochrome display adapter!'+CRLF);
		halt;
	end;
	firstSprite:=nil;
	textSegment:=$B800;
	textGraphOn:=false;
	updateList.init;
	new(backgroundBuffer);
	new(renderBuffer);
	oldTgExitProc:=exitProc;
	exitProc:=@tg_exitProc;
end.