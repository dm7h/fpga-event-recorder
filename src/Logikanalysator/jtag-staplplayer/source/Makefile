
OBJS =  jamarray.o jamcomp.o jamcrc.o jamexec.o jamexp.o jamheap.o jamjtag.o jamnote.o jamstack.o jamstub.o jamsym.o jamutil.o

CFLAGS = -O2 -g
LUSB= -lusb
all: jamplayer

clean:
	rm -rf core $(OBJS) jamplayer

jamplayer: $(OBJS)	
	gcc $(CFLAGS) $(OBJS) $(LUSB) -o jamplayer
 
jamarray.o: jamarray.c jamarray.h
	gcc $(CFLAGS) -c jamarray.c -o jamarray.o

jamcomp.o: jamcomp.c jamcomp.h
	gcc $(CFLAGS) -c jamcomp.c -o jamcomp.o

jamcrc.o: jamcrc.c
	gcc $(CFLAGS) -c jamcrc.c -o jamcrc.o

jamexec.o: jamexec.c jamexec.h
	gcc $(CFLAGS) -c jamexec.c -o jamexec.o

jamexp.o: jamexp.c jamexp.h
	gcc $(CFLAGS) -c jamexp.c -o jamexp.o

jamheap.o: jamheap.c jamheap.h
	gcc $(CFLAGS) -c jamheap.c -o jamheap.o

jamjtag.o: jamjtag.c jamjtag.h
	gcc $(CFLAGS) -c jamjtag.c -o jamjtag.o

jamnote.o: jamnote.c 
	gcc $(CFLAGS) -c jamnote.c -o jamnote.o

jamstack.o: jamstack.c jamstack.h
	gcc $(CFLAGS) -c jamstack.c -o jamstack.o

jamstub.o: jamstub.c 
	gcc $(CFLAGS) -c jamstub.c -o jamstub.o

jamsym.o: jamsym.c jamsym.h
	gcc $(CFLAGS) -c jamsym.c -o jamsym.o

jamutil.o: jamutil.c jamutil.h
	gcc $(CFLAGS) -c jamutil.c -o jamutil.o

