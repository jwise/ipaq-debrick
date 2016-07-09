
@ 39 is FFTXD - GAFR1_L{14:15} 0x2
@ 62 is bootloader-detect - set to 0
@ 4 is RS232_CIR - set to 0
@ 80 is RS232_ON - set to 1

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
#define MDREFR_SLFRSH	(1 << 22)
#define MDREFR_K1RUN	(1 << 16)
#define MDREFR_K2RUN	(1 << 18)
#define MDREFR_E1PIN	(1 << 15)
#define MDCNFG_DE0	(1 << 0)
#define MDCNFG_DE1	(1 << 1)
#define MDCNFG_DE2	(1 << 16)
#define MDCNFG_DE3	(1 << 17)

.global _start
_start:
	nop

	ldr	r1, CLOCK_BASE
	ldr	r0, CCCR_INIT
	str	r0, [r1]		@ CCCR
	ldr	r0, CKEN_INIT
	str	r0, [r1, #0x04]		@ CKEN
	ldr	r0, OSCC_INIT
	str	r0, [r1, #0x8]		@ CCCR
	
	ldr	r0, GPIO_BASE
	ldr	r1, GPSR0
	str	r1, [r0, #0x18]	@GPSR0
	ldr	r1, GPSR1
	str	r1, [r0, #0x1C]	@GPSR1
	ldr	r1, GPSR2
	str	r1, [r0, #0x20]	@GPSR2
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
	
	@enable gpios
	ldr	r0, PWER_BASE
	mov	r1, #0x30	@RDH|PH
	str	r1, [r0, #0x04]	@PSSR

	bl	clockstab	@ wait for clocks, transceiver

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

	dbg("\r\nEmergency flash recover mode\r\nInitializing system ");

	dbg("[SDRAM ");
	@ Wait 200uS for internal clocks to stabilize
	bl	clockstab

	ldr	r0, MEM_BASE
	ldr	r1, MDREFR
	orr	r1, r1, #MDREFR_SLFRSH
	bic	r1, r1, #(MDREFR_K1RUN|MDREFR_K2RUN|MDREFR_E1PIN)
	str	r1, [r0, #0x4]	@ MDREFR
	
	mov	r1, #0
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
	
	dbg("OK] ");

	dbg("[relocate ");

	mov	r0, #0x0
	mov	r1, #0xA0000000
	orr	r1, r1, #0x00001000
	mov	r2, #0x1000
1:	ldr	r3, [r0], #4
	str	r3, [r1], #4
	subs	r2, r2, #4
	bne	1b
	
	dbg("OK] ");
	
	dbg("[jump ");
	mov	r0, #0xA0000000
	orr	r0, r0, #0x00001000
	add	pc, pc, r0
	nop
	dbg("PC: ");
	dbghex(pc);
	dbg(" OK] ");

	dbg("[MSC0 ");
	@ setup MSC0
	mov	r0, #0x48000000
	ldr	r1, MSC0
	str	r1, [r0, #0x08]
	dbg("OK] ");
	
	dbg("[VLIO ");
	@ enter VLIO mode
	mov	r0, #0x4000
	mov	r1, #0x1
	strh	r1, [r0]
	dbg("OK] ");

	@ disable watchdog
	dbg("[watchdog ");
	mov	r0, #0x6000
	mov	r1, #0x0000
	strh	r1, [r0]
	dbg("OK] ");
	
	@ hamcop GPIOs
	dbg("[GPIO ");
	ldr	r1, UART_BASE	@ Just make sure real quick that we are done xmitting.
1:	ldr     r2, [r1, #0x14]	@ FFLSR - Get UART status
	tst     r2, #0x40	@ LSR_TEMT - Check TX empty
	beq     1b
	mov	r0, #0x7000
	ldr	r1, GPACON
	str	r1, [r0, #0x00]
	ldr	r1, GPADAT
	str	r1, [r0, #0x04]
	ldr	r1, GPAUP
	str	r1, [r0, #0x08]
	ldr	r1, GPBCON
	str	r1, [r0, #0x10]
	ldr	r1, GPBDAT
	str	r1, [r0, #0x14]
	ldr	r1, GPBUP
	str	r1, [r0, #0x18]
	ldr	r1, GPCCON
	str	r1, [r0, #0x20]
	ldr	r1, GPCDAT
	str	r1, [r0, #0x24]
	ldr	r1, GPCUP
	str	r1, [r0, #0x28]
	ldr	r1, GPDCON
	str	r1, [r0, #0x30]
	ldr	r1, GPDDAT
	str	r1, [r0, #0x34]
	ldr	r1, GPDUP
	str	r1, [r0, #0x38]
	mov	r0, #0x7200
	mov	r1, #0x2	@enable
	str	r1, [r0, #0x08]	@green led, why not
	mov	r1, #0x0
	str	r1, [r0, #0x00]
	str	r1, [r0, #0x10]
	ldr	r6, GPIO_BASE	@ turn off rs232 and reenable it
	mov	r7, #0x10000
	str	r7, [r6, #0x2C]	@GPCR2
	bl	clockstab	@ give plenty of time for it to come to life
	bl	clockstab
	bl	clockstab
	bl	clockstab
	str	r7, [r6, #0x20]	@GPSR2
	bl	clockstab
	bl	clockstab
	bl	clockstab
	bl	clockstab
	dbg("OK] ");

	b	ymodemdownload

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
	dbg("\r\nWaiting for YMODEM download\r\n");
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

out_str:	mov	r4, lr
		mov	r5, r0
1:		ldrb	r0, [r5], #1
		tst	r0, r0
		moveq	pc, r4
		adr	lr, 1b
		b	out_char

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
GPSR0:	.long	0x02428000 | GPIO_BIT_H2200_RS232_CIR_N
GPSR1:	.long	0x20ff0382
GPSR2:	.long	0x0001431c | GPIO_BIT_H2200_RS232_ON
MSC0:	.long	0x246c44cc
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
GPCCON:		.long	0x0000
GPCDAT:		.long	0x0064
GPCUP:		.long	0x00FF
GPDCON:		.long	0x556A
GPDDAT:		.long	0x0000
GPDUP:		.long	0x00FF
CLKOUTCON:	.long	0x0054
SPUCR:		.long	0x001B

