;-*-asm-*-
	.gpu
	run	$800410

	echo "Start %XSTART_ADDR"
	dc.l START_ADDR
	dc.l datae-data
data:
	ibytes	"tetris.bin.lz4",8
datae
