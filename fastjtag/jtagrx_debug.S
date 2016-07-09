@ Debug handler code
@ Ripped straight out of 278525-001.pdf
@ If you arent debugging, use jtagrx.S

#if !defined(H3900) && !defined(H1900) && !defined(E7XX)
#  warning You have not defined a supported architecture
#  warning Sorry, you have to do that
#endif

#if defined(H3900)
#  warning Compiling in H3900 mode
#endif

#if defined(H1900)
#  warning Compiling in H1900 mode
#endif

#if defined(E7XX)
#  warning Compiling in E7XX mode
#endif

.globl _start

_start:
	b	start
UndefEntryPoint:	
	b	start
SWIEntryPoint:	
	b	start
PrefetchAbortEntryPoint:	
	b	start
DataAbortEntryPoint:	
	b	start
NotUsedEntryPoint:      	
	b       start
IRQEntryPoint:	
	b	start
FIQEntryPoint:	
	b	start

start:
	@ At this point, we should enable the icache.
	mov	r0, #0x78	@ Must-be-on bits
	orr	r0, r0, #0x1000	@ icache
	mcr	p15, 0, r0, c1, c0, 0
	
	@ and cpwait for it
	mrc	p15, 0, r0, c2, c0, 0
	mov	r0, r0
	sub	pc, pc, #4

#ifdef H1900
	@ Initialize GPIOs
	mov	r0,     #0x40000000
	orr	r0, r0, #0x00E00000
	
	mov	r1,	#0xD3000000
	orr	r1, r1, #0x00D30000
	orr	r1, r1, #0x00009000
	str	r1, [r0, #0x0C] @ GPDR0
	mov	r1,	#0xFC000000
	orr	r1, r1, #0x00770000
	orr	r1, r1, #0x00008500
	orr	r1, r1, #0x000000FF
	str	r1, [r0, #0x10] @ GPDR1
	mov	r1,	#0x00020000
	sub	r1, r1, #0x00000001
	str	r1, [r0, #0x14] @ GPDR2

	mov	r1,	#0x80000000
	str	r1, [r0, #0x54] @ GAFR0_L

	mov	r1,	#0x5A000000
	orr	r1, r1, #0x001A0000
	orr	r1, r1, #0x00008000
	orr	r1, r1, #0x00000010
	str	r1, [r0, #0x58] @ GAFR0_U
	mov	r1,	#0x60000009
	str	r1, [r0, #0x5C] @ GAFR1_L
	mov	r1,	#0xAA000000
	orr	r1, r1, #0x00A10000
	orr	r1, r1, #0x00000008
	str	r1, [r0, #0x60] @ GAFR1_U
	mov	r1,	#0xAA000000
	orr	r1, r1, #0x008A0000
	orr	r1, r1, #0x0000AA00
	orr	r1, r1, #0x000000AA
	str	r1, [r0, #0x64] @ GAFR2_L
	mov	r1,	#0x00000002
	str	r1, [r0, #0x68]	@ GAFR2_U
	
	mov	r1,	#0x01000000
	orr	r1, r1, #0x00030000
	str	r1, [r0, #0x18]	@ GPSR0
	mov	r1,	#0x00420000
	orr	r1, r1, #0x00000100
	orr	r1, r1, #0x00000012
	str	r1, [r0, #0x1C]	@ GPSR1
	mov	r1,	#0x0001C000
	str	r1, [r0, #0x20]	@ GPSR2

	@ <pb_> have you done the little "gpio enable" dance that you have to go through at reset?
	@ * joshua_ picks up the 1900, punts it back and forth while hopping around
	@ <joshua_> like that?
	mov	r9,	#0x40000000
	orr	r9, r9, #0x00F00000
	mov	r1,	#0x30
	str	r1,	[r9, #0x4]

	@ Initialize MSCs
	mov	r0,	#0x48000000
	mov	r1, 	#0x7f000000	@ set to 7ff42358 in WinCE
	orr	r1, r1,	#0x00f40000
	orr	r1, r1, #0x00002300
	orr	r1, r1, #0x00000058
@	str	r1, [r0, #0x0C]		@ MSC1
@	ldr	r4, [r0, #0x0C]
#endif

#if defined(H3900)
	@ Initialize GPIOs
	mov	r0,     #0x40000000
	orr	r0, r0, #0x00E00000
	
	mov	r1,	#0xD3000000
	orr	r1, r1, #0x00E30000
	orr	r1, r1, #0x0000B900
	orr	r1, r1, #0x00000040
	str	r1, [r0, #0x0C] @ GPDR0
	mov	r1,	#0x7C000000
	orr	r1, r1, #0x00FF0000
	orr	r1, r1, #0x0000AB00
	orr	r1, r1, #0x00000083
	str	r1, [r0, #0x10] @ GPDR1
	mov	r1,	#0x0001E000
	orr	r1, r1, #0x00000BD0
	orr	r1, r1, #0x0000000F
	@str	r1, [r0, #0x14] @ GPDR2

	mov	r1,	#0x80000000
	orr	r1, r1, #0x00000001
	@str	r1, [r0, #0x54] @ GAFR0_L
	mov	r1,	#0x00000010
	@str	r1, [r0, #0x58] @ GAFR0_U
	
	mov	r1,	#0x99000000
	orr	r1, r1, #0x009A0000
	orr	r1, r1, #0x00008500
	orr	r1, r1, #0x00000058
	@str	r1, [r0, #0x5C] @ GAFR1_L
	mov	r1,	#0xAA000000
	orr	r1, r1, #0x00A50000
	orr	r1, r1, #0x0000AA00
	orr	r1, r1, #0x000000AA
	@str	r1, [r0, #0x60] @ GAFR1_U

	mov	r1,	#0xA0000000
	orr	r1, r1, #0x00000000
	orr	r1, r1, #0x00000800
	orr	r1, r1, #0x00000000
	@str	r1, [r0, #0x64] @ GAFR2_L
	mov	r1,	#0x00000002
	@str	r1, [r0, #0x68]	@ GAFR2_U

	@ <pb_> have you done the little "gpio enable" dance that you have to go through at reset?
	@ * joshua_ picks up the 1900, punts it back and forth while hopping around
	@ <joshua_> like that?
	@mov	r9,	#0x40000000
	@orr	r9, r9, #0x00F00000
	@mov	r1,	#0x30
	@str	r1,	[r9, #0x4]

	@ Initialize MSCs
	@ mov	r0,	#0x48000000
	@ mov	r1, 	#0x7f000000	@ set to 7ff42358 in WinCE
	@ orr	r1, r1,	#0x00f40000
	@ orr	r1, r1, #0x00002300
	@ orr	r1, r1, #0x00000058
	@ str	r1, [r0, #0x0C]		@ MSC1
	@ ldr	r4, [r0, #0x0C]
#endif
#if defined(E7XX)
	@ Initialize GPIOs
        mov     r0,     #0x40000000
        orr     r0, r0, #0x00E00000
                                                                                
        mov     r1,     #0xC9000000
        orr     r1, r1, #0x00990000
        orr     r1, r1, #0x00009000
        orr     r1, r1, #0x00000088
@        str     r1, [r0, #0x0C] @ GPDR0
        mov     r1,     #0xFC000000
        orr     r1, r1, #0x00FF0000
        orr     r1, r1, #0x0000AB00
        orr     r1, r1, #0x000000EB
@        str     r1, [r0, #0x10] @ GPDR1
        mov     r1,     #0x0001F000
        orr     r1, r1, #0x00000FF0
        orr     r1, r1, #0x0000000F
@        str    r1, [r0, #0x14] @ GPDR2
                                                                                
        mov     r1,     #0x81000000
        orr     r1, r1, #0x00000004
        str    r1, [r0, #0x54] @ GAFR0_L
        mov     r1,     #0xA5000000
        orr     r1, r1, #0x00000010
        str    r1, [r0, #0x58] @ GAFR0_U
                                                                                
        mov     r1,     #0x68000000
        orr     r1, r1, #0x00900000
        orr     r1, r1, #0x00008000
        orr     r1, r1, #0x00000018
        str    r1, [r0, #0x5C] @ GAFR1_L
        mov     r1,     #0xAA000000
        orr     r1, r1, #0x00A50000
        orr     r1, r1, #0x0000AA00
        orr     r1, r1, #0x000000AA
        str    r1, [r0, #0x60] @ GAFR1_U
                                                                                
        mov     r1,     #0xAA000000
        orr     r1, r1, #0x00AA0000
        orr     r1, r1, #0x0000AA00
        orr     r1, r1, #0x000000AA
        str    r1, [r0, #0x64] @ GAFR2_L
        mov     r1,     #0x00000002
        str    r1, [r0, #0x68] @ GAFR2_U
                                                                                
        @ Initialize MSCs
        mov   r0,     #0x48000000
        mov   r1,     #0x12000000     @ set to 124c2989 in WinCE
        orr   r1, r1, #0x004c0000
        orr   r1, r1, #0x00002900
        orr   r1, r1, #0x00000089
        str   r1, [r0, #0x0C]         @ MSC1
        ldr   r4, [r0, #0x0C]
#endif

	mov	r6, #0x20000000
	sub	r6, r6, #1	
	@ ok we're ready now
	mcr	p14, 0, r6, c8, c0, 0	
loop:	@ now we wait for debugger signal
	mrc	p14, 0, r15, c14, c0, 0
	bpl	loop

	mcr	p14, 0, r0, c9, c0, 0

	@ jump to 0xC0000000, where we assume that the debugger has loaded
	@ stuff into the icache at.

	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
	mov pc, #0xC0000000
