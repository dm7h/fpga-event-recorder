CC            = gcc
DEFINES       =
CFLAGS        = -O2 -std=gnu99 -Wall $(DEFINES)
INCPATH       = 
LINK          = gcc
LFLAGS        = 
LIBS          = -L./ -lftd2xx


setmode:  setmode.o 
	$(LINK) $(LFLAGS) -o setmode setmode.o  $(LIBS)

setmode.o: setmode.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o setmode.o setmode.c

recordSamples:  recordSamples.o 
	$(LINK) $(LFLAGS) -o recordSamples recordSamples.o  $(LIBS)

recordSamples.o: recordSamples.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o recordSamples.o recordSamples.c

loadbit:
	./fpgaprog -v -f ../synth/logic_sniffer.bit

flashbit:
	./fpgaprog.exe -v -f ../synth/logic_sniffer.bit -b bscan_spi_lx9_csg324.bit -sa -r

clean:
	rm -rf *.o