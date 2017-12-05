#CC=mips-linux-gnu-gcc
#CC=gcc
#CC=arm-linux-gcc
CC=arm-linux-gnueabihf-gcc
CFLAG=-O3  -o
myzbar:myvideo.o img_scanner.o error.o decoder.o qr_finder.o  \
            symbol.o scanner.o image.o refcnt.o convert.o \
	    qrdec.o isaac.o rs.o bch15_5.o util.o binarize.o qrdectxt.o  \
            video.o window.o v4l2.o v4l.o myvideo.o processor.o 
	${CC} ${CFLAG}  myzbar myvideo.o zbar/img_scanner.o zbar/error.o zbar/decoder.o \
	zbar/decoder/qr_finder.o zbar/convert.o \
	zbar/symbol.o zbar/scanner.o zbar/image.o zbar/refcnt.o \
	zbar/video.o zbar/window.o   \
	zbar/qrcode/qrdec.o zbar/qrcode/isaac.o zbar/qrcode/rs.o  zbar/qrcode/bch15_5.o zbar/qrcode/util.o zbar/qrcode/binarize.o zbar/qrcode/qrdectxt.o zbar/video/v4l.o zbar/video/v4l2.o zbar/processor.o -lpthread -static
#-------------------zbar
#-------covert video window
video.o: zbar/video.c 
	${CC} -c ${CFLAG} zbar/video.o zbar/video.c -I include
window.o:zbar/window.c
	${CC} -c ${CFLAG} zbar/window.o zbar/window.c -I include
processor.o:zbar/processor.c zbar/processor.h
	${CC} -c ${CFLAG} zbar/processor.o zbar/processor.c -I include
	
convert.o: zbar/convert.c zbar/image.h zbar/video.h zbar/window.h
	${CC} -c ${CFLAG} zbar/convert.o zbar/convert.c -I include
error.o:zbar/error.c include/config.h zbar/error.h
	${CC} -c ${CFLAG} zbar/error.o zbar/error.c -I ./include

img_scanner.o:zbar/img_scanner.c include/zbar.h include/config.h zbar/error.h \
              zbar/img_scanner.h zbar/svg.h 
	${CC} -c ${CFLAG} zbar/img_scanner.o zbar/img_scanner.c	-I ./include -I zbar
image.o:zbar/image.c zbar/error.h zbar/image.h zbar/refcnt.h include/config.h
	${CC} -c ${CFLAG} zbar/image.o zbar/image.c -I ./include
refcnt.o: zbar/refcnt.c zbar/refcnt.h
	${CC} -c ${CFLAG}  zbar/refcnt.o zbar/refcnt.c -I include 
symbol.o:zbar/symbol.c include/config.h include/zbar.h zbar/symbol.h
	${CC} -c ${CFLAG} zbar/symbol.o zbar/symbol.c -I include
scanner.o:zbar/scanner.c include/config.h include/zbar.h zbar/svg.h zbar/debug.h
	${CC} -c ${CFLAG} zbar/scanner.o zbar/scanner.c -I include
decoder.o: zbar/decoder.c zbar/decoder.h  include/zbar.h include/config.h  zbar/decoder/qr_finder.h 
	${CC} -c ${CFLAG} zbar/decoder.o zbar/decoder.c  -I ./include -I zbar/decoder -I ./zbar
v4l2.o: zbar/video/v4l2.c   
	${CC} -c ${CFLAG} zbar/video/v4l2.o zbar/video/v4l2.c  -I ./zbar -I include

v4l.o: zbar/video/v4l.c   
	${CC} -c ${CFLAG} zbar/video/v4l.o zbar/video/v4l.c  -I ./zbar -I include
#--------------decoder
qr_finder.o:zbar/decoder/qr_finder.c include/config.h include/zbar.h zbar/decoder.h zbar/debug.h
	${CC} -c ${CFLAG} zbar/decoder/qr_finder.o zbar/decoder/qr_finder.c -I ./include -I zbar

#--------------qrcode
qrdec.o:zbar/qrcode/qrdec.c zbar/qrcode.h zbar/qrcode/qrdec.h 
	${CC} -c ${CFLAG} zbar/qrcode/qrdec.o zbar/qrcode/qrdec.c -I include -I zbar
isaac.o:zbar/qrcode/isaac.c zbar/qrcode/isaac.h
	${CC} -c ${CFLAG} zbar/qrcode/isaac.o zbar/qrcode/isaac.c
rs.o:zbar/qrcode/rs.c zbar/qrcode/rs.h
	${CC} -c ${CFLAG} zbar/qrcode/rs.o zbar/qrcode/rs.c
bch15_5.o:zbar/qrcode/bch15_5.c zbar/qrcode/bch15_5.h
	${CC} -c ${CFLAG} zbar/qrcode/bch15_5.o zbar/qrcode/bch15_5.c
util.o: zbar/qrcode/util.c zbar/qrcode/util.o
	${CC} -c ${CFLAG} zbar/qrcode/util.o zbar/qrcode/util.c
binarize.o:zbar/qrcode/binarize.c zbar/qrcode/binarize.h
	${CC} -c ${CFLAG} zbar/qrcode/binarize.o zbar/qrcode/binarize.c -I zbar -I include
qrdectxt.o:zbar/qrcode/qrdectxt.c 
	${CC} -c ${CFLAG} zbar/qrcode/qrdectxt.o zbar/qrcode/qrdectxt.c -I zbar	-I include
myvideo.o:test_video.c include/zbar.h
	${CC} -c ${CFLAG} myvideo.o test_video.c -I ./include -I zbar
clean:
	rm ./*.o zbar/*.o
