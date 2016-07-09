@ iPAQ h2210 first stage bootloader
@ Reads from NAND flash, then boots device
@ If power button is held, enters emergency debrick mode
@ Handles suspend/resume
@
@ Copyright (c) 2005 Joshua Wise. Released under the GNU GPL.
@ Portions copyright (c) 2005 Andrew Zabolotny.

#define dbg(str) \
		adr	r0, 98f ; \
		adr	lr, 99f ; \
		b	out_str ; \
98:		.asciz	str ; \
		.align	2 ; \
99:

#define dbghex(reg) \
.ifnc  reg,r0 ; \
		mov	r0, reg ; \
.endif ; \
		bl      out_hex_word

#define dbgchr(chr) \
		mov	r0, # ## chr; \
		bl      out_char

#define GPIO_bit(x)     (1 << ((x) & 0x1f))
#define MDREFR_K2FREE	(1 << 25)
#define MDREFR_SLFRSH	(1 << 22)
#define MDREFR_K1RUN	(1 << 16)
#define MDREFR_K2RUN	(1 << 18)
#define MDREFR_E1PIN	(1 << 15)
#define MDCNFG_DE0	(1 << 0)
#define MDCNFG_DE1	(1 << 1)
#define MDCNFG_DE2	(1 << 16)
#define MDCNFG_DE3	(1 << 17)

#define NAND_CMD_READA  0x00
#define NAND_CMD_READB  0x01
#define NAND_CMD_READC  0x50
#define NAND_CMD_STATUS 0x70
#define NAND_CMD_READID 0x90
#define NAND_CMD_RESET  0xFF
#define NAND_PAGE_SIZE  512
#define NAND_OOB_SIZE   16
#define NAND_BLOCK_SIZE 32 /* In pages */

.global _start
	b	_start
	nop		@b	UNDF
	nop		@b	SWI
	nop 		@b	PABT
	b	DABT
	nop
	nop		@b	IRQ
	nop		@b	FIQ
DABT:	mov	r12, lr
	dbghex(r1)
	dbghex(r11)
	dbghex(r12)
	dbg("Data abort\r\n")
1:	b 1b
_start:
	nop
	bl	init_gpios
	bl	init_clocks
	bl	init_uart
	
	mov	sp, #0x0
	dbg("\r\niPAQ h2210 first stage bootloader\r\n")
	
	dbg("Initializing system ")
	bl	init_sdram
	bl	init_relocate
	bl	init_hamcop
	dbg("[done]\r\n")
	mov	sp, #0x1

	ldr	r0, GPIO_BASE
	ldr	r7, [r0, #0x00]
	tst	r7, #(1<<13)
#ifndef NO_HANDS
	beq	ymodemdownload
#else /* NO_HANDS is defined */
	bne	ymodemdownload
#endif

	dbg("Booting system normally... ")
	mov	r11, #0x5000			@ NAND base
	mov	r0, #(1 << 12) | (2 << 8)
	orr	r0, r0, #(1 << 1) | (0 << 3)
	strh	r0, [r11, #0x0]			@ NFCONF0
	mov	r0, #0x0003			@ NAND FLASH mode
	strh	r0, [r11, #0x08]		@ NFCONT0
	
	dbg("[NAND ")
	mov	r0, #NAND_CMD_RESET		@ NAND reset
	strh	r0, [r11, #0x10]		@ NFCMMD
	bl	wait_nandok
	
	mov	r0, #NAND_CMD_READID
	strh	r0, [r11, #0x10]		@ NFCMMD
	strh	r0, [r11, #0x14]		@ NFADDR0
	mov	r0, r6
	bl	wait_nandok
	ldrh	r0, [r11, #0x1C]		@ NFDATA
	and	r0, r0, #0xFF
	dbghex(r0)
	ldrh	r0, [r11, #0x1C]		@ NFDATA
	and	r0, r0, #0xFF
	dbghex(r0)
	dbg("] ")
	
	mov	r12,      #0xA0000000	@ bootloader current destination
	orr	r12, r12, #0x00008000
	mov	r13, #((512*1024)/NAND_PAGE_SIZE) @ remaining pages
	mov	r6, #NAND_BLOCK_SIZE	@ current page

nand_loop:	
	@ Check for bad blocks if necessary.
	tst	r6, #31
	beq	2f
	
	mov	r0, #NAND_CMD_READC
	strh	r0, [r11, #0x10]	@ NFCMMD
	mov	r0, #0x5
	strh	r0, [r11, #0x14]	@ NFADDR0
	and	r0, r6, #0xff
	strh	r0, [r11, #0x14]	@ NFADDR0
	mov	r0, r6, LSR#8
	and	r0, r0, #0xff
	strh	r0, [r11, #0x14]	@ NFADDR0
	bl	wait_nandok
	ldrh	r1, [r11, #0x1C]	@ NFDATA
	
	mov	r0, #NAND_CMD_READC
	strh	r0, [r11, #0x10]	@ NFCMMD
	mov	r0, #0x5
	strh	r0, [r11, #0x14]	@ NFADDR0
	add	r0, r6, #1
	and	r0, r0, #0xff
	strh	r0, [r11, #0x14]	@ NFADDR0
	mov	r0, r6, LSR#8
	and	r0, r0, #0xff
	strh	r0, [r11, #0x14]	@ NFADDR0
	bl	wait_nandok
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	
	and	r1, r1, r7
	teq	r1, #0xff
	beq	2f
	
	dbg("[bad block - skipping] ")
	add	r6, r6, #NAND_BLOCK_SIZE
	b	nand_loop
2:
	
	@dbgchr('\\')
	@ First, whack the ECC generator.
	ldrh	r0, [r11, #0x08]	@ NFCONT0
	orr	r0, r0, #(1 << 11)	@ InitECC
	strh	r0, [r11, #0x08]	@ NFCONT0

	@dbgchr('|')
	@ Now read the page in.
	mov	r0, #NAND_CMD_READA
	strh	r0, [r11, #0x10]	@ NFCMMD
	mov	r0, #0x00
	strh	r0, [r11, #0x14]	@ NFADDR0
	and	r0, r6, #0xFF
	strh	r0, [r11, #0x14]	@ NFADDR0
	mov	r0, r6, LSR#8
	and	r0, r0, #0xFF
	strh	r0, [r11, #0x14]	@ NFADDR0
	bl	wait_nandok

	mov	r1, #512
	mov	r2, #0xA0000000
	orr	r2, r2, #0x00002000
1:	ldrh	r0, [r11, #0x1C]	@ NFDATA
	and	r0, r0, #0xFF
	ldrh	r3, [r11, #0x1C]	@ NFDATA
	and	r3, r3, #0xFF
	orr	r0, r0, r3, LSL#8
	ldrh	r3, [r11, #0x1C]	@ NFDATA
	and	r3, r3, #0xFF
	orr	r0, r0, r3, LSL#16
	ldrh	r3, [r11, #0x1C]	@ NFDATA
	and	r3, r3, #0xFF
	orr	r0, r0, r3, LSL#24
	str	r0, [r2], #4
	subs	r1, r1, #4
	bne	1b
	@dbgchr('/')
	
	@ Read in calculated ECC.
	ldrh	r1, [r11, #0x20]	@ NFECCL0
	ldrh	r0, [r11, #0x24]	@ NFECCL1
	orr	r1, r1, r0,LSL#16
	and	r1, r1, #0xffffff
	
	@ Read in three bytes of ECC at offsets 6-8.
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	ldrh	r7, [r11, #0x1C]	@ NFDATA
	and	r7, r7, #0xFF
	ldrh	r0, [r11, #0x1C]	@ NFDATA
	and	r0, r0, #0xFF
	orr	r7, r7, r0, lsl #8
	ldrh	r0, [r11, #0x1C]	@ NFDATA
	and	r0, r0, #0xFF
	orr	r7, r7, r0, lsl #16	@ Put the bits rogether
	eors	r0, r1, r7		@ Do the compare
	beq	10f			@ If the data is clean, leapfrog the error
	dbgchr('E')
10:	mov	r1, #512
	mov	r2, #0xA0000000
	orr	r2, r2, #0x00002000
1:	ldr	r0, [r2], #4
	str	r0, [r12], #4
	subs	r1, r1, #4
	bne	1b
	
	dbgchr('.')
	add	r6, r6, #1
	subs	r13, r13, #1
	bne	nand_loop

	dbg("done!\r\nFinal address:")
	
	dbghex(r12)

	mov	r11,     #0xA0000000
	orr	r11, r11, #0x00008000
	
	dbg("\r\nFirst word of binary: ")
	ldr	r0, [r11]
	dbghex(r0)
	dbg("\r\n")
	
	mov	pc, r11

@ assumes that 0x5000 is in r11
wait_nandok:
	ldrh	r0, [r11, #0x34]	@ NFSTAT1
	tst	r0, #0x1		@ flash_rnb
	beq	wait_nandok
	mov	pc,lr
	
	
@ init_gpios
@ brings GPIOs to life using predefined values
init_gpios:
	ldr	r0, GPIO_BASE
	ldr	r1, GPSR0
	str	r1, [r0, #0x18]	@GPSR0
	mov	r2, #0xffffffff
	eor	r1, r1, r2
	str	r1, [r0, #0x24]	@GPCR0
	ldr	r1, GPSR1
	str	r1, [r0, #0x1C]	@GPSR1
	eor	r1, r1, r2
	str	r1, [r0, #0x28]	@GPCR1
	ldr	r1, GPSR2
	str	r1, [r0, #0x20]	@GPSR2
	eor	r1, r1, r2
	str	r1, [r0, #0x2c]	@GPCR2
	ldr	r1, GPDR0
	str	r1, [r0, #0x0c]	@GPDR0
	ldr	r1, GPDR1
	str	r1, [r0, #0x10]	@GPDR1
	ldr	r1, GPDR2
	str	r1, [r0, #0x14]	@GPDR2
	ldr	r1, GAFR0
	str	r1, [r0, #0x54]	@GAFR0
	ldr	r1, GAFR1
	str	r1, [r0, #0x58]	@GAFR1
	ldr	r1, GAFR2
	str	r1, [r0, #0x5C]	@GAFR2
	ldr	r1, GAFR3
	str	r1, [r0, #0x60]	@GAFR3
	ldr	r1, GAFR4
	str	r1, [r0, #0x64]	@GAFR4
	ldr	r1, GAFR5
	str	r1, [r0, #0x68]	@GAFR5
	
	@ enable gpios
	ldr	r0, PWER_BASE
	mov	r1, #0x30	@RDH|PH
	str	r1, [r0, #0x04]	@PSSR
	
	mov	pc,lr
	
@ init_clocks
@ brings clocks to life using predefined values
init_clocks:
	mov	r10, lr
	ldr	r1, CLOCK_BASE
	ldr	r0, CCCR_INIT
	str	r0, [r1]		@ CCCR
	ldr	r0, CKEN_INIT
	str	r0, [r1, #0x04]		@ CKEN
	ldr	r0, OSCC_INIT
	str	r0, [r1, #0x8]		@ CCCR

	bl	clockstab	@ wait for clocks, transceiver
	
	mov	pc, r10
	
init_uart:
	ldr	r1, UART_BASE
	mov	r0, #0		@ No interrupts (0x00)
	str	r0, [r1, #0x4]	@ --- FFIER
	mov	r0, #0x1	@ Enable FIFO (0x01)
	str	r0, [r1, #0x8]	@ --- FFFCR
	mov	r0, #0x7	@ Clear buffers (0x07)
	str	r0, [r1, #0x8]	@ --- FFFCR
	ldr	r0, [r1, #0x14]	@ --- FFLSR: Clear interrupts
	ldr	r0, [r1, #0x8]	@ --- FFIIR
	ldr	r0, [r1, #0x18]	@ --- FFMSR

	mov	r0, #0x3	@ LCR_WLEN8 (0x03)
	str	r0, [r1, #0xC]	@ --- FFLCR
	
	mov	r0, #0x01	@ DTR only (0x01)
	str	r0, [r1, #0x10]	@ -- FFMCR
	
	mov	r0, #0x83	@ Access divisor latch (0x83)
	str	r0, [r1, #0xC]	@ --- FFLCR
	mov	r0, #0x8	@ Set divisor (0x08)
	str	r0, [r1, #0x0]	@ --- FFDLL
	mov	r0, #0x0	@ Set divisor (0x00)
	str	r0, [r1, #0x4]	@ --- FFDLH
	mov	r0, #0x3	@ Deaccess div latch
	str	r0, [r1, #0xC]	@ --- FFLCR
	mov	r0, #0x1	@ UART_FCR_ENABLE_FIFO (0x01)
	str	r0, [r1, #0x8]	@ -- FFFCR
	
	mov	r0, #0x40	@ IER_UUE (0x40)
	str	r0,[r1, #0x4]	@ --- FFIER
	
	mov	pc, lr

init_sdram:
	mov	r10,lr
	dbg("[SDRAM ")
	
	@ Wait 200uS for internal clocks to stabilize
	bl	clockstab

	ldr	r0, MEM_BASE
	ldr	r1, MDREFR
	orr	r1, r1, #MDREFR_SLFRSH
	bic	r1, r1, #(MDREFR_K1RUN|MDREFR_K2RUN|MDREFR_E1PIN)
	bic	r1, r1, #MDREFR_K2FREE
	str	r1, [r0, #0x4]	@ MDREFR
	
	mov	r1, #4
	str	r1, [r0, #0x1C]	@ SXCNFG
	
	ldr	r1, MDREFR
	str	r1, [r0,#0x4]	@ MDREFR
	
	ldr	r1, MDCNFG
	bic	r1, r1, #(MDCNFG_DE0|MDCNFG_DE1)
	bic	r1, r1, #(MDCNFG_DE2|MDCNFG_DE3)
	str	r1, [r0]	@ MDCNFG
	
	bl	clockstab
	
	@ start refreshing
	mov	r0, #0xA0000000
	mov	r2, #0x800
1:	ldr	r1, [r0]
	subs	r2, r2, #1
	bne	1b
	
	ldr	r0, MEM_BASE
	ldr	r1, MDCNFG
	str	r1, [r0]	@MDCNFG
	
	mov	r1, #0
	str	r1, [r0,#0x40]	@MDMRS
	
	mov	r0, #0xA0000000
	mov	r2, #0x800
1:	ldr	r1, [r0]
	subs	r2, r2, #1
	bne	1b
	
	dbg("OK] ")
	
	mov	pc, r10
	
init_relocate:
	mov	r10,lr
	dbg("[relocate ")
	mov	r0, #0x0
	mov	r1, #0xA0000000
	orr	r1, r1, #0x00001000
	mov	r2, #0x1000
1:	ldr	r3, [r0], #4
	str	r3, [r1], #4
	subs	r2, r2, #4
	bne	1b
	
	mov	r8, #0xA0000000
	orr	r8, r8, #0x00001000
	add	pc, pc, r8
	nop
	dbg("PC: ");
	dbghex(pc);

	dbg(" OK] ")

	add	pc, r10, r8

init_hamcop:
	mov	r10, lr
	dbg("[HAMCOP ")
	@ setup MSC0
	mov	r0, #0x48000000

	ldr	r1, MSC0
	str	r1, [r0, #0x08]
	ldr	r1, [r0, #0x08]	@ read MSC0

	ldr	r1, MSC1
	str	r1, [r0, #0x0C]
	ldr	r1, [r0, #0x0C]	@ read MSC1

	ldr	r1, MSC2
	str	r1, [r0, #0x10]
	ldr	r1, [r0, #0x10]	@ read MSC2

	@ enter VLIO mode
	mov	r0, #0x4000
	mov	r1, #0x1
	strh	r1, [r0]

	@ set CLKSLEEP
	mov	r0, #0x4C00
	ldr	r1, CLKSLEEP
	strh	r1, [r0, #0x0C]

	@ disable watchdog timer
	mov	r0, #0x6000
	mov	r1, #0x0000
	strh	r1, [r0]

	@ initialize HAMCOP GPIOs to sane values
	mov	r0, #0x7000
	ldr	r1, GPACON
	strh	r1, [r0, #0x00]
	ldr	r1, GPADAT
	strh	r1, [r0, #0x04]
	ldr	r1, GPAUP
	strh	r1, [r0, #0x08]
	ldr	r1, GPBCON
	strh	r1, [r0, #0x10]
	ldr	r1, GPBDAT
	strh	r1, [r0, #0x14]
	ldr	r1, GPBUP
	strh	r1, [r0, #0x18]
	ldr	r1, GPCCON
	strh	r1, [r0, #0x20]
	ldr	r1, GPCDAT
	strh	r1, [r0, #0x24]
	ldr	r1, GPCUP
	strh	r1, [r0, #0x28]
	ldr	r1, GPDCON
	strh	r1, [r0, #0x30]
	ldr	r1, GPDDAT
	strh	r1, [r0, #0x34]
	ldr	r1, GPDUP
	strh	r1, [r0, #0x38]
	
	@ show a green LED
	mov	r0, #0x7200
	mov	r1, #0x3	@enable
	str	r1, [r0, #0x08]	@green led, why not
	mov	r1, #0x1
	str	r1, [r0, #0x00]
	str	r1, [r0, #0x10]
	dbg("OK] ")
	
	mov	pc,r10

out_char:
	ldr	r1, UART_BASE
1:	ldr     r2, [r1, #0x14]	@ FFLSR - Get UART status
	tst     r2, #0x40	@ LSR_TEMT - Check TX empty
	beq     1b
	str	r0, [r1, #0x0]	@ FFTHR
	ldr     r0, [r1, #0x14]	@ FFLSR
	mov	pc, lr
		
in_char:
	ldr	r1, UART_BASE
	ldr	r3, OSCR_BASE
	mov	r4, #0
	str	r4, [r3, #0x10]
	ldr	r4, waittime
1:	ldr	r2, [r1, #0x14]	@ FFLSR
	tst	r2, #0x1	@ data ready?
	bne	2f		@ done, then receive
	@now check to see if we've timed out
	ldr	r2, [r3, #0x10]	@ OSCR
	cmp	r2, r4
	movgt	pc, lr		@ if so, bail out with gt set
	b	1b
2:	ldr	r0, [r1, #0x0]	@ FFRBR - receive
	tst	r2, #0x82	@ overrun? return ne if overrun, eq if no overrun
	mov	pc, lr
	
	
ymodemdownload:
	dbg("Emergency recover mode entered.\r\nWaiting for YMODEM download\r\n");
#define SOH (0x01)
#define STX (0x02)
#define EOT (0x04)
#define ACK (0x06)
#define NAK (0x15)
#define CAN (0x18)
#define CRC (0x43)
#define GRC (0x47)
@ r7:  packet size
@ r8:  waitpackets remaining
@ r9:  blocks received
@ r10: receive buffer
@ r11: received size
	mov	r10,      #0xA0000000
	orr	r10, r10, #0x00008000
	mov	r9, #0x0
	mov	r11, #0x0
	mov	r8, #0x10
ymodemloop:
	cmp	r8, #0x0
	beq	ymodem_timeout

	cmp	r9, #0x0
	moveq	r0, #'C'
	bleq	out_char
	
	bl	in_char
	subgt	r8, r8, #1
	bgt	ymodemloop
	bne	ymodem_recverror
	
	adr	lr, ymodemloop
	cmp	r0, #SOH
	moveq	r7, #128
	beq	ymodem_getpacket
	
	cmp	r0, #STX
	moveq	r7, #1024
	beq	ymodem_getpacket
	
	cmp	r0, #EOT
	beq	ymodem_eot
	
	cmp	r0, #CAN
	beq	ymodem_can
	
	mov	r0, #NAK
	bl	out_char
	
	b	ymodemloop

ymodem_getpacket:
	bl	in_char	@minimal error checking
	bl	in_char	@minimal being zero in our case
	
1:	cmp	r7, #0
	beq	2f
	bl	in_char
	mov	r5,     r0
	bl	in_char
	orr	r5, r5, r0,LSL#8
	bl	in_char
	orr	r5, r5, r0,LSL#16
	bl	in_char
	orr	r5, r5, r0,LSL#24
	sub	r7, r7, #4
	cmp	r9, #0	@first block? store nothing
	beq	1b
	add	r11, r11, #4
	str	r5, [r10], #4
	b	1b
	
2:	bl	in_char	@throw away two bytes
	bl	in_char
	
	mov	r0, #ACK
	bl	out_char
	cmp	r9,#0
	moveq	r0, #CRC
	bleq	out_char
	
	add	r9, r9, #1
	
	b	ymodemloop

ymodem_eot:
	mov	r0,#ACK
	bl	out_char
	bl	waitonesec
	mov	r0,#CRC
	bl	out_char
	bl	waitonesec
	mov	r0,#ACK
	bl	out_char
	bl	waitonesec
	mov	r0,#CAN
	bl	out_char
	mov	r0,#CAN
	bl	out_char
	mov	r0,#CAN
	bl	out_char
	mov	r0,#CAN
	bl	out_char
1:	bl	in_char
	bgt	2f
	b	1b
2:	dbg("***YMODEM receive complete\r\n");
	dbg("***Received ");
	dbghex(r11)
	dbg(" bytes\r\n");
	mov	r8,   #0xA0000000
	orr	r8,r8,#0x00008000
	dbg("***First word of data: ");
	ldr	r1,[r8]
	dbghex(r1)
	dbg("\r\n***Jumping to received code\r\n");
	mov	pc,r8

ymodem_can:
	mov	r0,#CAN
	bl	out_char
	mov	r0,#CAN
	bl	out_char
	mov	r0,#CAN
	bl	out_char
	mov	r0,#CAN
	bl	out_char
	bl	waitonesec
	dbg("***YMODEM receive cancelled - reboot device to restore sanity\r\n")
	b	errcommon

ymodem_timeout:
	dbg("***YMODEM receive timed out - reboot device to restore sanity\r\n")
	b	errcommon

ymodem_recverror:
	dbg("***YMODEM receive error - reboot device to restore sanity\r\n")
	b	errcommon

errcommon:
	dbg("***Some error occurred. Reboot device to restore sanity.\r\n");
1:	b	1b
	

	
waitonesec:
	ldr	r3,OSCR_BASE
	mov	r4, #0
	str	r4, [r3, #0x10]
	ldr	r4, waittime
1:	ldr	r2, [r3, #0x10]	@ OSCR
	cmp	r4, r2
	movgt	pc, lr		@ if so, bail out with gt set
	b	1b
	
waittime:
	.word	0x00483A88	@ Close enough

clockstab:
	ldr	r0, OSCR_BASE
	mov	r1, #0
	str	r1, [r0, #0x10]	@ OSCR
	mov	r3, #0x300
1:	ldr	r2, [r0, #0x10]	@ OSCR
	cmp	r3, r2
	bgt	1b
	mov	pc,lr

out_str:	tst	sp, #0x1
		beq	1f
		ldr	r1, OUTPUT_STORAGE
		stmia	r1, {r2-r12}
1:		mov	r4, lr
		mov	r5, r0
1:		ldrb	r0, [r5], #1
		tst	r0, r0
		beq	2f
		adr	lr, 1b
		b	out_char
2:		mov	r0,r4
		tst	sp, #0x1
		moveq	pc,r0
		ldr	r1, OUTPUT_STORAGE
		ldmia	r1, {r2-r12}
		mov	pc,r0

@destroy r0, r1, r2, r4, r5, r6, r7, lr (woohoo!!!)
out_hex_word:	mov     r6, lr
		mov	r7, r0
		mov	r0, r0, lsr #24
		bl      out_hex_byte
		mov	r0, r7, lsr #16
		bl      out_hex_byte
		mov	r0, r7, lsr #8
		bl      out_hex_byte
		mov	r0, r7
		mov	lr, r6
out_hex_byte:	mov	r4, lr
		mov	r5, r0
		mov	r0, r0, lsr #4
		bl	out_hex_nibble
		mov	lr, r4
		mov	r0, r5
out_hex_nibble:	and	r0, r0, #0xf
		add	r0, r0, #'0'
		cmp	r0, #'9'
		addgt	r0, r0, #7
		b	out_char

OUTPUT_STORAGE:
	.long	0xA1000000	
UART_BASE:
	.long 	0x40100000
GPIO_BASE:
	.long	0x40E00000
PWER_BASE:
	.long	0x40F00000
CLOCK_BASE:
	.long	0x41300000
MEM_BASE:
	.long	0x48000000
OSCR_BASE:
	.long	0x40A00000

#define CKEN6_FFUART	(1 << 6)
#define CKEN16_LCD	(1 << 16)
#define CKEN5_STUART	(1 << 5)
#define OSCC_OON	(1 << 1)

#define GPIO_BIT_H2200_RS232_CIR_N (1 << 24)
#define GPIO_BIT_H2200_RS232_ON    (1 << 16)

GPDR0:	.long	0xf7e38c00
GPDR1:	.long	0xbcffbf83
GPDR2:	.long	0x000157ff
GAFR0:	.long	0x80411000
GAFR1:	.long	0x00000112
GAFR2:	.long	0x600a9550
GAFR3:	.long	0x0005aaaa
GAFR4:	.long	0x20000000
GAFR5:	.long	0x00000000
GPSR0:	.long	0x05438000 | GPIO_BIT_H2200_RS232_CIR_N
GPSR1:	.long	0xa0ff0380
GPSR2:	.long	0x0000571c | GPIO_BIT_H2200_RS232_ON
MSC0:	.long	0x246c44cc
MSC1:	.long	0x7ff07ff0
MSC2:	.long	0x7ff07ff0
MDREFR:	.long	0x0099E018
MDCNFG:	.long	0x01C801CB
CCCR_INIT:	.long	((4<<7)|(2<<5)|(1))
CKEN_INIT:	.long   (CKEN16_LCD | CKEN6_FFUART | CKEN5_STUART)
OSCC_INIT:	.long   (OSCC_OON)

GPACON:		.long	0x82AA
GPADAT:		.long	0x00FF
GPAUP:		.long	0x00FF
GPBCON:		.long	0x1500
GPBDAT:		.long	0x000F
GPBUP:		.long	0x00FF
GPCCON:		.long	0x1000
GPCDAT:		.long	0x0064
GPCUP:		.long	0x00FF
GPDCON:		.long	0x556A
GPDDAT:		.long	0x0000
GPDUP:		.long	0x00FF
CLKOUTCON:	.long	0x0054
SPUCR:		.long	0x001B
CLKSLEEP:	.long	0x0088
