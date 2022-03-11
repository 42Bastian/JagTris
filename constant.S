;- PlayField dimensions (blocks)
PFmax_x	equ 10+2	; 2 for border
PFmax_y	equ 24+1	; 1 for border

;- PlayField dimensions (pixels)
gfx_max_x 	 EQU (PFmax_x*8)
gfx_max_y	 EQU (PFmax_y*8)
gfx_x		 EQU (320-gfx_max_x)>>1

dbg_max_x	EQU 128
dbg_max_y	EQU 200
;- Addresses

_68kvars	equ $1000
GraphicScreen   EQU $00100000
GraphicScreen2  EQU $0010A000
TitleScreen	EQU $00114000
ScoreScreen     EQU $00130000
LevelScreen	EQU $00140000
StatScreen      equ $00150000
DebugScreen	equ $00158000
stone_stat      equ $160000
LastJoy		equ stone_stat+7*4
seed1		EQU LastJoy+3*4

OBL             equ $0009f000              ;; max. 16 Objekte
OBL0            equ $0009f200
Pattern         EQU $000A0000              ;; 13*64 words
Stones          EQU $000B0000
Maske           EQU $000C0000
PlayField       EQU $000F0000

;- Game constants

DROP_INIT0	EQU 60
