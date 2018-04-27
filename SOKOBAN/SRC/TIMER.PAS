unit timer;

interface

uses
	jfunc;

const
	clock8253=1193181;
	countExact=4971;
	countInterval=65536 div countExact;
	freq=clock8253 div countExact;

	PIT_Data0=$40;
	PIT_Data1=$41;
	PIT_Data2=$42;
	PIT_Control=$43;

	PIT_Select0=$00;
	PIT_Select1=$40;
	PIT_Select2=$80;

	PIT_Latch=$00;
	PIT_LSB=$10;
	PIT_MSB=$20;
	PIT_LSBMSB=$30;

	PIT_Mode0=$00; { Interrupt on Terminal Count }
	PIT_Mode1=$02; { Hardware Retriggerable One-Shot }
	PIT_Mode2=$04; { Rate Generator }
	PIT_Mode3=$06; { Square Wave }
	PIT_Mode4=$08; { Software Triggered Strobe }
	PIT_Mode5=$0A; { Hardware Triggered Strobe }

	PIT_BCD=$01;

var
	userClockCounter:word;

procedure startTimer;
procedure killTimer;
function getUserClockInterval(hz:word):word;
function userTimerExpired(interval:word):boolean;

implementation

var
	isrCount:word;
	timerActive:boolean;
	oldISR:pointer;
	oldExitProc:pointer;

function getUserClockInterval(hz:word):word; assembler;
asm
	xor  dx,dx
	mov  ax,freq
	mov  bx,hz
	div  bx
end;

procedure timerISR; interrupt; assembler;
asm
	inc  userClockCounter
	dec  isrCount
	jnz  @done
	mov  isrCount,countInterval
	pushf
	call oldISR
@done:
end;

procedure startTimer; assembler;
asm
	mov  isrCount,countInterval
	mov  dx,OFFSET timerISR
	mov  bx,SEG timerISR
	push ds
	mov  ds,bx
	mov  ax,$251C
	int  $21
	pop  ds
	{ set timer 0 to desired high speed frequency }
	mov  al,PIT_Select0 or PIT_LSBMSB or PIT_Mode2
	out  PIT_Control,al
	mov  ax,countExact
	out  PIT_Data0,al
	mov  al,ah
	out  PIT_Data0,al
	mov  timerActive,1
end;

procedure killTimer; assembler;
asm
	{ reset timer0 to normal }
	mov  al,PIT_Select0 or PIT_LSBMSB or PIT_Mode2
	out  PIT_Control,al
	mov  ax,$FFFF
	out  PIT_Data0,al
	mov  al,ah
	out  PIT_Data0,al
	mov  ax,$251C
	push ds
	lds  dx,oldISR
	int  $21
	pop  ds
	mov  timerActive,0
end;

function userTimerExpired(interval:word):boolean; assembler;
asm
	xor  al,al
	mov  bx,userClockCounter
	mov  cx,interval
	cmp  bx,cx
	jl   @done
	sub  bx,cx
	mov  userClockCounter,bx
	inc  al
@done:
end;

procedure safeExit; far; assembler;
asm
	cmp  timeractive,0
	je   @done
	call killTimer;
@done:
	les  di,oldExitProc
	mov  WORD PTR exitProc,di
	mov  WORD PTR exitProc+2,es
end;

begin
	asm
		mov  ax,$351C
		int  $21
		mov  WORD PTR oldISR,bx
		mov  WORD PTR oldISR+2,es
		mov  timerActive,0
	end;
	oldExitProc:=exitproc;
	exitproc:=@safeExit;
end.