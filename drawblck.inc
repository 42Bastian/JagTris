* -*-asm-*-
;---------------
; draw stone at x_count,y_count
;---------------
draw_block::
.wait
	load (blitter+$38),r0
	shrq #1,r0
	jr cc,.wait
	nop

	movei #BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID80|BLIT_XADDPHR,r1
	store screen_ptr,(blitter)
	store r1,(blitter+4)
	move y_count,r0
	move x_count,r1
	shlq #3,r0
	shlq #3,r1	; *8
	shlq #16,r0
	or r1,r0
	store r0,(blitter+12)
;---------------
	move color,r0
	shlq #29,r0
	shrq #29-7,r0
	add pattern_ptr,r0
	movei #BLIT_MASK|BLIT_PITCH1|BLIT_PIXEL16|BLIT_WID64|BLIT_XADDPHR,r1
	store r0,(blitter+$24)
	store r1,(blitter+$28)
;---------------
	movei #8<<16|8,r0
	movei #BLIT_UPDA1|BLIT_SRCEN|BLIT_LFU_REPLACE|BLIT_BUSHI,r1
	store r0,(blitter1+$3c)
	store r1,(blitter1+$38)
	RTS
