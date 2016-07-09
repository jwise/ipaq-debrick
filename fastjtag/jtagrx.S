@ Debug handler code (simple version)
@ Ripped straight out of 278525-001.pdf

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

	@ Signal that we are ready now
	mov	r6, #0x20000000
	sub	r6, r6, #1	
	mcr	p14, 0, r6, c8, c0, 0	

loop:	@ now we wait for debugger signal
	mrc	p14, 0, r15, c14, c0, 0
	bpl	loop

	mcr	p14, 0, r0, c9, c0, 0

	@ jump to 0xC0000000, where we assume that the debugger has loaded
	@ stuff into the icache at. (FIXME: perhaps we should allow the
	@ debugger to specify this address?)

	mov pc, #0xC0000000
