	.68000
START:
	jmp	INIT
	.phrase
//->	.incbin "mod/bubsy2.mod"
//->	.incbin "mod/robric32.mod"
//->	.incbin	"mod/smooth_bs.mod"
	.incbin "mod/trance_adventure.mod"
INIT:
        move    #$2700,sr
	lea     $1000.w,sp
	lea     $F00000,a0
	move.l  #$070007,d0           ;; big endian
	move.l  d0,$210C(a0)
	move.l  d0,$F1A10C
	moveq 	#0,d0
	move.l	d0,$2114(a0)           ;; stop gpu
	move.l	d0,$f1a114             ;; stop dsp
	move.l	d0,$2100(a0)           ;; disable GPU-IRQs
	                              ;; disable DSP-IRQs
	move.l	#%10001111100000000,$f1a100
	move.l	#%111111<<8,$f10020    ;; clear and disable IRQs

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
	move.l  d0,$f10000            ;; stop JPIT1
	move.l  d0,$f10004            ;; stop JPIT2
	move.l  #$1F01<<16,$E0(a0)    ;; clear pending irqs
	move.w  #$7fff,$4e(a0)        ;; no VI
	lea     dummy_irq(pc),a0
	move.l  a0,$0100.w
	bra.s   INIT1
dummy_irq:
        move.l	#$1f00<<16,$f000e0
	rte
INIT1:
	lea	code+4(pc),a1
	move.l	(a1)+,d0
	move.l	#$40000,a0
	bsr	depack
	jmp	$40000

depack:
	lea	(a0,d0.l),a3	// last unpacked byte

	moveq	#0,d2
depack0:
	cmpa.l	a3,a0
	bhs.s	depack9

	move.b	(a1)+,d1	// pack-byte
	beq.s	copy8
	moveq	#7,d0
depack1:
	add.b	d1,d1
	bcc.s	depack2

	move.b	(a1)+,d2	// hi-offset::count
	move.b	(a1)+,d3	// low-offset

	move.w	#$f0,d4
	and.w	d2,d4
	andi.w	#$0F,d2		// r2:= count
	add.w	d2,d2
	lsl.w	#4,d4
	move.b	d3,d4		// r4:= offset

	neg.w	d4
	lea	(a0,d4.w),a2
	jmp	copy(pc,d2.w)

copy:
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+

	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+

	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	dbra	d0,depack1
	bra.s	depack0

depack2:
	move.b	(a1)+,(A0)+
	dbra	d0,depack1
	bra.s	depack0

copy8:
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	bra.s	depack0
depack9:
	rts

code:
	.incbin	"tetris.pck"
