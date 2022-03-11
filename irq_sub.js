; -*-asm-*-
****************
* MODUL IRQ
* created 30/07/95
****************
* exports MODrun.irq_sub
*	  MODend.irq_sub
*	  MODlen.irq_sub

GPU		 set 1

AnzObjekte	equ 13

	gpu

	include <js\symbols\blit_eq.js>
	include <js\symbols\jagregeq.js>
	include <js\macro\help.mac>
	include <js\macro\module.mac>
	include <js\macro\joypad1.mac>
	include "constant.S"
****************
* global register equates
	include "globlreg.inc"
****************
* local macros
MACRO IRQ_PUSH
	subqt #4,IRQ_SP.a
	store \0,(IRQ_SP.a)
ENDM
MACRO IRQ_POP
	load (IRQ_SP.a),\0
	addqt #4,IRQ_SP.a
ENDM

	MODULE irq_sub,$f03000
IRQ_START::
;;; ----------------------------------------
;;; CPU interrupt
;;; ----------------------------------------
	org	$f03000
irq0::
	movei	#ill_irq,r0
	load	(IRQ_FLAGADDR.a),IRQ_FLAG.a
	jump	(IRQScratch0.a)
	bclr	#3,IRQ_FLAG.a

;;; ----------------------------------------
;;; DSP interrupt
;;; ----------------------------------------
	org	$f03010
irq1::
	movei	#ill_irq,r0
	load	(IRQ_FLAGADDR.a),IRQ_FLAG.a
	jump	(IRQScratch0.a)
	bclr	#3,IRQ_FLAG.a
;;; ----------------------------------------
;;; Timer interrupt
;;; ----------------------------------------
	org 	$f03020

	load	(IRQ_FLAGADDR.a),IRQ_FLAG.a
	movei	#timer_irq,IRQScratch0.a
	bset	#11,IRQ_FLAG.a
	load	(IRQ_SP.a),IRQ_RTS.a
	jump	(IRQScratch0.a)
	bclr	#3,IRQ_FLAG.a

;;; ----------------------------------------
;;; OBJECT interrupt
;;; ----------------------------------------
	org $f03030

	load	(IRQ_FLAGADDR.a),IRQ_FLAG.a
	movei	#obj_irq,IRQScratch0.a
	bset	#12,IRQ_FLAG.a
	load	(IRQ_SP.a),IRQ_RTS.a
	jump	(IRQScratch0.a)
	bclr	#3,IRQ_FLAG.a

;;; ----------------------------------------
;;; Blitter interrupt
;;; ----------------------------------------
	org	$f03040
irq4::
	movei	#ill_irq,r0
	load	(IRQ_FLAGADDR.a),IRQ_FLAG.a
	jump	(IRQScratch0.a)
	bclr	#3,IRQ_FLAG.a
;;; ----------------------------------------
;;; Illegal interrupt
;;; ----------------------------------------
ill_irq:
	bclr	#6,IRQ_FLAG.a
	bset	#11,IRQ_FLAG.a

	movei	#$f02114,r0
	load	(r0),r0
	moveta	r0,r0

	jump	(DUMP_REG.a)
	store	IRQ_FLAG.a,(IRQ_FLAGADDR.a)

;;; ----------------------------------------
;;; OBJECT interrupt
;;; ----------------------------------------
obj_irq::
	movefa	IRQ_SP,IRQ_SP.a
	addqt	#2,IRQ_RTS.a
;---------------
	movei	#OBF,IRQScratch0.a
	storew	IRQScratch1.a,(IRQScratch0.a)		; resume OP
;---------------
	moveq	#1,IRQScratch1.a
	movei	#OBL,IRQScratch0.a
	moveta	IRQScratch1.a,VBLFlag
	movei	#OBL0,IRQScratch1.a
	movei	#AnzObjekte*4,IRQScratch2.a
	loadp	(IRQScratch1.a),IRQScratch3.a
.loop
	addq	#8,IRQScratch1.a
	subq	#1,IRQScratch2.a
	storep	IRQScratch3.a,(IRQScratch0.a)
	addqt	#8,IRQScratch0.a
	jr	nz,.loop
	loadp	(IRQScratch1.a),IRQScratch3.a
;---------------
.exit
	jump	(IRQ_RTS.a)
	store	IRQ_FLAG.a,(IRQ_FLAGADDR.a)

****************
* ms-timer
****************
timer_irq::
	addqt	#2,IRQ_RTS.a
	movefa	IRQ_SP,IRQ_SP.a
;---------------
	subq	#1,_50Hz.a
	movei	#$f02114,IRQScratch0.a
	jr	nz,.cont0
	load	(IRQScratch0.a),IRQScratch1.a

	moveq	#20,_50Hz.a
	bset	#1,IRQScratch1.a
	store	IRQScratch1.a,(IRQScratch0.a)		; kick CPU-VBL

.cont0
	movefa	PauseFlag,IRQScratch1.a
	cmpq	#0,IRQScratch1.a
	movei 	#.exit,IRQScratch0.a
	jump	nz,(IRQScratch0.a)
	nop

	subq	#1,ms.a
	jr	nz,.cont
	subq	#1,DropCounter.a
	movei	#1000,ms.a
	addqt	#1,sec.a	; sec inc
.cont
	jr	nn,.exit
	movefa	Y_Flag,IRQScratch1.a
	cmpq	#0,IRQScratch1.a
	jr	z,.x1
	nop
	jr	nn,.exit
.x1
	moveq	#8,IRQScratch1.a

	move	DropCounterInit.a,DropCounter.a
	moveta	IRQScratch1.a,Y_Flag
.exit
	jump	(IRQ_RTS.a)
	store	IRQ_FLAG.a,(IRQ_FLAGADDR.a)

 ENDMODULE irq_sub

	END
