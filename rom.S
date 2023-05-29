	.globl	_start
_start:
        move    #$2700,sr
	lea     $1000.w,sp
	lea     $F00000,a0
	move.l  #$070007,d0           // big endian
	move.l  d0,$210C(a0)
	move.l  d0,$F1A10C
	moveq #0,d0
	move.l d0,$2114(a0)           // stop gpu
	move.l d0,$f1a114             // stop dsp
	move.l d0,$2100(a0)           // disable GPU-IRQs
	                              // disable DSP-IRQs
	move.l #%10001111100000000,$f1a100
	move.l #%111111<<8,$f10020    // clear and disable IRQs

	move.l  d0,0.w
	moveq   #4,d0
	move.l  d0,4.w
	moveq   #0,d0
	move.l  d0,$20(a0)            // set OP to STOP-OBJECT
	move.w  d0,$26(a0)            // clear OP-Flag
	move.l  d0,$2A(a0)            // border black
	move.w  d0,$56(a0)            // set CRY mode to color/not BW
	move.w  d0,$58(a0)            // background black
	move.l  d0,$50(a0)            // stop PIT
	move.l  d0,$f10000            // stop JPIT1
	move.l  d0,$f10004            // stop JPIT2

	lea $4000,a0
	lea tetris,a1
.1:
	movem.l	(a1)+,d0-d7/a2-a7
	movem.l d0-d7/a2-a7,(a0)
	lea	14*4(a0),a0
	cmp.l	#tetris_e,a1
	ble .1
	jmp	$4000
	.long
tetris:
 IF SOUND = 1
	.incbin "tetris_final.bin"
 ELSE
	.incbin "tetris_final_nos.bin"
 ENDIF
tetris_e:

	.end
