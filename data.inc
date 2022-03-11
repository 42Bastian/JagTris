;-*-asm-*-
****************
* extern DATA  *
****************

	align 32
	;; coordinates (x,y) of central block
steine::
; MMM
; M
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
; M
; MMM
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
;  MM
; MM
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
; MM
;  MM
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
; MMMM
	dc.l -16,-8
	dc.l -16,-16
	dc.l -16,-16
	dc.l -8,-16
;  M
; MMM
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
; MM
; MM
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8
	dc.l -8,-8

	MACRO IBIN
	PHRASE
_\0::
	.incbin "digits/\0.raw"
	ENDM

	IBIN level
	IBIN time
	IBIN score
	IBIN high
	IBIN lines
	IBIN left
	IBIN pause
	IBIN title

digits_small::
	IBIN 0
	IBIN 1
	IBIN 2
	IBIN 3
	IBIN 4
	IBIN 5
	IBIN 6
	IBIN 7
	IBIN 8
	IBIN 9
	IBIN A
	IBIN B
	IBIN C
	IBIN D
	IBIN E
	IBIN F
	IBIN double

	PHRASE
digit_table:
	dc.l	_0,_1,_2,_3,_4
	dc.l	_5,_6,_7,_8,_9
	dc.l	_A,_B,_C,_D,_E
	dc.l	_F,_double

sprite_table:
	dc.l	_level_sprite
	dc.l	_score_sprite
	dc.l	_high_sprite
	dc.l	_time_sprite
	dc.l	_lines_sprite
	dc.l	_left_sprite

_level_sprite:
	dc.l	(y_level<<16)|4
	dc.l	(1<<16)|(-level_w & $ffff)
	dc.l	ScoreScreen
	dc.l	_level
	dc.l	(level_h<<16)|(level_w)

_score_sprite:
	dc.l	(y_score<<16)|4
	dc.l	(1<<16)|(-score_w & $ffff)
	dc.l	ScoreScreen
	dc.l	_score
	dc.l	(score_h<<16)|(score_w)

_high_sprite:
	dc.l	(y_high<<16)|4
	dc.l	(1<<16)|(-high_w & $ffff)
	dc.l	ScoreScreen
	dc.l	_high
	dc.l	(high_h<<16)|(high_w)

_time_sprite:
	dc.l	(y_time<<16)|4
	dc.l	(1<<16)|(-time_w & $ffff)
	dc.l	ScoreScreen
	dc.l	_time
	dc.l	(time_h<<16)|(time_w)

_lines_sprite:
	dc.l	(y_lines<<16)|4
	dc.l	(1<<16)|(-lines_w & $ffff)
	dc.l	ScoreScreen
	dc.l	_lines
	dc.l	(lines_h<<16)|(lines_w)

_left_sprite:
	dc.l	(y_to_go<<16)|4
	dc.l	(1<<16)|(-left_w & $ffff)
	dc.l	ScoreScreen
	dc.l	_left
	dc.l	(left_h<<16)|(left_w)

	PHRASE
CheckKey
	dc.l	JOY_0,JOY_1,JOY_2,JOY_3,JOY_4
	dc.l	JOY_5,JOY_6,JOY_7,JOY_8,JOY_9

	dc.b	"Game: (c) 42Bastian Schick",0

	PHRASE
_dbg0:
	dc.l	$04FF0404	;0
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$0404FF04	;1
	dc.l	$04FFFF04
	dc.l	$0404FF04
	dc.l	$0404FF04
	dc.l	$0404FF04
	dc.l	$0404FF04
	dc.l	$0404FF04
	dc.l	$04040404

	dc.l	$04FF0404	;2
	dc.l	$FF04FF04
	dc.l	$0404FF04
	dc.l	$04FF0404
	dc.l	$04FF0404
	dc.l	$FF040404
	dc.l	$FFFFFF04
	dc.l	$04040404

	dc.l	$04FF0404	;3
	dc.l	$FF04FF04
	dc.l	$0404FF04
	dc.l	$04FF0404
	dc.l	$0404FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$FF040404	;4
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FFFFFF04
	dc.l	$0404FF04
	dc.l	$0404FF04
	dc.l	$0404FF04
	dc.l	$04040404

	dc.l	$FFFFFF04	;5
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$04FF0404
	dc.l	$0404FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$04FF0404
	dc.l	$FF04FF04
	dc.l	$FF040404
	dc.l	$FFFF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$FFFFFF04
	dc.l	$FF04FF04
	dc.l	$0404FF04
	dc.l	$04FF0404
	dc.l	$04FF0404
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$04040404

	dc.l	$04FF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$04FF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$04FFFF04
	dc.l	$0404FF04
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$04FF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FFFFFF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$04040404

	dc.l	$FFFF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FFFF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FFFF0404
	dc.l	$04040404

	dc.l	$04FF0404
	dc.l	$FF04FF04
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$FF04FF04
	dc.l	$04FF0404
	dc.l	$04040404

	dc.l	$FFFF0404
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FF04FF04
	dc.l	$FFFF0404
	dc.l	$04040404


	dc.l	$FFFFFF04
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$FFFF0404
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$FFFFFF04
	dc.l	$04040404

	dc.l	$FFFFFF04
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$FFFF0404
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$FF040404
	dc.l	$04040404
