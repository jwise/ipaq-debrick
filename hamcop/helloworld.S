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

.global _start
_start:
	dbg("Hello, world!\r\n");
1:	b	1b


out_char:
	ldr	r1, UART_BASE
1:	ldr     r2, [r1, #0x14]	@ FFLSR - Get UART status
	tst     r2, #0x40	@ LSR_TEMT - Check TX empty
	beq     1b
	str	r0, [r1, #0x0]	@ FFTHR
	ldr     r0, [r1, #0x14]	@ FFLSR
	mov	pc, lr
		
out_str:	mov	r4, lr
		mov	r5, r0
1:		ldrb	r0, [r5], #1
		tst	r0, r0
		moveq	pc, r4
		adr	lr, 1b
		b	out_char

UART_BASE:
	.long 	0x40100000
