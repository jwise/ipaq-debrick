# gibber.S
#
# This is used to test wether or not rterm is functioning. Once loaded onto the
# PXA it will spew abcdef...xyz repeatedly

		mov     r1, #'a'
1:              mrc     p14, 0, r15, c14, c0, 0
                bvs     1b
		mcr     p14, 0, r1, c8, c0, 0
		add	r1, r1, #1
		cmp	r1, #'z'
		movgt	r1, #'a'
		b	1b
	
