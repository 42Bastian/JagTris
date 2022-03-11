; -*-asm-*-
;----------------------------------------
;- Random
;-
;- IN: -
;- OUT: r0 - new random value
;- DESTROY: r0,r1
;----------------------------------------
PTR	REG 15
SEED3	REG 6
SEED2	REG 5
SEED1	REG 4

Random::
	PUSH r2,r3,r4,r5,r6,r15
	movei	#seed1,PTR
	movei	#206,r2
	load	(PTR),r0
	move	r0,SEED1	; retten
	div	r2,r0		; r0 = (seed1 / 206)
	sub	r0,SEED1	; r2 = (seed1 mod 206)
	movei	#157,r2
	moveq	#21,r3
	mult	r2,SEED1	; r2 = r2*157
	mult	r3,r0		; r0 = r0*21
	sub	r0,SEED1
	jr	nn,.cont
	load	(PTR+4),SEED2
	movei	#206*157,r0
	add	r0,SEED1
.cont
	movei	#217,r2
	store	SEED1,(PTR)
	move	SEED2,r0
	div	r2,r0
	sub	r0,SEED2
	movei	#146,r2
	movei	#45,r3
	mult	r2,SEED2
	mult	r3,r0
	sub	r0,SEED2
	jr	nn,.cont1
	load	(PTR+8),SEED3
	movei	#31727,r0
	add	r0,SEED2
.cont1
	movei	#222,r2
	store	SEED2,(PTR+4)
	move	SEED3,r0
	div	r2,r0
	sub	r0,SEED3
	movei	#142,r2
	movei	#133,r3
	mult	r2,SEED3
	mult	r3,r0
	sub	r0,SEED3
	jr	nn,.cont2
	sub	SEED2,SEED1
	movei	#31657,r0
	add	r0,SEED3
.cont2
	store	SEED3,(PTR+8)
	movei	#706,r0
	cmp	SEED1,r0	; SEED1-SEED2>706
	movei	#32362,r1
	jr	nn_nz,.cont3
	add	SEED3,SEED2
	sub	r1,SEED2
.cont3
	cmpq	#1,SEED2
	jr	nn,.cont4
	nop
	add	r1,SEED2
.cont4
	move	SEED2,r0
	POP	r15,r6,r5,r4,r3,r2
	jump	(LR)
	nop

UNREG PTR,SEED1,SEED2,SEED3
;----------------------------------------
;- NewStone
;-
;- IN: -
;- OUT: new stone in StoneType.a
;- DESTROY: r0,r1,r2,r3
;----------------------------------------
NewStone::
	PUSH	LR
	movei #Random,r3
.loop
	BL	(r3)
	shrq	#1,r0
	shlq	#29,r0
	shrq	#29,r0
	cmpq	#7,r0
	jr	nn,.loop

	moveta	r0,StoneType.a
	shlq	#2,r0
	movei	#stone_stat,r1
	add	r0,r1
	load	(r1),r3
	shlq	#3,r0
	addqt	#1,r3
	store	r3,(r1)

	moveq	#0,r1
	moveta	r1,Rotation.a
	moveq	#20,r1
	shlq	#1,r1
	moveta	r1,x_pos.a
	movei	#steine+4,r1
	add	r0,r1
	load	(r1),r0
	neg	r0
//->	subq	#8,r0	;; TODO: start off screen => need to fix checknewpos
	jr	ShowStat2
	moveta	r0,y_pos.a

;----------------------------------------
;- ShowStat
;-
;- IN: -
;- OUT: new stone in StoneType.a
;- DESTROY: r0,r1,r2,r3,r4,r5,r6,r7
;----------------------------------------
SUM		REG 7
COUNTER		REG 6
PRINTDEZ	REG 5
PTR		REG 4
SCREEN		REG 3
TXT_YX		REG 2

STAT_X	EQU 50

ShowStat::
	PUSH	LR
ShowStat2:
	movei	#stone_stat,PTR
	movei	#DrawDigitDez3,PRINTDEZ
	moveq	#7,COUNTER
	moveq 	#1+2,TXT_YX
	moveq	#1,SCREEN
	moveq	#0,SUM
.loop
	shlq	#16,TXT_YX
	movei	#STAT_X,r1
	or	r1,TXT_YX
	load 	(PTR),r1
	add	r1,SUM
	BL	(PRINTDEZ)
	shrq	#16,TXT_YX
	subq 	#1,COUNTER
	addqt	#22,TXT_YX
	jr	nz,.loop
	addqt	#4,PTR

	movei	#9999,r0
	cmp	r0,SUM
.stop
	jr	z,.stop
	nop

	addqt	#4,TXT_YX
	shlq	#16,TXT_YX
	movei	#STAT_X-digit_w,r1
	movei	#DrawDigitDez4,PRINTDEZ
	or	r1,TXT_YX
	POP	LR
	jump	(PRINTDEZ)
	move	SUM,r1

UNREG PRINTDEZ,SCREEN,TXT_YX,COUNTER,PTR,SUM
