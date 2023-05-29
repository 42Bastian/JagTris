* -*-asm-*-
****************
* GAME
* created : ??.7.95
****************

	MODULE GAME,MODend_MAIN
*****************
*  main loop	*

game::
	movei #draw_pf,r0	; game
	BL (r0)

again::
	WaitVBL
//->	movei	#128*8*16,r0
//->	moveta	r0,Cursor.a
//->	movei	#$f00058,r0
//->	loadw	(r0),r0
//->	movei	#PrintHex,r1
//->	BL	(r1)

	movei #ShowTime,r0
	BL (r0)

	movei #ClearSecond,r0
	BL (r0)

	cmpq #0,UPDATE
	movei #.cont,r0
	jump z,(r0)
	nop

	movei	#SettleStone,r0	; game2
	BL	(r0)
	movei	#draw_pf,r0	; game
	BL	(r0)
	movei	#IncPoints,r1	; main
	moveq	#1,r0
	BL	(r1)
	movei	#CheckLines,r0	; game2
	BL	(r0)
	movei	#NewStone,r0
	BL	(r0)

	moveq	#0,Y_Flag
	moveq	#0,X_Flag
	move	YADD_init,YADD
	movefa	DropCounterInit0.a,r0
	moveta	r0,DropCounterInit.a
	moveta	r0,DropCounter.a

	movei	#CheckNewPos0,r2	; game
	BL	(r2)
	cmpq	#0,r0
	movei	#ExitGame,r0
	jump	nz,(r0)
	nop
.cont
	movei	#DrawCurrentStone,r0	; game1
	BL	(r0)
;---------------
;--- test horizontal move
;---------------
MoveHor::
	cmpq	#0,X_Flag
	jr	z,.cont1
	movefa	x_pos.a,r0
	jr	nn,.cont
	subq 	#1,r0
	jr	.cont1
	addq	#1,X_Flag
.cont:
	addqt	#2,r0
	subq	#1,X_Flag
.cont1:
	moveta r0,x_pos.a
;---------------
;--- test move down
;---------------
y_pos	REG 13

MoveDown::
	cmpq	#4,YADD			; dropped?
	jr	nz,.xx
	cmpq	#8,Y_Flag		; full line?
	jr	nz,.xx
	moveq	#1,r0
	movei	#IncPoints,r1		; yes =>
	BL	(r1)
.xx:

	movefa	y_pos.a,y_pos
	cmpq	#8,Y_Flag
	movei	#CheckNewPos0,r2
	jr	nz,.cont0		; Y_Flag=8 => cont0
	cmpq	#0,Y_Flag
	BL	(r2)
	cmpq	#0,r0
	jr	z,.cont1
	cmpq	#0,X_Flag		; hori. move active ?
	jr	nz,.cont2		; yes, no new stone
	movefa	y_pos.a,y_pos
	jr	.cont2
	subq	#1,UPDATE

.cont0:
	cmpq	#0,Y_Flag
	jr 	z,.cont2		; Y_Flag=0 => .cont2
	movefa	y_pos.a,y_pos

.cont1:
	sub	YADD,Y_Flag
	add	YADD,y_pos
.cont2:
	moveta	y_pos,y_pos.a
UNREG y_pos
;-----------------
weiter::
	movei	#joypad,r0
	BL	(r0)
	cmpq	#1,r1
	movei	#ExitGame,r0
	jump	z,(r0)
	nop

//->	movei	#$f00400,r0
//->	movei	#$000,r1
//->	storew	r1,(r0)

//->	movei	#128*8*23,r0
//->	moveta	r0,Cursor.a
//->	movei	#$f005fA,r0
//->	loadw	(r0),r0
//->	movei	#PrintHex,r1
//->	BL	(r1)

	movei #again,r0
	jump (r0)
	nop
ExitGame:
	RTS
*****************
* draw play-field
y_count	REG  6
x_count	REG  5
PF_ptr	REG  4
LOOP	REG  3

draw_pf::
	PUSH	r2,r3,r4,r5,r6

	WAITBLITTER

	xor	UPDATE,UPDATE
	movei	#1<<16|(-8&$ffff),r0
	store	r0,(blitter+_BLIT_A1_STEP)
	xor	r0,r0
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	movei	#$3f,r1

	store	r0,(blitter+_BLIT_A2_STEP)
	store	r0,(blitter+_BLIT_A2_PIXEL)
	store	r1,(blitter+_BLIT_A2_MASK)
	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID96|BLIT_XADDPHR,r1
	store	screen_ptr,(blitter)
	store	r1,(blitter+_BLIT_A1_FLAGS)

	movei #BLIT_MASK|BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID64|BLIT_XADDPHR,r1
	store	r1,(blitter+_BLIT_A2_FLAGS)

	movei	#PlayFieldTop,PF_ptr
	moveq	#PFmax_x-1,x_count
	moveq	#PFmax_y-1,y_count
	shlq	#3,x_count
	shlq	#3,y_count
	movei	#.loop0,LOOP

.loop0
	move	y_count,r0
	move	x_count,r2
	shlq	#16,r0
	load	(PF_ptr),r1
	or	r2,r0
	shlq	#7,r1
	add	pattern_ptr,r1

.wait1
	load	(blitter+_BLIT_CMD),r2
	shrq	#1,r2
	jr	cc,.wait1
	nop

	store	r0,(blitter+_BLIT_A1_PIXEL)
;---------------
	store	r1,(blitter+_BLIT_A2_BASE)	; source
;---------------
	movei	#8<<16|8,r0
	movei	#BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE|BLIT_BUSHI*0,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
;---------------
	subq	#8,x_count
	jump	nn,(LOOP)
	subq	#4,PF_ptr

	subq	#8,y_count
	moveq	#PFmax_x-1,x_count
	jump	nn,(LOOP)
	shlq	#3,x_count
	POP	r2,r3,r4,r5,r6
	jump	(LR)
	nop

UNREG LOOP,PF_ptr,x_count,y_count
*****************
* clear Overlay
ClearSecond::
	movei	#BLIT_PITCH1|BLIT_PIXEL32|BLIT_WID3584|BLIT_XADDPHR,r1
	WAITBLITTER
	store	screen1_ptr,(blitter)
	store	r1,(blitter+_BLIT_A1_FLAGS)
	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	store	r0,(blitter+_BLIT_A1_STEP)
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_PATD)
	store	r0,(blitter+_BLIT_PATD+4)
	movei	#1<<16|((PFmax_x*8)*(PFmax_y*8)*2)>>2,r0
	movei	#BLIT_PATDSEL,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
	jump	(LR)
	nop
*****************
* CheckNewPos
* r0 = new_x / r1 = new_y

COUNTER2	REG 7
EXIT		REG 6
LINEOFFSET	REG 5
MASKE		REG 4
PFptr		REG 3

CheckNewPos0::
	movefa	x_pos.a,r0
CheckNewPos::
	movefa	y_pos.a,r1

	movefa	StoneType.a,r5	; Stein
	movefa	Rotation.a,r6	; Drehung
	shlq	#2,r5
	or	r5,r6
	movei	#steine,r3	; Position des Drehpunktes
	shlq	#3,r6
	add	r6,r3
	shlq	#3,r6
	movei	#Maske,MASKE
	add	r6,MASKE
	movei	#.exit,EXIT

	load	(r3),r5
	addq	#4,r3
	addq	#8,r0
	add	r5,r0		; x korrigieren
	add	X_Flag,r0
	jump	n,(EXIT)
	load	(r3),r5
	shrq	#3,r0
	add	r5,r1		; y korrigieren
	add	Y_Flag,r1
	sharq	#3,r1
	movei	#PFmax_x<<2,LINEOFFSET
	shlq	#2,r0
	movei	#PlayField,PFptr
	imult	LINEOFFSET,r1
	add	r0,PFptr
	add	r1,PFptr
	subq	#4*4,LINEOFFSET
	moveq	#4,r0		; 4x4 blocks
	moveq	#4,COUNTER2
	load	(MASKE),r1
.loop
	addq	#4,MASKE
	load	(PFptr),r2
	addq	#4,PFptr
	and	r2,r1
	jump	nz,(EXIT)
	subq	#1,r0
	jr	nz,.loop
	load	(MASKE),r1

	subq	#1,COUNTER2
	moveq	#4,r0
	jr	nz,.loop
	add	LINEOFFSET,PFptr

	jump	(LR)
	moveq	#0,r0

.exit
	jump	(LR)
	moveq 	#1,r0



UNREG PFptr,MASKE,LINEOFFSET,EXIT,COUNTER2
*****************
	include "joystick.inc"

;-------------------------------

*****************
* DrawCurrentStone
Stone	REG 4
x_pos	REG 3
y_pos	REG 2

DrawCurrentStone::
	WAITBLITTER

	moveq	#0,r1
	store	r1,(blitter+_BLIT_A1_FSTEP)
	store	r1,(blitter+_BLIT_A1_FPIXEL)
	store	r1,(blitter+_BLIT_A2_PIXEL)

	movefa	x_pos.a,x_pos
	movefa	y_pos.a,y_pos
	addq	#8,x_pos		; Offset for border
	movefa	StoneType.a,r1		; stone type
	movefa	Rotation.a,r0
	shlq	#2,r1
	add	r1,r0
	move	r0,Stone
	movei	#steine,r1		; coordinates of central point
	shlq	#3,r0
	add	r0,r1
	load	(r1),r0
	addq	#4,r1
	add	r0,x_pos
	load	(r1),r0
	add	r0,y_pos
	shlq	#16,x_pos
	shlq	#16,y_pos
	shrq	#16,x_pos
	or	x_pos,y_pos
	store	y_pos,(blitter+_BLIT_A1_PIXEL)

	move	x_pos,r0
	neg	r0
	shlq	#30,r0
	shrq	#30,r0			; = AND #$3
	addq	#32,r0
	neg	r0
	shlq	#16,r0
	shrq	#16,r0
	bset	#16,r0
	store	r0,(blitter+_BLIT_A1_STEP)

	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID96|BLIT_XADDPHR,r1
	store	screen1_ptr,(blitter)	; A1 Base
	store	r1,(blitter+_BLIT_A1_FLAGS)
;---------------
	movei	#Stones,r0
	shlq	#11,Stone
	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID32|BLIT_XADDPHR,r1
	add	Stone,r0
	store	r1,(blitter+_BLIT_A2_FLAGS)
	store	r0,(blitter+_BLIT_A2_BASE)
	movei	#-4&$ffff,r0
	store	r0,(blitter+_BLIT_A2_STEP)
;---------------
	movei	#32<<16|32,r0
	movei	#BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE,r1
	store	r0,(blitter+_BLIT_COUNT)
	movefa	x_pos.a,r0
	shlq	#30,r0
	shrq	#30,r0
	jr z,.cont
	nop
	  bset #10,r1	  ; UPDA2
.cont
	jump	(LR)
	store	r1,(blitter+_BLIT_CMD)

UNREG x_pos,y_pos,Stone
*****************

***************************************************************
***************************************************************

;---------------
; settle current stone
;---------------
COLOR		REG 7
COUNTER2	REG 6
LINEOFFSET	REG 5
MASKE		REG 4
PFptr		REG 3

SettleStone::
//->	jump	(LR)

	movefa	StoneType.a,r5	; Stein
	movefa	Rotation.a,r6	; Drehung
	move	r5,COLOR
	shlq	#2,r5
	or	r5,r6
	movei	#steine,r3	; Position des Drehpunktes
	shlq	#3,r6
	addq	#1,COLOR
	add	r6,r3
	shlq	#3,r6
	movei	#Maske,MASKE
	add	r6,MASKE

	load	(r3),r5
	movefa	x_pos.a,r0
	addq	#4,r3
	addq	#8,r0
	add	r5,r0		; x korrigieren
	load	(r3),r5
	movefa	y_pos.a,r1
	shrq	#3,r0
	add	r5,r1		; y korrigieren
	sharq	#3,r1
	movei	#PFmax_x<<2,LINEOFFSET
	shlq	#2,r0
	movei	#PlayField,PFptr
	mult	LINEOFFSET,r1
	add	r0,PFptr
	add	r1,PFptr
	subq	#4*4,LINEOFFSET
	moveq	#4,r0		; 16 blocks
	moveq	#4,COUNTER2
.loop
	load	(MASKE),r1
	addqt	#4,MASKE
	and	COLOR,r1
	load	(PFptr),r2
	or	r2,r1
	subq	#1,r0
	store	r1,(PFptr)
	jr	nz,.loop
	addqt	#4,PFptr

	subq	#1,COUNTER2
	moveq	#4,r0
	jr	nz,.loop
	add	LINEOFFSET,PFptr

	jump	(LR)
	nop

UNREG PFptr,MASKE,LINEOFFSET,COUNTER2,COLOR
;---------------
; test for completed lines
;---------------
LOOP2		REG 15
Points		REG 13
Y1		REG 12
Y		REG 11
CONT		REG 10
LOOP1		REG  9
LOOP0		REG  8
Counter		REG  7
LineCount	REG  6
ptr_save	REG  5
ptr		REG  4
ptr2		REG  3
LineOffset	REG  2

CheckLines::
	PUSH	LR
	movei	#.loop0,LOOP0
	movei	#.loop1,LOOP1
	movei	#.loop2,LOOP2
	movei	#.cont,CONT
	movei	#PlayFieldTop,ptr
	movei	#PFmax_x*4,LineOffset	; field	width
	moveq	#4,LineCount		; max.	4 lines
	moveq	#10,Points
	moveq	#PFmax_y-2,Y

.loop0
	sub	LineOffset,ptr
	move	ptr,ptr_save
	subq	#4,ptr			; border
	moveq	#PFmax_x-3,Counter	; field width - border
.loop1
	load	(ptr),r0
	subq	#4,ptr
	cmpq	#0,r0
	jump	z,(CONT)		; empty space
	subq	#1,Counter
	jump	nn,(LOOP1)
	nop

;-- line filled, "blink" line
	moveq	#7,r1
	move	ptr_save,ptr
.loop2
	moveq	#PFmax_x-3,Counter

	subq	#4,ptr
.loop3
	subq	#1,Counter
	store	r1,(ptr)
	jr	nn,.loop3
	subqt	#4,ptr

	; wait 3 VBLs
	moveq	#1,r0
.wait
	WaitVBL
	subq	#1,r0
	jr	nn,.wait
	nop

	movei	#draw_pf,r0
	PUSH	r1
	BL	(r0)
	POP	r1
	subq	#1,r1
	jump	nn,(LOOP2)
	move	ptr_save,ptr

; -- now move all lines above down
	move	ptr_save,ptr2	; dest
	subq	#4,ptr2
	move	ptr2,ptr
	sub	LineOffset,ptr
	move	Y,Y1
	moveq	#10,Counter
.loop4
	load	(ptr),r0
	subq	#4,ptr
	store	r0,(ptr2)
	subq	#1,Counter
	jr	nz,.loop4
	subq	#4,ptr2

	subq	#8,ptr			; skip two borders
	subq	#8,ptr2
	subq	#1,Y1
	jr	nn,.loop4
	moveq	#10,Counter

	WaitVBL
	movei	#draw_pf,r0
	BL	(r0)
	PUSH	r2,r3,r4
	movei	#IncLines,r0
	BL	(r0)
	movei	#IncPoints,r1
	move	Points,r0
	BL	(r1)
	POP	r2,r3,r4
	shlq	#1,Points
	subq	#1,LineCount
	addqt	#1,Y
	jr	z,.exit
	add	LineOffset,ptr_save
.cont
	subq	#1,Y
	jump	nn,(LOOP0)
	move	ptr_save,ptr

.exit
	RTS

UNREG Y1,Y,CONT,LOOP2,LOOP1,LOOP0,Counter,LineCount
UNREG ptr_save,ptr,ptr2,LineOffset,Points
;---------------
	ENDMODULE GAME
**********************************************************
**********************************************************

//->	MODULE JOYSTICK,MODend_GAME
//->	include "joystick.inc"
//->	ENDMODULE JOYSTICK
