;-*-asm-*-
	GPU

TEST	set 0
AnzObjekte	equ 4
SOUND	equ 1

	include <js\symbols\blit_eq.js>
	include <js\symbols\jagregeq.js>
	include <js\symbols\joypad.js>
	include <js\macro\help.mac>
	include <js\macro\joypad1.mac>
	include <js\macro\module.mac>


****************
* global register equates
	include "globlreg.inc"
	include "constant.S"
	include "sprite.S"
	include "sprite_coor.S"
****************
* vars
leer		equ GPU_ENDRAM-4
//->seed1		equ leer-3*4
//->IRQ_STACK	EQU seed1-4
IRQ_STACK	EQU leer
STACK		EQU IRQ_STACK-32

 echo "Stacktop %XIRQ_STACK"
****************

pf_x	equ 320/2-PFmax_x*8/2

screen_size 	equ PFmax_x*8*PFmax_y*8*2

ResetFlag	equ _68kvars
MuteFlag	equ _68kvars+4

level_sprite	equ 0
time_sprite	equ 4
score_sprite	equ 8
high_sprite	equ 12
lines_sprite	equ 16
left_sprite	equ 20
credits_sprite	equ 24

PlayFieldTop	equ PlayField+PFmax_x*PFmax_y*4-4

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
* PART MACRO-definitions
MACRO WaitVBL
	xor VBLFlag,VBLFlag
	cmpq #0,VBLFlag
.\_wait	jr z,.\_wait
	cmpq #0,VBLFlag
ENDM

MACRO WAITBLITTER
.\wait@	load (blitter+$38),r0
	shrq #1,r0
	jr cc,.\wait@
	nop
ENDM

MACRO MyINITMODULE
	movei #MODrun_\0+$8000,r0	; dest-adr
	movei #MODstart_\0,r1
	movei #1<<16|(MODlen_\0),r2
	movei #overlay,r3
	BL (r3)
ENDM
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	run $40020

startGPU::
	INITMODULE INIT
 IF SOUND = 1
	movei	#DSP_start,r0
	movei	#DSP_RAM,r1
	movei	#(DSP_end-DSP_start),r2
	nop
cpy_dsp:
	load	(r0),r3
	addq	#4,r0
	subq	#4,r2
	store	r3,(r1)
	jr	pl,cpy_dsp
	addq	#4,r1

	movei	#$20-8,r14
	movei	#LSP_module_music_data,r0
	store	r0,(r14)
	movei	#LSP_module_sound_bank,r0
	store	r0,(r14+4)

	moveq	#0,r0
	bset	#14,r0
	movei	#$f1a100,r14
	store	r0,(r14)
	movei	#$f1b000,r0
	store	r0,(r14+$10)	; PC
	moveq	#1,r0
	store	r0,(r14+$14)	; GO
 ENDIF

	movei #init,r0
	nop
	jump (r0)
	nop
endless::
	jr	endless
	nop
****************
 IMODULE irq_sub	 ; pre-assembled IRQ-routs
****************
* MODULE MAIN    *
	MODULE MAIN,MODend_irq_sub
main::
	movei	#1<<14|%11111<<9|%01100<<4,r0
	store	r0,(status)
	nop
	nop
	movei	#$f00026,r0
	storew	r1,(r0)

main1
	MyINITMODULE NEWGAME
	MCALL NEWGAME

	MyINITMODULE GAME
	MCALL GAME

	movei #main1,r0
	jump (r0)
	nop

;-----------------------------------------
;- Copy overlay routine
;-----------------------------------------
overlay:
	load (blitter+$38),r3
	shrq #1,r3
	jr cc,overlay
	nop

	store r0,(blitter)
	store r1,(blitter+$24)
	movei #BLIT_PITCH1|BLIT_PIXEL8|BLIT_WID320|BLIT_XADDPHR,r0
	xor r1,r1
	store r0,(blitter+4)
	store r0,(blitter+$28)
	store r1,(blitter+$c)
	store r1,(blitter+$18)
	store r1,(blitter+$30)

	movei #BLIT_SRCEN|BLIT_LFU_REPLACE|BLIT_BUSHI*0,r1
	store r2,(blitter+$3c)
	store r1,(blitter+$38)
	WAITBLITTER
	jump	(LR)
	nop
;----------------------------------------
;- IncPoints
;-
;- IN: D0 increment
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
IncPoints::
	movefa 	Points.a,r1
	add 	r1,r0
	moveta 	r0,Points.a
;----------------------------------------
;- ShowPoints
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
ShowPoints::
//->	jump	(LR)
//->	nop

	PUSH	r4,LR
	movefa 	Points.a,r0
	movefa 	High.a,r1
	cmp 	r1,r0
	jr 	n,.cont	; points<high =>
	moveq 	#0,r4
	moveta 	r0,High.a
	subq 	#1,r4	; set flag
.cont
//->	move	r0,r1
//->	movei	#128*8*2+48,r0
//->	moveta	r0,Cursor.a
//->	movei	#DrawDigitDez6,r0
//->	BL	(r0)

	move	r0,r1
	movei	#(y_score<<16)|(pf_x-6*digit_w-4),r2
	moveq	#scr_score,r3
	movei 	#DrawDigitDez6,r0
	BL 	(r0)
	cmpq 	#0,r4
	POP 	r4,LR
	jump	z,(LR)
	nop
;----------------------------------------
;- ShowHigh
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
ShowHigh::
//->	jump	(LR)
//->	nop

//->	movei	#128*8*0+48,r0
//->	moveta	r0,Cursor.a
//->	movei 	#DrawDigitDez6,r0
//->	jump	(r0)
//->	movefa	High.a,r1

	movei	#(y_high<<16)|(pf_x-6*digit_w-4),r2
	movei 	#DrawDigitDez6,r0
	moveq	#scr_high,r3
	jump	(r0)
	movefa	High.a,r1
;----------------------------------------
;- IncLines
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
IncLines::
	movefa	Lines.a,r0
	movei	#999,r1
	cmp	r0,r1
	jr	z,.saturate
	movefa	LinesToGo.a,r1
	addq	#1,r0
.saturate
	subq	#1,r1
	moveta	r0,Lines.a
	movei	#ShowLines,r0
	jump	nz,(r0)
	moveta	r1,LinesToGo.a

	PUSH	LR
	BL	(r0)
	POP	LR
	;; fall thru
;----------------------------------------
;- IncLevel
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
IncLevel::
	PUSH	LR
	movei	#(y_to_go)<<16|(pf_x-3*digit_w-4),r2
	movefa	Level.a,r0
	movei	#999,r1
	cmpq	#0,r0
	moveq	#2,YADD_init
	movei	#.cont,LR
	jump	z,(LR)
	subq	#1,r0			// Level 9 == 0 !!

	moveta	r0,Level.a
	move	r0,LR
	neg	r0
	moveq	#5,r1
	addq	#10,r0
	cmpq	#9,r0
	jr	nn,.cont0
	mult	r0,r1
	moveq	#1,YADD_init
.cont0

	movei	#DROP_INIT0,r0
	addq	#1,LR
	mult	LR,r0
	moveta	r0,DropCounter.a
	moveta	r0,DropCounterInit.a
	moveta	r0,DropCounterInit0.a

.cont
	moveta	r1,LinesToGo.a
	movei	#DrawDigitDez3,r0
	BL	(r0)
	POP	LR
;----------------------------------------
;- ShowLevel
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
ShowLevel::
	movei	#(y_level<<16)|(pf_x-digit_w-4),r2
	movefa	Level.a,r1
	neg	r1
	addq	#9,r1
	movei	#DrawDigit,r0
	jump	(r0)
	moveq	#scr_level,r3

;----------------------------------------
;- ShowLines
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
ShowLines::
	PUSH	LR
	movei	#y_lines<<16|(pf_x-3*digit_w-4),r2
	movei	#DrawDigitDez3,r0
	moveq	#scr_lines,r3
	movefa	Lines.a,r1
	BL	(r0)
	movei	#(y_to_go)<<16|(pf_x-3*digit_w-4),r2
	movei	#DrawDigitDez3,r0
	POP	LR
	jump	(r0)
	movefa	LinesToGo.a,r1
;----------------------------------------
;- ShowTime
;-
;- IN: -
;- OUT: -
;- DESTROY: r0,r1,r2,r3,r4,r5,r6
;----------------------------------------
DRAWDIGITDEZ2 	REG 6
Minutes		REG 5
Seconds		REG 4

ShowTime::
	movefa	sec.a,r0
	movefa	last_sec.a,r1
	cmp	r0,r1
	jr	nz,.cont
	moveta	r0,last_sec.a
	jump	(LR)
	nop
.cont
//->	movei	#128*8*4+48,r1
//->	moveta	r1,Cursor.a
//->	movei	#PrintHex,r1
//->	jump	(r1)
//->	nop

	PUSH	LR
	moveq	#30,r1
	movefa	sec.a,r0
	shlq	#1,r1
	move 	r0,Seconds ; sec merken
	div	r1,r0
	move	r0,Minutes
	mult	r1,r0
	sub	r0,Seconds
	movei	#DrawDigitDez2,DRAWDIGITDEZ2

	move	Minutes,r1
	movei	#(y_time<<16)|(pf_x-4*digit_w-6-4),r2
	moveq	#scr_time,r3
	BL	(DRAWDIGITDEZ2)

	movei	#DrawDigit,r0
	moveq	#16,r1
	BL	(r0)
	subq	#2,r2
	POP	LR
	jump 	(DRAWDIGITDEZ2)
	move 	Seconds,r1

UNREG Minutes,Seconds,DRAWDIGITDEZ2
 if 0
;----------------------------------------
;- DebugOut
;- r1 - number
;-
;- OUT:
;- r2 - y+1*digit_h
;-
;- DESTROY: r0,r1
;----------------------------------------
DebugOut::
	movei	#16<<16,r2
DebugOutCont::
	PUSH	r3
	moveq	#0,r3
	addq	#4,r2
DrawDigitHex::
	PUSH	r4,r5,r6
	movei	#DrawDigit,r4
	moveq	#8,r5
.loop
	PUSH	r1
	shrq	#28,r1
	BL	(r4)
	POP	r1
	subq	#1,r5
	movei	#.loop,r0
	jump	nz,(r0)
	shlq	#4,r1
	POP	r4,r5,r6
	shrq	#16,r2
	addq	#digit_h,r2
	shlq	#16,r2
	POP	R3
	RTS
 endif
;----------------------------------------
;- DrawDigitDez4
;- r1 - number
;- r2 - y/x
;- r3 - screen
;-
;- OUT:
;- r2 - x+4*digit_w
;-
;- DESTROY: r0,r1
;----------------------------------------
DrawDigitDez4::
//->	jump	(LR)
//->	nop

	PUSH	r4,LR
	move	r1,r4
	movei	#1000,r0
	div	r0,r1
	PUSH	r1
	mult	r0,r1
	sub	r1,r4		; r4 - remainder
	POP	r1

	movei	#DrawDigit,r0
	BL	(r0)
	move	r4,r1
	POP	r4,LR
	movei	#DrawDigitDez3,r0
	jump	(r0)
	nop
;----------------------------------------
;- DrawDigitDez6
;- r1 - number
;- r2 - y/x
;- r3 - screen
;-
;- OUT:
;- r2 - x+6*digit_w
;-
;- DESTROY: r0,r1
;----------------------------------------
DrawDigitDez6::
	PUSH	LR,r4
	move	r1,r4
	movei	#1000,r0
	div	r0,r1
	PUSH	r1
	mult	r0,r1
	sub	r1,r4		; r4 - remainder


//->	movei	#128*8*6+48,r1
//->	moveta	r1,Cursor.a
//->	POP	r0
//->	movei	#PrintHex,r1
//->	BL	(r1)
//->	move	r4,r0
//->	POP	r4
//->	movei	#PrintHex,r1
//->	POP	LR
//->	jump	(r1)
//->	nop

	POP	r1
	movei	#DrawDigitDez3,r0
	BL	(r0)
	move	r4,r1
	POP	LR,r4

	;; fall thru
;----------------------------------------
;- DrawDigitDez3
;- r1 - number
;- r2 - y/x
;- r3 - screen
;-
;- OUT:
;- r2 - x+3*digit_w
;-
;- DESTROY: r0,r1
;----------------------------------------
DrawDigitDez3::
	PUSH	LR
	PUSH	r2
	movei	#999,r2
	cmp	r1,r2
	jr	nn,.c1
	nop
	move	r2,r1
.c1
	moveq	#25,r2
	shlq	#2,r2
	moveq	#0,r0
	subqt	#1,r0
.l1
	sub	r2,r1
	jr	nn,.l1
	addqt	#1,r0
	add	r2,r1
	POP	r2
	PUSH	r1
	move	r0,r1
	movei	#DrawDigit,r0
	BL	(r0)
	POP	LR,r1
	;; fall thru

;----------------------------------------
;- DrawDigitDez2
;- IN:
;- r1 - number
;- r2 - y/x
;- r3 - screen
;-
;- OUT:
;- r2 - x+2*digit_w
;-
;- DESTROY: r0,r1
;----------------------------------------
DrawDigitDez2::
	PUSH	LR
	moveq	#0,r0
	subqt	#1,r0
.l1
	subq	#10,r1
	jr	nn,.l1
	addqt	#1,r0
	addq	#10,r1
	PUSH	r1
	move	r0,r1
	movei	#DrawDigit,r0
	BL	(r0)
	POP	LR,R1

;----------------------------------------
;- DrawDigit
;-
;- Draw a small (13x22) digit
;- IN:
;- r1 - digit (10 = space)
;- r2 - y/x
;- r3 - screen (0 = score, 1 = level)
;-
;- r2 - x+digit_w
;
;- DESTROY: r0,r1
;----------------------------------------
DrawDigit::
 IF 1
	PUSH 	r2
	WAITBLITTER

	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_FSTEP)
	store	r0,(blitter+_BLIT_A1_FPIXEL)
	store	r0,(blitter+_BLIT_A2_PIXEL)
	store	r0,(blitter+_BLIT_A2_STEP)

	movei	#$10000|(-digit_w&$ffff),r0
	store	r2,(blitter+_BLIT_A1_PIXEL)
	store	r0,(blitter+_BLIT_A1_STEP)

	movei	#ScoreScreen,r2
	cmpq	#0,r3
	jr	eq,.ok
	nop
	movei	#LevelScreen,r2
.ok:

___X::
	movei	#BLIT_PITCH1|BLIT_PIXEL8|BLIT_WID128|BLIT_XADDPIX,r0

	store	r2,(blitter)	; A1 Base
	store	r0,(blitter+_BLIT_A1_FLAGS)
;---------------
	shlq	#2,r1
	movei	#digit_table,r0
	add	r1,r0
	load	(r0),r0
	movei	#BLIT_PITCH1|BLIT_PIXEL8|BLIT_WID4|BLIT_XADDPIX,r1
	store	r0,(blitter+_BLIT_A2_BASE)
	store	r1,(blitter+_BLIT_A2_FLAGS)
;---------------
	movei	#(digit_h<<16)|digit_w,r0
	movei	#BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
//->	WAITBLITTER
	POP	r2
	addqt	#digit_w,r2
 ENDIF
	jump	(LR)
	nop

	include "newstone.inc"
;----------------------------------------
;- Read joypad 1
;-
;- IN: -
;- OUT: r0 - current, r1 - edge
;- DESTROY: r0-r4
;----------------------------------------
readJoy1::
	JOYPAD1
	jump	(LR)
	nop
dumpReg::
	movefa	IRQ_RTS.a,r8
dumpReg1:
	cmpq	#0,dumpFlag
	jr	nz,dumpReg1
	nop



	PUSH	LR,r3,r2,r1
	moveq	#0,r2
	moveta	r2,Cursor.a
	movei	#PrintHex,r22
	BL	(r22)	; r0
	POP	r0
	BL	(r22)	; r1
	POP	r0
	BL	(r22)	; r2
	POP	r0
	BL	(r22)	; r3

//->	move	r4,r0
//->	BL	(r22)
//->	move	r5,r0
//->	BL	(r22)
//->	move	r6,r0
//->	BL	(r22)
//->	move	r7,r0
//->	BL	(r22)

	movefa	IRQ_SP.a,r0
	BL	(r22)

	move	r8,r0
	BL	(r22)

	move	r14,r0
	BL	(r22)	; r14

	POP	r0
	BL	(r22)	; LR

	move	SP,r0
	BL	(r22)

.block
	jr	.block
	moveq	#1,dumpFlag

 IF 0
DEBUG0	reg 30
DEBUG1	reg 29
DEBUG2	reg 28
DEBUG3	reg 27
DEBUG4	reg 26
DEBUG5	reg 25

reg2ram::
	movei $089E9419,DEBUG3
	movei ._addq,DEBUG1
	store DEBUG3,(DEBUG1)
	movei save_regs-4,DEBUG0	; hierhin kommen dir Regs
	moveq #31,DEBUG2	; 32 Register
.loop	addq #32,DEBUG3		; Rn=R(n+1)
._addq	addq #4,DEBUG0
	movefa r0,DEBUG5	; Register-Wert holen
	store DEBUG3,(DEBUG1)	; SelfMode
	subq #1,DEBUG2
	store DEBUG5,(DEBUG0)	; Reg ins RAM
	jr nn,.loop
	nop
 endif
//->lr_check:
//->	move	LR,LR_CHECK
//->	moveta	LR,LAST_LR.a
//->	shrq	#20,LR_CHECK
//->	cmpq	#15,LR_CHECK
//->	movei	#lr_check,LR_CHECK
//->	jump	z,(LR)
//->	nop
//->	movei	#$f00400,r0
//->	loadw	(r0),r1
//->block::
//->	addq	#1,r0
//->	jr	block
//->	store	r1,(r0)

	include "debugprint.inc"
	ENDMODULE MAIN


****************
*    NEWGAME   *
* level selection
	MODULE NEWGAME,MODend_MAIN
	include "newgame.inc"
	ENDMODULE NEWGAME

****************
*     game     *
	include "game.inc"

****************
* MODULE	INIT   *
*
* Only needed at startup, can be overwritten later
*
	MODULE INIT,$f03000+$1000-$180
;---------------
;-     init    -
;---------------
init::
	movei	#$f02100,status
	moveta	status,status

	movei	#1<<14|%11111<<9,r1	; clear all ints, REGPAGE = 1
	store	r1,(status)
	nop
	nop				; wait
	moveta	status,IRQ_FLAGADDR.a

	movei	#IRQ_STACK,SP
	moveta	SP,IRQ_SP.a
	move	SP,IRQ_SP
	subq	#32,SP

	movei	#$f0211c,r0
	moveq	#0,r1
	store	r1,(r0)		; 32bit division

*	init some regs
	movei	#GraphicScreen,screen_ptr
	movei	#GraphicScreen2,screen1_ptr
	movei	#Pattern,pattern_ptr
	moveq	#0,dumpFlag
	movei	#$f02200,blitter
	moveta	blitter,blitter.a

	moveq	#4,r0
	moveta	r0,LastLevel.a
	moveq	#0,r0
	moveta	r0,Rotation.a
	moveta	r0,StoneType.a
	moveta	r0,LinesToGo.a
	moveta	r0,Lines.a
	moveta	r0,Points.a
	moveta	r0,High.a
	moveta	r0,x_pos.a
	moveta	r0,y_pos.a
	moveta	r0,Cursor.a
	moveq	#0,X_Flag
	moveq	#0,Y_Flag
	moveq	#0,PauseFlag
;---------------
	WAITBLITTER
	movei	#$f03000,r0
	movei	#BLIT_PITCH1|BLIT_PIXEL32|BLIT_WID3584|BLIT_XADDPHR,r1
	store	r0,(blitter)
	store	r1,(blitter+_BLIT_A1_FLAGS)
	moveq	#0,r0
	store	r0,(blitter+_BLIT_A1_PIXEL)
	store	r0,(blitter+_BLIT_A1_STEP)
	store	r0,(blitter+_BLIT_PATD)
	store	r0,(blitter+_BLIT_PATD+4)
	movei	#1<<16|($1000-$180)>>2,r0
	movei	#BLIT_PATDSEL,r1
	store	r0,(blitter+_BLIT_COUNT)
	store	r1,(blitter+_BLIT_CMD)
	WAITBLITTER

	INITMODULE irq_sub
	INITMODULE MAIN
;--------------------
	movei	#stone_stat,r0
	moveq	#7,r1
	moveq	#0,r2
.loop
	subq	#1,r1
	store	r2,(r0)
	jr	nz,.loop
	addqt	#4,r0
	movei	#LastJoy,r0
	store	r2,(r0)
	addq	#4,r0
	store	r2,(r0)

;--------------------
;- setup Random seeds
	movei	#seed1,r0
	movei	#$f00004,r1
	load	(r1),r1
	shrq	#17,r1
	store	r1,(r0)
	addq	#4,r0
	store	r1,(r0)
	addq	#4,r0
	store	r1,(r0)

;--------------------
;- Init ms-Timer
	moveq	#0,r2
	moveta	r2,sec.a
	movei	#4*5,r2
	moveta	r2,_50Hz.a
	movei	#$f00050,r0
	movei	#(26591-1)<<16|(1-1),r1
	store	r1,(r0)	; Start
;--------------------
	movei	#dumpReg,r0
	moveta	r0,DUMP_REG.a
	movei	#$f03ffc,r1
	store	r0,(r1)
//->	movei	#lr_check,LR_CHECK
//->	moveq	#0,r0
//->	moveta	r0,LAST_LR.a
	movei #main,r0
	jump (r0)
	nop
	ENDMODULE INIT

 IF MODlen_INIT > $180
	echo "INIT to large, fix start"
 ENDIF
	include "data.inc"

	IF SOUND = 1
	align 8
DSP_start:
	ibytes "lsp_v15.lib"
DSP_end:
	.long
LSP_module_music_data:
	.ibytes "mod/trance_adventure.lsmusic"

	.long
LSP_module_sound_bank:
	.ibytes "mod/trance_adventure.lsbank"
	ENDIF

ende
	echo "End of GPU %xende"
****************
END
