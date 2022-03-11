;-*-asm-*-
joypad::
	PUSH	LR
joypad2:
	movei	#readJoy1,r0
	BL	(r0)
	cmpq	#0,PauseFlag
	jr	z,nop
	nop
	WaitVBL
	btst	#JOY_PAUSE_BIT,r1
	jr	z,joypad2
	nop
	moveq	#0,PauseFlag
	RTS
nop:
	btst	#JOY_LEFT_BIT,r0
	movei	#left,r2
	jump	nz,(r2)
	btst	#JOY_RIGHT_BIT,r0
	movei	#right,r2
	jump	nz,(r2)
	nop

	movei	#JOY_HASH|JOY_STAR,r2
	and	r2,r0
	cmp	r2,r0
	movei	#.exit,r2
	jump	z,(r2)
	moveq	#1,r1

joypad1
	movei	#LastJoy+4,r0
	load	(r0),r1
	btst	#JOY_A_BIT,r1
	movei	#rotate_right,r2
	jump	nz,(r2)
	btst	#JOY_6_BIT,r1
	jump	nz,(r2)
	btst	#JOY_B_BIT,r1
	movei	#rotate_left,r2
	jump	nz,(r2)
	btst	#JOY_4_BIT,r1
	jump	nz,(r2)
	btst	#JOY_DOWN_BIT,r1
	movei	#drop,r2
	jump	nz,(r2)
	btst	#JOY_PAUSE_BIT,r1
	movei	#do_pause,r2
	jump	nz,(r2)
	btst	#JOY_0_BIT,r1
	jr	nz,mute
	moveq	#0,r1
.exit
	RTS

mute:
	movei	#MuteFlag,r0
	load	(r0),r1
	not	r1
	store	r1,(r0)
	moveq	#0,r1
	RTS

****************
LOOP		REG 5
CounterX	REG 4
CounterY	REG 3
ptr		REG 2


do_pause::
	moveq	#1,PauseFlag

 IF 1
	movei	#24*8,CounterY
	movei	#.loop,LOOP
	move	screen1_ptr,ptr
	movei	#10*8,CounterX
	addq	#16,ptr
	movei	#$f010,r0
	movei	#$0001,r1
.loop
	storew	r0,(ptr)
	subq	#1,CounterX
	jr	nz,.loop
	addq	#2,ptr
	add	r1,r0
	subq	#1,CounterY
	movei	#10*8,CounterX
	jump	nz,(LOOP)
	addq	#32,ptr
 ENDIF
 if 1
	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	store	r0,(blitter+_BLIT_A2_PIXEL)

	movei	#62<<16|44,r0
	movei	#1<<16|(-pause_w & $ffff),r1
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r1,(blitter+_BLIT_A1_STEP)

	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID96|BLIT_XADDPIX,r1
	store	screen1_ptr,(blitter)	; A1 Base
	store	r1,(blitter+_BLIT_A1_FLAGS)
;---------------
	movei	#_pause,r0
	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID4|BLIT_XADDPIX,r1
	store	r1,(blitter+_BLIT_A2_FLAGS)
	store	r0,(blitter+_BLIT_A2_BASE)
;---------------
	movei	#pause_h<<16|pause_w,r0
	movei	#BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
 endif
	WAITBLITTER
wait:
	WaitVBL
	movei	#readJoy1,r0
	BL	(r0)
	cmpq	#0,r1
	jr	nz,wait
	nop

	movei	#joypad2,r0
	jump	(r0)
	nop

UNREG ptr,CounterX,CounterY,LOOP

***************
drop::
	moveq	#1,r0
	moveq	#4,YADD
	moveta	r0,DropCounterInit.a
	moveta	r0,DropCounter.a
	moveq	#0,r1
	RTS

rotate_left::
	moveq #1,r1
	neg r1
	jr .cont0
rotate_right
	movefa Rotation.a,r0
	moveq #1,r1
.cont0
	PUSH	r0
	add	r1,r0
	shlq	#30,r0
	shrq	#30,r0		; and $3
	moveta	r0,Rotation.a
	movei	#CheckNewPos0,r2
	BL	(r2)
	POP	r1
	cmpq	#0,r0
	jr	z,.cont
	nop
	moveta	r1,Rotation.a
.cont
	moveq	#0,r1
	RTS

right::
	jr	.cont
left
	moveq	#8,r1
	neg	r1
.cont
	movei	#joypad1,r0
	cmpq	#0,X_Flag
	jump	nz,(r0)
	movefa	x_pos.a,r0
	movei	#CheckNewPos,r2
	add	r1,r0
	PUSH	r1
	BL	(r2)
	POP	X_Flag

	cmpq	#0,r0
	movei	#joypad1,r0
	jump	z,(r0)
	nop
	jump	(r0)
	moveq	#0,X_Flag
