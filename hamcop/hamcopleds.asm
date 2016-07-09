_start:
	mov	r0, #0x7200
	add	r1, r0, #0x0
	mov	r2, #0x00F3
	strh	r2, [r1]
	add	r1, r0, #0x4
	mov	r2, #0x0000
	strh	r2, [r1]
1:	b	1b
