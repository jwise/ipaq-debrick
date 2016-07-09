@ Tweaks h1900 LEDs

.globl _start

_start:
	mov	r10,     #0x0C000000	@ h1900 ASIC3 base
	add	r9, r10, #0x00000700	@ red LED base
	add	r8, r9,  #0x00000000	@ timebase
	mov	r11, #(0x6 | 1 << 4)	@ set up
	str	r11, [r8]
	
	add	r8, r9,  #0x00000004	@ period time
	mov	r11, #0x40
	str	r11, [r8]
	
	add	r8, r9,  #0x00000008	@ duty time
	mov	r11, #0x0
	str	r11, [r8]
	
	add	r9, r10, #0x00000800	@ green LED base
	add	r8, r9,  #0x00000000	@ timebase
	mov	r11, #(0x6 | 1 << 4)	@ set up
	str	r11, [r8]
	
	add	r8, r9,  #0x00000004	@ period time
	mov	r11, #0x40
	str	r11, [r8]
	
	add	r8, r9,  #0x00000008	@ duty time
	mov	r11, #0x41
	str	r11, [r8]
	
loop:	b	loop
