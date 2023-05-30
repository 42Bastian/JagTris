* -*-asm-*-
****************
* MODUL NEWGAME
* created 30/07/95
****************
* exports MODrun.NEWGAME
*	  MODend.NEWGAME
*	  MODlen.NEWGAME

;---------------
;- Choose Level
;---------------
SELECT	reg 12
EXIT	reg 11
LOOP	reg 7

ChooseLevel::
	WAITBLITTER
	movei	#TitleScreen,r0
	movei	#_title,r1
	WaitVBL

	store	r0,(blitter)
	store	r1,(blitter+$24)
	movei	#BLIT_PITCH1|BLIT_PIXEL32|BLIT_WID3584|BLIT_XADDPHR,r0
	moveq	#0,r1
	store	r0,(blitter+4)
	store	r0,(blitter+$28)
	store	r1,(blitter+$c)
	store	r1,(blitter+$18)
	store	r1,(blitter+$30)

	movei	#1<<16|(200*96)>>2,r0
	movei	#BLIT_SRCEN|BLIT_LFU_REPLACE,r1
	store	r0,(blitter+$3c)
	store	r1,(blitter+$38)
	WAITBLITTER

	movei	#DrawSprite,r3

	moveq	#level_sprite,r1
	BL	(r3)

	moveq	#score_sprite,r1
	BL	(r3)

	moveq	#high_sprite,r1
	BL	(r3)

	moveq	#lines_sprite,r1
	BL	(r3)

	moveq	#left_sprite,r1
	BL	(r3)

	moveq	#time_sprite,r1
	BL	(r3)

	movefa	LastLevel.a,r0
	moveta	r0,Level.a
	movei	#ShowLevel,r1
	BL	(r1)

	movei	#DrawStatStones,r0
	BL	(r0)

	movei	#ShowStat,r0
	BL	(r0)

	movei	#-1,r0
	moveta	r0,last_sec.a
	movei	#ShowTime,r0
	BL	(r0)

	movei	#ShowHigh,r0
	BL	(r0)

	movei	#ShowLines,r0
	BL	(r0)

	movei	#ShowPoints,r0
	BL	(r0)

	// change font color to red, save original
	movei	#$f00400+16*2,r0
	load	(r0),r1
	PUSH	r1
	movei	#$F000F040,r1
	store	r1,(r0)
	addq	#4,r0
	load	(r0),r1
	PUSH	r1,r0
	movei	#$f0e0f0fe,r1
	store	r1,(r0)

	// choose
	movei	#.exit,EXIT
	movei	#.select,SELECT
	move	pc,LOOP
.loop
	WaitVBL

	movei	#readJoy1,r0
	BL	(r0)
	cmpq	#0,r1
	jump	z,(LOOP)
	btst	#JOY_B_BIT,r1
	jump	nz,(SELECT)
	movefa	Level.a,r9
	btst	#JOY_UP_BIT,r1
	subqt	#1,r9
	jump	nz,(EXIT)
	btst	#JOY_DOWN_BIT,r1
	movei	#JOY_HASH|JOY_STAR,r5
	jump	nz,(EXIT)
	addqt	#2,r9

	cmp	r0,r5
	movei	#CheckKey,r4
	jr	nz,.loop1
	moveq	#10,r9
.reset:
	movei	#ResetFlag,r0
	moveq	#0,r1
	subq	#1,r1
	jr	.reset
	store	r1,(r0)

.loop1
	load	(r4),r5
	and	r1,r5
	cmp	r1,r5
	jr	z,.exit
	subq	#1,r9
	jr	nz,.loop1
	addqt	#4,r4
	jump	(LOOP)
	nop

.exit
	cmpq	#0,r9
	jump	n,(LOOP)
	cmpq	#10,r9
	jump	nn,(LOOP)
	nop
	moveta	r9,Level.a
	movei	#ShowLevel,r1
	jump	(r1)
	move	LOOP,LR

.select
	// restore font color
	POP	R2,R1,r0
	store	r1,(r0)
	subq	#4,r0
	store	r2,(r0)

	UNREG EXIT,LOOP,SELECT
;---------------
InitPlay::
	// clear title screen (should remove object ...)
	movei	#BLIT_PITCH1|BLIT_PIXEL32|BLIT_WID3584|BLIT_XADDPHR,r1
	movei	#TitleScreen,r2
	WAITBLITTER
	store	r1,(blitter+_BLIT_A1_FLAGS)
	store	r2,(blitter)
	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_STEP)
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	store	r0,(blitter+_BLIT_PATD)
	store	r0,(blitter+_BLIT_PATD+4)
	movei	#1<<16|96*200>>2,r0
	movei	#BLIT_PATDSEL,r1
	store	r0,(blitter+_BLIT_COUNT)
	WaitVBL
	store	r1,(blitter+_BLIT_CMD)
	WAITBLITTER

	moveq	#0,r0
	moveta	r0,Lines.a
	moveta	r0,sec.a
	moveta	r0,Points.a

	movei	#1000,r0
	moveta	r0,last_sec.a	; force first update
	moveta	r0,ms.a

	movefa	Level.a,r1
	movei	#DROP_INIT0,r0
	moveta	r1,LastLevel.a
	addq	#1,r1
	mult	r1,r0
	moveta	r0,DropCounter.a
	moveta	r0,DropCounterInit.a
	moveta	r0,DropCounterInit0.a

	movefa	Level.a,r1
	movei	#999,r0
	neg	r1
	jr	z,.cont
	addq	#10,r1
	moveq	#5,r0
	mult	r1,r0
.cont
	moveta	r0,LinesToGo.a

	moveq	#1,YADD_init
	cmpq	#9,r1
	jr	n,.cont2
	nop
	addq	#1,YADD_init
.cont2

	moveq	#1,YADD_init

	move	YADD_init,YADD
	moveq	#0,Y_Flag
	moveq	#0,X_Flag
	moveq	#0,PauseFlag

	;; Clear stone statistics
	movei	#stone_stat,r0
	moveq	#7,r1
	moveq	#0,r2
.loop
	subq	#1,r1
	store	r2,(r0)
	jr	nz,.loop
	addqt	#4,r0

	movei	#ShowLevel,r0
	BL	(r0)
	movei	#ShowHigh,r0
	BL	(r0)
	movei	#ShowPoints,r0
	BL	(r0)
	movei	#ShowLines,r0
	BL	(r0)
	movei	#NewStone,r0
	BL	(r0)

	POP	LR
	// fall thru
****************
ClearPf::
	movei	#BLIT_PITCH1|BLIT_PIXEL32|BLIT_WID3584|BLIT_XADDPHR,r1
	movei	#PlayField-(PFmax_x<<2)*4,r2
	WAITBLITTER
	store	r1,(blitter+_BLIT_A1_FLAGS)
	store	r2,(blitter)
	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_STEP)
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	store	r0,(blitter+_BLIT_PATD)
	store	r0,(blitter+_BLIT_PATD+4)
	movei	#1<<16|PFmax_x*(PFmax_y+4),r0
	movei	#BLIT_PATDSEL,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
	WAITBLITTER

	movei	#PlayField-(PFmax_x<<4),r0	; left
	moveq	#PFmax_x-1,r1
	moveq	#8,r2		; left stone
	shlq	#2,r1
	moveq	#9,r3		; right stone
	add	r0,r1
	moveq	#PFmax_x,r5
	moveq	#PFmax_y+3,r4
	shlq	#2,r5
.loop
	store	r2,(r0)		; border
	add	r5,r0
	subq	#1,r4
	store	r3,(r1)
	jr	nz,.loop
	add	r5,r1

	moveq	#11,r2		; left corner
	moveq	#12,r3		; right corner
	store	r2,(r0)
	store	r3,(r1)
	moveq	#10,r2		; bottom stone
	addq	#4,r0
	moveq	#PFmax_x-2,r4
.loop1
	subq	#1,r4
	store	r2,(r0)		; bottom
	jr	nz,.loop1
	addqt	#4,r0

	jump	(LR)
	nop

;;; ****************
;;; DrawSprite
;;; r1 - sprite-number
;;; ****************
spriteTable	REG 15
DrawSprite::
	movei	#sprite_table,spriteTable
	load	(spriteTable+r1),spriteTable

	WAITBLITTER

	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	store	r0,(blitter+_BLIT_A2_PIXEL)

	load	(spriteTable),r0	; y/x position
	load	(spriteTable+4),r1	; step
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r1,(blitter+_BLIT_A1_STEP)

	load	(spriteTable+8),r0	; screen
	movei	#BLIT_PITCH1|BLIT_PIXEL8|BLIT_WID128|BLIT_XADDPIX,r1
	store	r0,(blitter)	; A1 Base
	store	r1,(blitter+_BLIT_A1_FLAGS)
;---------------
	load	(spriteTable+12),r0	; sprite data
	movei	#BLIT_PITCH1|BLIT_PIXEL8|BLIT_WID4|BLIT_XADDPIX,r1
	store	r1,(blitter+_BLIT_A2_FLAGS)
	store	r0,(blitter+_BLIT_A2_BASE)
;---------------
	load	(spriteTable+16),r0	; y/x size
	movei	#BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE,r1
	store	r0,(blitter+_BLIT_COUNT)
	jump	(LR)
	store	r1,(blitter+_BLIT_CMD)
UNREG spriteTable

;;; Draw statistic stones
STATSTONEDATA	REG 2
LOOP		REG 3
COUNTER		REG 4

DrawStatStones::
	movei	#StatStoneData,STATSTONEDATA
	movei	#.loop,LOOP
	moveq	#7,COUNTER
.loop
	WAITBLITTER
	moveq	#0,r1
	store	r1,(blitter+_BLIT_A1_FSTEP)
	store	r1,(blitter+_BLIT_A1_FPIXEL)
	store	r1,(blitter+_BLIT_A2_STEP)
	store	r1,(blitter+_BLIT_A2_PIXEL)

	load	(STATSTONEDATA),r0
	addqt	#4,STATSTONEDATA
	movei	#1<<16|(-32 & $ffff),r1
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r1,(blitter+_BLIT_A1_STEP)

	movei	#StatScreen,r0
	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID32|BLIT_XADDPHR,r1
	store	r0,(blitter)	; A1 Base
	store	r1,(blitter+_BLIT_A1_FLAGS)
;---------------
	load	(STATSTONEDATA),r0
	movei	#BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID32|BLIT_XADDPHR,r1
	store	r0,(blitter+_BLIT_A2_BASE)
	store	r1,(blitter+_BLIT_A2_FLAGS)
;---------------
	movei	#32<<16|32,r0
	movei	#BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
	subq	#1,COUNTER
	jump	nz,(LOOP)
	addqt	#4,STATSTONEDATA

	jump	(LR)
	nop

	LONG
StatStoneData:
	DC.L ((0*22+1)<<16)|4, Stones+(0<<13)
	DC.L ((1*22+1)<<16)|4, Stones+(1<<13)
	DC.L ((2*22+1)<<16)|4, Stones+(2<<13)
	DC.L ((3*22+1)<<16)|4, Stones+(3<<13)
	DC.L ((4*22-3)<<16)|0, Stones+(4<<13)
	DC.L ((5*22+0)<<16)|4, Stones+(5<<13)
	DC.L ((6*22+1)<<16)|8, Stones+(6<<13)

UNREG STATSTONEDATA,LOOP,COUNTER
