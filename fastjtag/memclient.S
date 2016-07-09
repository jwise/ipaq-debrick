@ Debug handler code
@ Ripped straight out of 278525-001.pdf

.globl _start
_start:
	@ Wait for a packet from the debugger.
pkt1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	pkt1

	@ Allow things to settle
	mov	r1, #0x1000
wait1:	sub	r1, r1, #0x1
	cmp	r1, #0x0
	bne	wait1
 
	mrc	p14, 0, r12, c9, c0, 0
	
	@ At this point, we have an packet in r12.
	
	@ Read packet? (0x11)
	cmp	r12,	#0x11
	beq	read

	@ Halfread packet? (0x10)
	cmp	r12,	#0x10
	beq	readh

	@ Fast read packet? (0x12)
	cmp	r12,	#0x12
	beq	fread
	
	@ Write packet? (0x21)
	cmp	r12,	#0x21
	beq	write

	@ Halfwrite packet? (0x20)
	cmp	r12,	#0x20
	beq	writeh

	@ Start fast write packet? (0x28)
	cmp	r12,	#0x28
	beq	fwrite
	
	@ Start fast halfwrite packet? (0x27)
	cmp	r12,	#0x27
	beq	fwriteh

	@ Start fast h1910 flash write? (0x30)
	cmp	r12,	#0x30
	beq	f1910write

	@ Start fast h3900 flash write? (0x31)
	cmp	r12,	#0x31
	beq	f3900write
	
	@ Start fast e7xx flash erase (0x32)
        cmp     r12,    #0x32
        beq     e7xx_flash_erase

	@ Start fast e7xx flash write (0x33)
        cmp     r12,    #0x33
        beq     e7xx_flash_write

	@ Check packet? (0xFE)
	cmp	r12,	#0xFE
	beq	check

@ If we get here something went wrong. Attempt to restart.
@ FIXME - for some reason we ALWAYS end up here before we do anything else
@ Probably crap in one of the registers.

	b _start

proccpt:
	 bl	waitrecv  @ de-pipeline things.
	@ ACK packet completion. (Send 0x10000000)
	@ mov	r6,     #0x10000000
	@ mcr	p14, 0, r6, c8, c0, 0
	
	@ We have completed handling the packet; go back to the beginning.
	b	_start	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

read:	mrc	p14, 0, r15, c14, c0, 0
	mrcmi	p14, 0, r1, c9, c0, 0
	bpl	read
	@ Address in r1.
	
	bl	waitrecv
	
	ldr	r2, [r1]
	@orr	r2, r1, #0xAA000000
	mcr	p14, 0, r2, c8, c0, 0
	b	proccpt

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

readh:	mrc	p14, 0, r15, c14, c0, 0
	mrcmi	p14, 0, r1, c9, c0, 0
	bpl	readh
	@ Address in r1.
	
	bl	waitrecv
	
	ldrh	r2, [r1]
	mcr	p14, 0, r2, c8, c0, 0
	b	proccpt
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

fread:	mrc	p14, 0, r15, c14, c0, 0
	mrcmi	p14, 0, r4, c9, c0, 0
	bpl	fread
	@ Address in r4
	
wordl:	mrc	p14, 0, r15, c14, c0, 0
	mrcmi	p14, 0, r5, c9, c0, 0
	bpl	wordl
	@ Word count in r5
	
sndl:	cmp	r5, #0x0
	beq	proccpt
	
	bl	waitrecv
	
	ldr	r2, [r4]
	mcr	p14, 0, r2, c8, c0, 0
	
	sub	r5, r5, #0x1
	add	r4, r4, #0x4
	
	b	sndl

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

write:	mrc	p14, 0, r15, c14, c0, 0
	bpl	write

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r1, c9, c0, 0

1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	1b

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r2, c9, c0, 0

	@ Address in r1, data in r2
	str	r2, [r1]
	b	proccpt

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

writeh:	mrc	p14, 0, r15, c14, c0, 0
	bpl	writeh

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r1, c9, c0, 0

1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	1b

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r2, c9, c0, 0

	@ Address in r1, data in r2
	strh	r2, [r1]

	bl	waitrecv
	mcr	p14, 0, r2, c8, c0, 0
	
	b	proccpt

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

fwrite:	mrc	p14, 0, r15, c14, c0, 0
	bpl	fwrite

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r1, c9, c0, 0

1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	1b

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r2, c9, c0, 0
	
	mov	r3, #0xF0000000
	sub	r3, r3, #1
	cmp	r3, r1
	beq 	proccpt

	@ Address in r1, data in r2
	str	r2, [r1]
	b	fwrite

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

fwriteh:mrc	p14, 0, r15, c14, c0, 0
	bpl	fwriteh

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r1, c9, c0, 0

1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	1b

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r2, c9, c0, 0
	
	mov	r3, #0xF0000000
	sub	r3, r3, #1
	cmp	r3, r1
	beq 	proccpt

	@ Address in r1, data in r2
	strh	r2, [r1]
	b	fwriteh

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

f1910write:
	mrc	p14, 0, r15, c14, c0, 0
	bpl	f1910write

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r1, c9, c0, 0

1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	1b

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r2, c9, c0, 0
	
	mov	r3, #0xF0000000
	sub	r3, r3, #1
	cmp	r3, r1
	beq 	proccpt

	@ Address in r1, data in r2
	mov	r3, #0x000000A0
	orr  r3,r3, #0x00A00000

	mov	r6, #0x10000
	sub	r6, r6, #1
	
	strh	r3, [r1]
	strh	r2, [r1]
1:	ldrh	r4, [r1]	@ wait for write complete
	and	r4, r4, r6
	and	r5, r2, r6
	cmp	r4, r5
	bne	1b
	mov	r2, r2, lsr #16	@advance 16 bits
	add  r1,r1, #0x2
	strh	r3, [r1]
	strh	r2, [r1]
1:	ldrh	r4, [r1]	@ wait for write complete
	and	r4, r4, r6
	and	r5, r2, r6
	cmp	r4, r5
	bne	1b
	
	b	f1910write

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
f3900write:
	mrc	p14, 0, r15, c14, c0, 0
	bpl	f3900write

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r1, c9, c0, 0

1:	mrc	p14, 0, r15, c14, c0, 0
	bpl	1b

	mov	r3, #0x1000
1:	sub	r3, r3, #0x1
	cmp	r3, #0x0
	bne	1b
	mrc	p14, 0, r2, c9, c0, 0
	
	mov	r3, #0xF0000000
	sub	r3, r3, #1
	cmp	r3, r1
	beq 	proccpt

	@ Address in r1, data in r2
	mov	r3, #0x00000040
	orr  r3,r3, #0x00400000

	str	r3, [r1]
	str	r2, [r1]
	
1:	ldr	r2, [r1]
	mov	r3, #0x00800000
	orr	r3, r3, #0x00000080
	and	r2, r2, r3
	cmp	r3, r2
	bne	1b		@ if not ready, look at status register again
	
	b	f3900write

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
e7xx_flash_write:
	mov 	r4, #0	

1:	mrc     p14, 0, r15, c14, c0, 0
        bpl     1b
        mov     r3, #0x1000
1:      sub     r3, r3, #0x1
        cmp     r3, #0x0
        bne     1b
	mrc     p14, 0, r1, c9, c0, 0  @ Flash address

1:      mrc     p14, 0, r15, c14, c0, 0
        bpl     1b
        mov     r3, #0x1000
1:      sub     r3, r3, #0x1
        cmp     r3, #0x0
        bne     1b
	mrc     p14, 0, r2, c9, c0, 0  @ Data to flash

	mov     r3, #0x1000000
        sub     r3, r3, #2
        cmp     r3, r1
        beq     proccpt                @ We're done, get outta here	

	mov     r3, #0x00000050
	orr	r3, r3, r3, lsl #16
	str	r3, [r4]               @ Clear status
	
	mov     r3, #0x00000040
	orr     r3, r3, r3, lsl #16
	str	r3, [r1]               @ Setup write
	
	str	r2, [r1]               @ Do the write

	mov     r3, #0x00000070
	orr     r3, r3, r3, lsl #16
	str     r3, [r4]               @ Enter Read Status reg mode

	mov     r3, #0x00000080
	orr     r3, r3, r3, lsl #16

1:	ldr     r2, [r4]
	and     r2, r2, r3
	cmp     r3, r2
	bne     1b              @ if not ready, look at status register again

	b	e7xx_flash_write

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

e7xx_flash_erase:
	mov 	r4, #0	

1:      mrc     p14, 0, r15, c14, c0, 0
        bpl     1b
	mov     r3, #0x1000
1:      sub     r3, r3, #0x1
        cmp     r3, #0x0
        bne     1b
        mrc     p14, 0, r1, c9, c0, 0  @ Flash block nr

        mov     r3, #0x00000050
        orr     r3, r3, r3, lsl #16
        str     r3, [r4]               @ Clear status
                                                                                
        mov     r3, #0x00000020
        orr     r3, r3, r3, lsl #16
        str     r3, [r1]               @ Setup erase
                     
	mov     r3, #0x000000D0
	orr     r3, r3, r3, lsl #16
	str     r3, [r1]               @ Do the erase                                                           
        mov     r3, #0x00000070
        orr     r3, r3, r3, lsl #16
        str     r3, [r4]               @ Enter Read Status reg mode
                                                                                
        mov     r3, #0x00000080
        orr     r3, r3, r3, lsl #16
                                                                                
1:      ldr     r2, [r4]
        and     r2, r2, r3
	cmp     r3, r2
        bne     1b              @ if not ready, look at status register again

	mov     r3, #0x000000ff
	orr     r3, r3, r3, lsl #16
	str     r3, [r4]               @ Enter Read array mode
                                                                                
        b       proccpt
                                                                                
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

check:	bl	waitrecv
	@ Communicate to the debugger that we are ready.
	@ (Send 0x0FFFFFFE)
	mov	r6,     #0x10000000
	sub	r6, r6, #0x2
	mcr	p14, 0, r6, c8, c0, 0
	
	b	proccpt

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
waitrecv:
	mrc     p14, 0, r15, c14, c0, 0
	bvs	waitrecv
	mov	pc,lr
