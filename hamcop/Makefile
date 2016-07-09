CFLAGS ?= -DWIGGLER -DNO_TIMING
CFLAGS += -Wall -O2 -I.. -I.
#ASFLAGS ?= -DNO_HANDS

HAMCOPOBJS = jtag_core.o nand.o libjtag.o hamcop.o
INSERTECCOBJS = insertecc.o
MODULES =  $(subst .c,.o,$(wildcard modules/*.c))

all: hamcop debrick.bin insertecc first-stage.bin

libjtag.c: libjtag.h

jtag_core.o: libjtag.c

modules: $(MODULES)

hamcop: includes.h modules $(HAMCOPOBJS)
	gcc $(CFLAGS) -o $@ $(HAMCOPOBJS) $(MODULES)

insertecc: $(INSERTECCOBJS)
	gcc $(CFLAGS) -o $@ $(INSERTECCOBJS)

includes.h: $(wildcard modules/*.h)
	echo "#ifndef __includes_h__" > includes.h
	echo "#define __includes_h__" >> includes.h
	echo "" >> includes.h
	for i in `ls modules/*.h`; do echo '#include "'$$i'"' >> includes.h; done
	echo "" >> includes.h
	echo "#endif" >> includes.h

debrick.o: debrick.S
	arm-linux-gcc $(ASFLAGS) -c -o debrick.o debrick.S 

debrick.elf: debrick.o
	arm-linux-ld -Ttext=0x00000000 -o debrick.elf debrick.o

debrick.bin: debrick.elf
	arm-linux-objcopy -Obinary debrick.elf debrick.bin
	
first-stage.o: first-stage.S
	arm-linux-gcc $(ASFLAGS) -c -o first-stage.o first-stage.S 

first-stage.elf: first-stage.o
	arm-linux-ld -Ttext=0x00000000 -o first-stage.elf first-stage.o

first-stage.bin: first-stage.elf
	arm-linux-objcopy -Obinary first-stage.elf first-stage.bin

debrick: first-stage.bin insertecc hamcop pwrctrl
	./pwrctrl 0 && sleep 0.25 && ./pwrctrl 1 && sleep 0.3
	./hamcop -b
	./insertecc < first-stage.bin | ./hamcop -d /dev/stdin
	./pwrctrl 0 && sleep 2 && ./pwrctrl 1 && sleep 0.3

clean:
	rm -rf *.o hamcop includes.h modules/*.o debrick.bin debrick.elf insertecc
