* -*-asm-*-
****************
* global REGister equates
		UNREG LR,SP,LR.a,SP.a
IRQ_SP		REG 31
YADD_init	REG 30
status		REG 29
dumpFlag	REG 28

SP		REG 27
LR		REG 26
screen1_ptr	REG 25
screen_ptr	REG 24
pattern_ptr	REG 23
YADD		REG 22
VBLFlag		REG 21
JOYPAD		REG 20
UPDATE		REG 19
X_Flag		REG 18
Y_Flag		REG 17
PauseFlag	REG 16
blitter		REG 14

	;; R0..R9 => scratch/parameters

****************
IRQ_SP.a	REG 31
IRQ_RTS.a	REG 30
IRQ_FLAGADDR.a	REG 29
IRQ_FLAG.a	REG 28

_50Hz.a		REG 27
ms.a		REG 26
sec.a		REG 25
LinesToGo.a	REG 24
Cursor.a	REG 23 ; debug
StoneType.a	REG 22
Rotation.a	REG 21
High.a		REG 20
Points.a	REG 19
LastLevel.a	REG 18
Lines.a		REG 17
DropCounterInit.a REG 16

blitter.a	REG 14
DUMP_REG.a	REG 13
LAST_LR.a	REG 12
last_sec.a	REG 10
Level.a		REG  9
DropCounterInit0.a REG  8
DropCounter.a	REG  7
y_pos.a		REG  6
x_pos.a		REG  5

IRQScratch4.a	REG  4
IRQScratch3.a	REG  3
IRQScratch2.a	REG  2
IRQScratch1.a	REG  1
IRQScratch0.a	REG  0
