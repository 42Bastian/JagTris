;-*-asm-*-
*****************
* prints r0 hex at cursor
printchar	reg 4

PrintHex::
	PUSHLR	r3,r4,r1
	movei	#PrintChar,printchar
	move	r0,r1
	moveq	#8,r3
.l1
	move	r1,r0
	shrq	#28,r0

	BL	(printchar)
	subq	#1,r3
	jr	nz,.l1
	shlq	#4,r1

	movefa	Cursor.a,r0
	movei	#128*8-8*4,r1
	add	r1,r0
	moveta	r0,Cursor.a
	POPLR	r3,r4,r1
	unreg printchar
*****************
****************************
* Textscreen with 8x8-Font *
* created : 29.06.95	   *
* (c) Bastian Schick	   *
****************************
;global EQUs
; max_x, max_y must be preset !
;global registers
;Cursor.a
;ScreenPtr.a		; must be initialized !

********************
* Print char in r0 *
********************
CharPtr	reg 5
PChelp2	reg 4
PChelp1	reg 3
PChelp	reg 2
ptr	reg 1

PrintChar::
	PUSH r1,r2,r3,r4,r5
	movefa	Cursor.a,PChelp		; get current cursor
	movei	#DebugScreen,ptr
	add	PChelp,ptr
	shlq	#3+2,r0
	movei	#_dbg0,CharPtr
	add	r0,CharPtr

	movei	#128,PChelp
	moveq	#8,PChelp2
	load	(CharPtr),PChelp1
.l1
	addq	#4,CharPtr
	store	PChelp1,(ptr)
	add	PChelp,ptr
	subq	#1,PChelp2
	jr	nz,.l1
	load	(CharPtr),PChelp1

	movefa	Cursor.a,PChelp
	addq	#4,PChelp
	moveta	PChelp,Cursor.a

	POP r1,r2,r3,r4,r5
	jump	(LR)
	nop
	ALIGN 2
	unreg CharPtr,ptr,PChelp,PChelp1,PChelp2
