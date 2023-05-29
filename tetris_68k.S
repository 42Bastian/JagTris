;;*****************************************************************************
;; Autor   :  Bastian Schick                                                  *
;;*****************************************************************************
;; history                                                                    *
;; 02.10.95      An die neue Entwicklungsumgebung angepasst                   *
;; 25.01.96      Für Veröfentlichung vorbereitet                              *
;; 26.01.96      Light-Font eingebaut                                         *
;; 27.01.96      VI rausgeschmissen und dafür GPU-Objekt eingeführt            *
;;               Player-Timing auf 50Hz für ms-Timer der GPU                  *
;; 07.05.96      Anpassungen für NEWSRC                                       *
;; 23.09.96      Neu Keyboard-Rountine eingebaut                              *
;;               Fehler in ClearPf                                            *
;;*****************************************************************************

	.68000

RESET_FADE = 1

	.text
	.globl _start
_start:

	.include "blit_eq.inc"
	.include "tetris.equ"
	.include "help.mac"
	.include "sprite_coor.S"
	.include "constant.S"

;;****************
;;  Variablen    *
	RSRESET
	RSL ResetFlag	;don't move!!
	RSL MuteFlag
	RSW count_obj
	RSW a_hde
	RSW a_hdb
	RSW a_vdb
	RSW a_vde
	RSL Cursor
	RSL StackBottom,$200
	RSL stack
vars_len        = RSCOUNT


;;****************

VID_MODE        = $6c1		; CRY

start:
	jmp	INIT
	dphrase
	dc.l	0
;;****************
;;    GPU-Code   *
GPUcode:
	.incbin "tetris.o"
	.phrase

;;-----------------
INIT:
	lea     _68kvars,a6
	lea     (a6),a0
	move.w  #vars_len>>2,d0
clear_var:
	clr.l   (a0)+
	dbra    d0,clear_var

	lea     stack(a6),sp

	lea	$f00400,a0
	moveq	#127,d0
	moveq	#0,d1
clear_clut:
	move.l	d1,(a0)+
	dbra	d0,clear_clut

	lea GraphicScreen,a0
	move.w #(gfx_max_x*gfx_max_y*2)>>1,d0
clear_scr:
	clr.l (a0)+
	dbra d0,clear_scr

	lea GraphicScreen2,a0
	move.w #(gfx_max_x*gfx_max_y*2)>>1,d0
clear_scr2:
	clr.l (a0)+
	dbra d0,clear_scr2

	lea TitleScreen,a0
	lea	_title,a1

	move.w #(96*200>>2)-1,d0
clear_scr3:
	clr.l	(a0)+
	dbra d0,clear_scr3

	lea ScoreScreen,a0
	move.w #((128*210)>>2)-1,d0
clear_scr4:
	clr.l	(a0)+
	dbra d0,clear_scr4

	lea LevelScreen,a0
	move.w #((128*210)>>2)-1,d0
clear_scr5:
	clr.l	(a0)+
	dbra d0,clear_scr5

	lea StatScreen,a0
	move.w #(4*8*20*8)>>1,d0
clear_scr6:
	clr.l	(a0)+
	dbra d0,clear_scr6

	lea DebugScreen,a0
	move.w #(dbg_max_x*dbg_max_y)>>1,d0
clear_scr7:
	move.l #0,(a0)+
	dbra d0,clear_scr7

	bsr     VideoInit
	bsr     InitOP
	bsr     MakePattern
	bsr     MakeStones

	bsr	SetupCLUT

	moveq	#1,d0
	moveq	#2,d1
	moveq	#3,d2
	moveq	#4,d3
	moveq	#5,d4
	moveq	#6,d5
	moveq	#7,d6
	moveq	#8,d7
	move.l	d0,a0
	move.l	d1,a1
	move.l	d2,a2
	move.l	d3,a3
	move.l	d4,a4
	move.l	d5,a5
	move.l	a0,usp
	clr.l	ResetFlag(a6)
	clr.l	MuteFlag(a6)
	lea     $00F02114,a3
	move.l	#startGPU,-4(a3)
	move.l  #0<<11|1,(a3)

.loop:
	move.l	$f02110,a0
	move	a0,usp
        stop    #$2000
	tst.l   ResetFlag(a6)
	beq.s   .loop
;;****************
;;   reset       *
reset:
 IF RESET_FADE = 1
reset0:
	move.w   #255,d6
	lea     $00F00400,a1
	moveq   #0,d5
reset1:
        move.w  (a1),d0
	move.w	d0,d1
	andi.w	#$ff,d1
	beq.s	reset2
	addq	#1,d5
	subq	#4,d1
	bpl.s	reset2
	moveq	#0,d1
reset2:
	clr.b	d0
	or.w	d1,d0
	move.w	d0,(a1)+
	dbra    d6,reset1
	moveq	#0,d6
reset3:
	stop 	#$2000
	dbra    d6,reset3
	tst.w 	d5
	bne.s 	reset0
 ENDIF
	lea     $F00000,a0
	lea	$F10000,a1
	moveq 	#0,d0
	move.l	d0,$2114(a0)         ;; stop gpu
	move.l	d0,$a114(a1)         ;; stop dsp
	move.l	d0,$2100(a0)           ;; disable GPU-IRQs
	                              ;; disable DSP-IRQs
	move.l	#%10001111100000000,$a100(a1)
	move.l	#%111111<<8,$0020(a1)    ;; clear and disable IRQs

	move.l  d0,0.w
	moveq   #4,d0
	move.l  d0,4.w
	moveq   #0,d0
	move.l  d0,$20(a0)            ;; set OP to STOP-OBJECT
	move.w  d0,$26(a0)            ;; clear OP-Flag
	move.l  d0,$2A(a0)            ;; border black
	move.w  d0,$56(a0)            ;; set CRY mode to color/not BW
	move.w  d0,$58(a0)            ;; background black
	move.l  d0,$50(a0)            ;; stop PIT
	move.l  d0,$0000(a1)          ;; stop JPIT1
	move.l  d0,$0004(a1)          ;; stop JPIT2
	move.l  #$1F01<<16,$E0(a0)    ;; clear pending irqs
	move.w  #$7fff,$4e(a0)        ;; no VI
	lea     dummy_irq2(pc),a0
	move.l  a0,$0100.w

	move.l $800404,a0
	jmp (a0)

dummy_irq2:
        move.l	#$1f00<<16,$f000e0
	rte
;;----------------------------------------
;; Setup CLUT
;;----------------------------------------
SetupCLUT:
	move.l	#$880088f0,$f00400
	move.l	#$0000F0f0,$f00400+254*2

	lea	$f00400+16*2,a0
	move.w	#$8800,(a0)+
	move.w	#$8840,(a0)+
	move.w	#$88e0,(a0)+
	move.w	#$88fe,(a0)+

	lea	$f00400+24*2,a0
	move.w	#$8800,(a0)+
	move.w	#$8840,(a0)+
	move.w	#$88e0,(a0)+
	move.w	#$88fe,(a0)+

	lea	$f00400+32*2,a0
	move.w	#$8800,d0
	moveq	#11,d1
.l2:
	move.w	d0,(a0)+
	add.w	#256/12,d0
	dbra	d1,.l2
	clr.l	$f00400+(34*2)
	move.w	#$F020,(a0)+
	move.w	#$F040,(a0)+
	move.w	#$F080,(a0)+
	move.w	#$F0fe,(a0)+

	rts
;;****************
;;  MakePattern  *
MakePattern:
	movem.l d0/d5-a2,-(sp)
	lea     Pattern,a0
	moveq   #63,d0
mp9:
        clr.w   (a0)+
	dbra    d0,mp9

	lea     Colors(pc),a1
	moveq   #6,d7
mp0:
        lea     maske(pc),a2
	move.w  (a1)+,d0
	moveq   #63,d5
mp1:
        move.b  (a2)+,d0
	bclr    #0,d0
	move.w  d0,(a0)+
	dbra    d5,mp1
	dbra    d7,mp0
	move.w  (a1)+,d0         ;; border color
	moveq   #4,d7
mp2:
        moveq   #63,d5
mp3:
        move.b  (a2)+,d0
	not.b   d0
	move.w  d0,(a0)+
	dbra    d5,mp3
	dbra    d7,mp2
	movem.l (sp)+,d0/d5-a2
	rts
Colors:
        DC.W $F000,$8800,$0F00,$4F00,$C700,$FF00,$0000,$8000
maske:

	DC.L $FFFFFFFF,$FFFFFFFF
	DC.L $FFDDDDDD,$DDDDDDFF
	DC.L $FFDDBBBB,$BBBBDDFF
	DC.L $FFDDBB77,$77BBDDFF
	DC.L $FFDDBB77,$77BBDDFF
	DC.L $FFDDBBBB,$BBBBDDFF
	DC.L $FFDDDDDD,$DDDDDDFF
	DC.L $FFFFFFFF,$FFFFFFFF
;; border left
	.rept 8
	DC.L $FFDDBB99,$77553311
	.endr

;; border right
	.rept 8
	DC.L $11335577,$99BBDDFF
	.endr

;; border bottom
	DC.L $11111111,$11111111
	DC.L $33333333,$33333333
	DC.L $55555555,$55555555
	DC.L $77777777,$77777777
	DC.L $99999999,$99999999
	DC.L $BBBBBBBB,$BBBBBBBB
	DC.L $DDDDDDDD,$DDDDDDDD
	DC.L $FFFFFFFF,$FFFFFFFF
;; corner left
	DC.L $FFDDBB99,$77553311
	DC.L $FFDDBB99,$77553333
	DC.L $FFDDBB99,$77555555
	DC.L $FFDDBB99,$77777777
	DC.L $FFDDBB99,$99999999
	DC.L $FFDDBBBB,$BBBBBBBB
	DC.L $FFDDDDDD,$DDDDDDDD
	DC.L $FFFFFFFF,$FFFFFFFF
;; corner right
	DC.L $11335577,$99BBDDFF
	DC.L $33335577,$99BBDDFF
	DC.L $55555577,$99BBDDFF
	DC.L $77777777,$99BBDDFF
	DC.L $99999999,$99BBDDFF
	DC.L $BBBBBBBB,$BBBBDDFF
	DC.L $DDDDDDDD,$DDDDDDFF
	DC.L $FFFFFFFF,$FFFFFFFF
;;***************
;;  MakeStones   *
MakeStones:
	movem.l d0-a6,-(sp)
	moveq   #6,d7            ;; 7 stones
	lea     Stones,a0
	lea     Pattern,a2       ;; Null
	lea     128(a2),a3       ;; pattern
	move.l  a3,d3
	lea     Maske,a6
	lea     StonesRaw(pc),a5

ms0:
        move.w  #(4*4)-1,d6      ;; 4*4 blocks variations
ms1:
        movea.l d3,a3            ;; patter merken
	moveq   #7,d5            ;; a 8 pixel lines
ms2:
        moveq   #3,d4            ;; 4 blocks
	lea     (a5),a4          ;; current line
ms3:
        lea     (a2),a1          ;; empty
	tst.b   (a4)+            ;; or color
	beq.s   ms_c0            ;; <>0 => empty
	lea     (a3),a1          ;; current color
ms_c0:
        move.l  (a1)+,(a0)+
	move.l  (a1)+,(a0)+      ;; 8 Pixel kopieren
	move.l  (a1)+,(a0)+
	move.l  (a1)+,(a0)+
	dbra    d4,ms3           ;; next block
	lea     16(a3),a3
	dbra    d5,ms2           ;; next line

	moveq   #0,d0
        .rept 4
	move.b  (a5)+,d0
	move.l  d0,(a6)+
        .endr
	dbra    d6,ms1
	move.l  a3,d3
	dbra    d7,ms0
	movem.l (sp)+,d0-a6
	rts
StonesRaw:
	DC.L $0000FF00
	DC.L $FFFFFF00
	DC.L $00000000
	DC.L $00000000

	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00FFFF00
	DC.L $00000000

	DC.L $00000000
	DC.L $FFFFFF00
	DC.L $FF000000
	DC.L $00000000

	DC.L $FFFF0000
	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00000000
;;--------------
	DC.L $FF000000
	DC.L $FFFFFF00
	DC.L $00000000
	DC.L $00000000

	DC.L $00FFFF00
	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00000000

	DC.L $00000000
	DC.L $FFFFFF00
	DC.L $0000FF00
	DC.L $00000000

	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $FFFF0000
	DC.L $00000000
;;---------------
	DC.L $00FFFF00
	DC.L $FFFF0000
	DC.L $00000000
	DC.L $00000000

	DC.L $00FF0000
	DC.L $00FFFF00
	DC.L $0000FF00
	DC.L $00000000

	DC.L $00000000
	DC.L $00FFFF00
	DC.L $FFFF0000
	DC.L $00000000

	DC.L $FF000000
	DC.L $FFFF0000
	DC.L $00FF0000
	DC.L $00000000
;;---------------
	DC.L $FFFF0000
	DC.L $00FFFF00
	DC.L $00000000
	DC.L $00000000

	DC.L $0000FF00
	DC.L $00FFFF00
	DC.L $00FF0000
	DC.L $00000000

	DC.L $00000000
	DC.L $FFFF0000
	DC.L $00FFFF00
	DC.L $00000000

	DC.L $00FF0000
	DC.L $FFFF0000
	DC.L $FF000000
	DC.L $00000000
;;---------------
	DC.L $00000000
	DC.L $FFFFFFFF
	DC.L $00000000
	DC.L $00000000

	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00FF0000

	DC.L $00000000
	DC.L $FFFFFFFF
	DC.L $00000000
	DC.L $00000000

	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00FF0000
	DC.L $00FF0000
;;---------------
	DC.L $00FF0000
	DC.L $FFFFFF00
	DC.L $00000000
	DC.L $00000000

	DC.L $00FF0000
	DC.L $00FFFF00
	DC.L $00FF0000
	DC.L $00000000

	DC.L $00000000
	DC.L $FFFFFF00
	DC.L $00FF0000
	DC.L $00000000

	DC.L $00FF0000
	DC.L $FFFF0000
	DC.L $00FF0000
	DC.L $00000000
;;---------------
	DC.L $FFFF0000
	DC.L $FFFF0000
	DC.L $00000000
	DC.L $00000000

	DC.L $FFFF0000
	DC.L $FFFF0000
	DC.L $00000000
	DC.L $00000000

	DC.L $FFFF0000
	DC.L $FFFF0000
	DC.L $00000000
	DC.L $00000000

	DC.L $FFFF0000
	DC.L $FFFF0000
	DC.L $00000000
	DC.L $00000000
;;*****************
;; Init- und Hilfsroutine für den Bildschirm
;; (c) 1994 Bastian Schick
;;****************
;; Objekt-Defs   *
NEXT_OBJ         = 1
;;**************************************
;; BitMap oder Scaled BitMap - Objekte *
;;**************************************
	RSRESET
	RSW obj_bm_type                ;; Type
	RSW obj_bm_l                   ;; size of object (für MakeOBL)
	RSL obj_bm_data                ;; Data
	RSW obj_bm_x                   ;; X
	RSW obj_bm_y                   ;; Y pos in half lines
	RSW obj_bm_height              ;; height or cc if BRANCH OBJECT
	RSW obj_bm_link                ;; next object
	RSB obj_bm_bits                ;; Bits per Pixel
	RSB obj_bm_pitch               ;; Pitch
	RSW obj_bm_dwidth              ;; width in Phrases
	RSW obj_bm_iwidth              ;; display width (sichtbar)
	RSB obj_bm_cindex              ;; color offset
	RSB obj_bm_flags               ;; Flags
	RSW obj_bm_firstpx             ;; FirstPixel
obj_bm_len      = RSCOUNT

	RSB obj_dummy1
	RSB obj_sc_rem
	RSB obj_sc_vscale
	RSB obj_sc_hscale
obj_sc_len      = RSCOUNT

;;*************
;; GPU-Object *
;;*************
	RSRESET
	RSW obj_gp_type
	RSW obj_gp_l                   ;; size
	RSL obj_gp_data1
	RSL obj_gp_data2
	RSW obj_gp_link

obj_gp_len      = RSCOUNT
;;****************
;; Branch-Object *
;;****************
	RSRESET
	RSW obj_br_type
	RSW obj_br_l                   ;; size
	RSW obj_br_y
	RSW obj_br_cc                  ;; condition
	RSW obj_br_link,2
obj_br_len      = RSCOUNT

	macro BRA_OBJ y_pos,cc,dest
	dc.w 3,obj_br_len
	dc.w \y_pos         ;; y
	if \cc = "<"
	dc.w 1
	else
	if \cc = ">"
	dc.w 2
	else
	if \cc = "="
	dc.w 0
	endif
	endif
	endif
	dc.w \dest,NEXT_OBJ
	endm

;;**************
;; Stop-Object *
;;**************
	RSRESET
	RSW obj_st_type
	RSW obj_st_l
	RSW obj_st_int
	RSL obj_st_data1
	RSL obj_st_data2
obj_st_len      = RSCOUNT
;;****************
;;    MakeOBL    *
MakeOBL:
        movem.l d0-a4,-(sp)

	move.l  #$FF000000,d4

	lea 	OBL0,a0
	lea 	OBL,a1
	lea     (a1),a4          ;; save start
	moveq   #0,d7            ;; object counter

	lea     Objekte(pc),a2    ;; source
loop_mo:
        lea     (a2),a3          ;; save current object
	movem.w (a2)+,d1/d5      ;; Type
	tst.w   d1
	bpl.s   ok_mo
	cmp.w   #$AFFE,d1
	beq     exit_mo
	lea     -4(a2,d5.w),a2
	bra.s   loop_mo

ok_mo:
        addq.w  #1,d7
	and.w   #7,d1
	ext.l   d1
	move.w  d1,d0
	subq.w  #2,d0
	bmi     bm_sc_mo
	beq     gpu_mo           ;; 2=GPU-Object
	subq.w  #2,d0
	bmi.s   branch_mo        ;; 3=Branch Object
;; create Stop-Objekt
	moveq   #1,d0
	and.w   (a2)+,d0
	lsl.w   #3,d0
	or.w    d0,d1            ;; Interrupt Flag
	movem.l (a2)+,d0/d2

	lsl.l   #4,d2
	or.l    d2,d1
	move.l  d0,(a0)+
	move.l  d1,(a0)+
	lea     24(a0),a0
	lea     32(a1),a1
	bra.s   loop_mo
;; branch_object
branch_mo:
        move.w  #$07FF,d0
	and.w   (a2)+,d0
	lsl.w   #3,d0
	or.w    d0,d1            ;; Ypos
	moveq   #7,d0
	and.w   (a2)+,d0
	swap    d0
	lsr.l   #2,d0
	or.l    d0,d1            ;; CC

	move.l  a1,d3            ;; current position
	move.w  (a2)+,d0         ;; next object if CC true
	bpl.s   ok_bra_mo
	neg.w   d0
	move.l  a4,d3            ;; else absolute
ok_bra_mo:
        asl.w   #5,d0            ;; * 32
	ext.l   d0
	add.l   d3,d0
	lsr.l   #3,d0
	ror.l   #8,d0
	move.l  d0,d3
	and.l   d4,d0
	or.l    d0,d1            ;; Link (24..31)
	move.l  d3,d0
	and.l   #$00FFFFFF,d0
	move.l  d0,(a0)+
	move.l  d1,(a0)+

	move.w  #($07FF<<3)|3,d1 ;; Branch Always
	ext.l   d1
	move.w  (a2)+,d0         ;; next object
	asl.w   #5,d0            ;; * 32
	ext.l   d0
	add.l   a1,d0
	lsr.l   #3,d0
	ror.l   #8,d0
	move.l  d0,d3
	and.l   d4,d0
	or.l    d0,d1            ;; Link (24..31)
	move.l  d3,d0
	and.l   #$00FFFFFF,d0
	move.l  d0,(a0)+
	move.l  d1,(a0)+
	lea     16(a0),a0
	lea     32(a1),a1
	bra     loop_mo
;; GPU Object
gpu_mo:
        movem.l (a2)+,d0/d1
	and.w   #$FFF8,d1
	addq #2,d1
	move.l  d0,(a0)+
	move.l  d1,(a0)+

	move.w  #($07FF<<3)|3,d1 ;; Branch Always
	ext.l   d1
	move.w  (a2)+,d0         ;; next object
	lsl.w   #5,d0            ;; * 32
	ext.l   d0
	add.l   a1,d0
	lsr.l   #3,d0
	ror.l   #8,d0
	move.l  d0,d3
	and.l   d4,d0
	or.l    d0,d1            ;; Link (24..31)
	move.l  d3,d0
	and.l   #$00FFFFFF,d0
	move.l  d0,(a0)+
	move.l  d1,(a0)+
	lea     16(a0),a0
	lea     32(a1),a1
	bra     loop_mo

;; BitMap or Scaled BitMap
bm_sc_mo:
        moveq   #-8,d0
	and.l   (a2)+,d0         ;; Data-Adress (3..23)
	bpl.s   ok_bm_sc_mo
	neg.l d0
	add.l   a6,d0
ok_bm_sc_mo:
	lsl.l   #8,d0            ;; an 63..43

	move.w  (a2)+,d6         ;; X
	and.w   #$0FFF,d6        ;; im 2. Phrase 0..11
	ext.l   d6

	move.w  (a2)+,d3
	and.w   #$07FF,d3
	lsl.w   #3,d3
	or.w    d3,d1            ;; Y-Pos (3..13)

	moveq   #0,d3
	move.w  (a2)+,d3
	and.w   #$03FF,d3
	swap    d3
	lsr.l   #2,d3
	or.l    d3,d1            ;; height (14..23)

	move.w  (a2)+,d3         ;; next Objekt
	lsl.w   #5,d3            ;; * 32
	ext.l   d3
	add.l   a1,d3
	lsr.l   #3,d3
	ror.l   #8,d3
	move.l  d3,d2            ;; save Link
	and.l   d4,d3
	or.l    d3,d1            ;; Link (24..31)

	move.l  d2,d3
	and.l   #$00FFFFFF,d3
	or.l    d3,d0
	move.l  d0,(a0)+
	move.l  d1,(a0)+

	moveq   #7,d0
	and.b   (a2)+,d0
	ror.w   #4,d0
	or.w    d0,d6            ;; Depth (12..14)

	moveq   #7,d0
	and.b   (a2)+,d0
	swap    d0
	lsr.l   #1,d0
	or.l    d0,d6            ;; Pitch (15..17)

	moveq   #0,d0
	move.w  (a2)+,d0
	and.w   #$03FF,d0
	lsl.w   #2,d0
	swap    d0
	or.l    d0,d6            ;; Dwidth (18..27)

	moveq   #0,d0
	move.w  (a2)+,d0
	and.w   #$03FF,d0
	ror.l   #4,d0
	move.l  d0,d1
	clr.w   d0
	or.l    d0,d6            ;; IWidth (28..31)
	ext.l   d1               ;; and (32..37)

	move   #$fe,d0
	and.b   (a2)+,d0
	lsl.w   #5,d0
	or.w    d0,d1            ;; Index (38..44)

	moveq   #$0F,d0
	and.b   (a2)+,d0
	swap    d0
	lsr.l   #3,d0
	or.l    d0,d1            ;; Flags (45..48)

	moveq   #$3F,d0
	and.w   (a2)+,d0

	add.w   d0,d0
	swap    d0
	or.l    d0,d1
	move.l  d1,(a0)+
	move.l  d6,(a0)+

	lea     16(a0),a0
	lea     32(a1),a1
	cmp.w   #obj_sc_len,d5   ;; Sized Object
	bne     loop_mo
	move.l  (a2)+,-12(a0)
;;                  move.b  d0,-8(a0)
;;                  move.b  d1,-9(a0)
;;                  move.b  d2,-10(a0)
;;                  clr.b   -11(a0)
	bra     loop_mo
exit_mo:
	move.l	#$affedead,(a0)+
	moveq   #4,d0
	move.l  d0,(a0)+

	addq.w  #1,d7
	move.w  d7,count_obj(a6)
	movem.l (sp)+,d0-a4
	rts
;;****************
;;    CopyOBL    *
CopyOBL:
        movem.l d1/a0,-(sp)
	lea	OBL,a0
	lea	OBL0,a1
	move.w	count_obj(a6),d1
.1:
        .rept 8
	move.l	(a1)+,(a0)+
        .endr
	dbra 	d1,.1
	movem.l	(sp)+,d1/a0
	rts
;;****************
;;     InitOP    *
InitOP:
        lea     $F00000,a0
	moveq	#$fffffffe,d0
	and.w	a_vde(a6),d0

	;; patch object list with VDE
	lea 	bra1+obj_br_y(pc),a1      ;; branch if count > vde
	move.w 	d0,(a1)
	lea 	bra2+obj_br_y(pc),a1      ;; branch if count = vde (GPU-Object)
	move.w 	d0,(a1)

	addq.w	#1,d0
	move.w  d0,$48(a0)            ;; vde

	bsr     MakeOBL
	bsr     CopyOBL
	move.l	#OBL,d0
	swap    d0
	move.l  d0,$20(a0)
	move.w 	#VID_MODE,$28(a0)
	lea     my_irq(pc),a1
	move.l  a1,$0100.w
	move.w	#$2000,sr
	move.l  #$1F02<<16,$E0(a0)
	rts
;;****************
;;    objekt     *
Y_POS           equ 65
X_POS		equ 28

Objekte:

;; 0
	BRA_OBJ 30,"<",-12             ;; y<30 => STOP
bra1:
	BRA_OBJ 600,">",-12             ;; y>600 => STOP
;; 2
bra2:
	BRA_OBJ 600,"=",-11            ;; GPU_OBJ
;; 3
;; Score-Screen
	DC.W 1,obj_sc_len
	DC.L ScoreScreen
	DC.W X_POS+gfx_x-120
score_y:
	DC.W Y_POS
	DC.W 210-1
	DC.W NEXT_OBJ
	DC.B 3,1                   ;; 8 Bit, Pitch 1
	DC.W 128/8,120/8
	DC.B 0,0
	DC.W 0
	DC.B 0,$20,$20,$20		;; %010.00000 => 2x
;; 4
	DC.W 1,obj_sc_len
	DC.L LevelScreen
	DC.W X_POS+gfx_x+gfx_max_x
level_y:
	DC.W Y_POS
	DC.W 210-1
	DC.W NEXT_OBJ
	DC.B 3,1                   ;; 8 Bit, Pitch 1
	DC.W 128/8,112/8
	DC.B 0,0
	DC.W 0
	DC.B 0,$20,$20,$20		;; %010.00000 => 2x
;; 5 debug
	DC.W 0,obj_bm_len
	DC.L DebugScreen
	DC.W 24,Y_POS+45              // x_start,y_start
	DC.W dbg_max_y
	DC.W NEXT_OBJ
	DC.B 3,1
	DC.W dbg_max_x>>3,dbg_max_x>>3
	DC.B 0,4
	DC.W 0

;; 6
	DC.W 0,obj_bm_len
	DC.L GraphicScreen
	DC.W X_POS+gfx_x
gfx1_y:
	DC.W Y_POS
	DC.W gfx_max_y
	DC.W NEXT_OBJ
	DC.B 4,1
	DC.W gfx_max_x*2/8,gfx_max_x*2/8
	DC.B 0,0
	DC.W 0
;; 7
	DC.W 0,obj_bm_len
	DC.L GraphicScreen2
	DC.W X_POS+gfx_x
gfx2_y:
	DC.W Y_POS
	DC.W gfx_max_y
	DC.W NEXT_OBJ
	DC.B 4,1
	DC.W gfx_max_x*2/8,gfx_max_x*2/8
	DC.B 0,4
	DC.W 0
;; 8
	DC.W 0,obj_bm_len
	DC.L TitleScreen
	DC.W X_POS+gfx_x
title_y:
	DC.W Y_POS
	DC.W gfx_max_y+2
	DC.W NEXT_OBJ
	DC.B 3,1
	DC.W gfx_max_x/8,gfx_max_x/8
	DC.B 0,4
	DC.W 0
;; 9
	DC.W 0,obj_bm_len
	DC.L StatScreen
	DC.W X_POS+gfx_x+gfx_max_x+10
stat_y:
	DC.W Y_POS+2
	DC.W 20*8
	DC.W NEXT_OBJ
	DC.B 4,1
	DC.W (4*8)*2/8,(4*8)*2/8
	DC.B 0,4
	DC.W 0
;; 10
	dc.w 4,obj_st_len
	dc.w 0
	dc.l $1234,0
;; 11
	dc.w 2,obj_gp_len
	dc.l 0,0
	dc.w NEXT_OBJ
;; 12
	dc.w 4,obj_st_len
	dc.w 0
	dc.l $1234,0
;; 13
	DC.W $AFFE                 ;; generates STOPOBJ

;;****************
;;   videoinit   *

;; THESE ARE THE NTSC DEFINITIONS
ntsc_width      = 1409
ntsc_hmid       = 823

ntsc_height     = 241
ntsc_vmid       = 266

;; THESE ARE THE PAL DEFINITIONS
pal_width       = 1381
pal_hmid        = 843

pal_height      = 287
pal_vmid        = 322

VideoInit:
        movem.l d0-d3/a0,-(sp)
	lea     $00F00000,a0
	move.w  #pal_vmid-pal_height,d0      	;; vdb
	move.w  #pal_vmid+pal_height,d1      	;; vde
	move.w  #((pal_width>>1)-1)|$0400,d2	;; hde
	move.w  #pal_hmid-(pal_width>>1)+4,d3 	;; hdb
	btst    #4,$00F14003
	beq.s   VideoInit1       		;; =0 => PAL
	move.w  #ntsc_vmid-ntsc_height,d0
	move.w  #ntsc_vmid+ntsc_height,d1
	move.w  #((ntsc_width>>1)-1)|$0400,d2
	move.w  #ntsc_hmid-(ntsc_width>>1)+4,d3
VideoInit1:
	move.w  d0,$0046(a0)		; VDB
	move.w  #$ffff,$0048(a0)	; VDE
	move.w  d2,$003C(a0)		; HDE
	move.w  d3,$0038(a0)		; HDB1
	move.w  d3,$003A(a0)		; HDB2

	move.w  d0,a_vdb(a6)
	move.w  d1,a_vde(a6)

	lea	score_y(pc),a0
	add.w	d0,(a0)
	add.w	d0,level_y-score_y(a0)
	add.w	d0,gfx1_y-score_y(a0)
	add.w	d0,gfx2_y-score_y(a0)
	add.w	d0,title_y-score_y(a0)
	add.w	d0,stat_y-score_y(a0)

	movem.l (sp)+,d0-d3/a0
	rts
;;****************
;;       IRQ     *
my_irq:
	movem.l	d0/a6,-(sp)
	btst    #1,$00F000E1          ;; GPU-Interrupt ?
	move.l  #$0202<<16,$00F000E0
	movem.l (sp)+,d0/a6
	rte
	.phrase
jag_end:
