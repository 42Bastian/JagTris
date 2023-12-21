# makefile for Tetris
#

DEMO=tetris

OS:=$(shell uname -s)

TJASS= lyxass
RMAC= rmac
RLN= rln

ifeq ($(OS),Linux)
MAGICK=convert
export MULTIPLY=-evaluate multiply 256
else
MAGICK=magick
export MULTIPLY=#
endif

SOUND?= 1

all: tetris.j64

DIGITS_SMALL=0 1 2 3 4 5 6 7 8 9 A B C D E F double
DIGITS_SMALL:=$(addprefix digits/,$(DIGITS_SMALL))
DIGITS_SMALL:=$(addsuffix .raw,$(DIGITS_SMALL))
SPRITES=digits_small
SPRITES+=digits/level.raw
SPRITES+=digits/time.raw
SPRITES+=digits/score.raw
SPRITES+=digits/high.raw
SPRITES+=digits/lines.raw
SPRITES+=digits/left.raw
SPRITES+=digits/pause.raw
SPRITES+=digits/credits.raw
SPRITES+=digits/title.raw

tetris.equ tetris.o : tetris.js irq_sub.o game.inc globlreg.inc newgame.inc\
	sprite.S sprite_coor.S data.inc newstone.inc joystick.inc constant.S\
	debugprint.inc
	$(TJASS) -w -sh -D SOUND=$(SOUND)  tetris.js

irq_sub.o : irq_sub.js globlreg.inc constant.S
	$(TJASS) -w -sh irq_sub.js

tetris_68k.o: tetris_68k.S sprite_coor.S tetris.equ tetris.o
	$(RMAC) -4 -i$(BJL_ROOT)/68k_inc -i$(BJL_ROOT)/68k_mac tetris_68k.S

tetris.bin: tetris_68k.o
	$(RLN) -z -n -a 40000 x x -o $@ $<

tetris.cof: tetris_68k.o
	$(RLN) -z -e -a 40000 x x -o $@ $<

tetris.j64:  tetris.bin
	cp sbl.XXX $@
	cat tetris.bin >> $@
	bzcat $(BJL_ROOT)/bin/allff.bin.bz2 >> $@
	truncate -s 1M $@

.PHONY: sprites
sprites: pre_sprites $(SPRITES)

.PHONY: clean
clean:
	rm -f tetris.o irq_sub.o tetris.equ irq_sub.equ
	rm -f tetris.bin tetris.pck *.bin *_nos.*
	rm -f *.o *.prn
	rm -f *~

# Jaguar: RRRRrBBBBbGGGGGg

include Rules.newfont

include Rules.launch
