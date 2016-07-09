CFLAGS += -Wall -O2

BSRFLASHOBJS = jtag_core.o pxa_bus.o strataflash.o h1900flash.o bsrflash.o
BSRDEVMEMOBJS = jtag_core.o pxa_bus.o strataflash.o h1900flash.o bsrdevmem.o

all: bsrflash bsrdevmem

libjtag.c: libjtag.h

jtag_core.o: libjtag.c

bsrflash: $(BSRFLASHOBJS)
	gcc $(CFLAGS) -o $@ $(BSRFLASHOBJS)

bsrdevmem: $(BSRDEVMEMOBJS)
	gcc $(CFLAGS) -o $@ $(BSRDEVMEMOBJS)

clean:
	rm -rf *.o bsrflash bsrdevmem
