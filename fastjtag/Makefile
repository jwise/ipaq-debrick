CROSS_COMPILE ?= arm-linux-

ARMCC ?= $(CROSS_COMPILE)gcc
ARMLD ?= $(CROSS_COMPILE)ld
ARMOBJCOPY ?= $(CROSS_COMPILE)objcopy

TARGETS := h1900leds.bin e7xxleds.bin jtagrx.bin memclient.bin miniload rdevmem rflash rdump rterm icacheboot taptest fflash

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.nopad

%.bin: %.S
	@echo "Assembling $<..."
	-echo $< | sed -e 's/\.S//g' > .fname
	$(ARMCC) $(ASFLAGS) -c -o `cat .fname`.o `cat .fname`.S
	$(ARMLD) -o `cat .fname`.elf `cat .fname`.o
	$(ARMOBJCOPY) -O binary `cat .fname`.elf `cat .fname`.bin.nopad
	dd if=`cat .fname`.bin.nopad of=`cat .fname`.bin bs=2048 count=1 conv=sync
	rm -f `cat .fname`.o `cat .fname`.elf .fname

load: jtagrx.bin
	yes n | jflash jtagrx.bin P 2>/dev/null
