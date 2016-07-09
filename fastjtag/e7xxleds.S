# This program is copyright Ian Molton (c) 2003
# You may NOT remove this header under any circumstances
# You may use this code subject to the GPLv2

.globl _start

_start:	mov r0, r0
# Load GPIO base address into r0
        mov r0, #0x40000000
        orr r0, r0, #0x00e00000

#set GPIO 23 to OUTPUT (read, modify, write)
        ldr r1, [r0, #0x0c]
        orr r1, r1, #0x00400000
        str r1, [r0, #0x0c]

.loopa: mov r1, #0x00400000 
        str r1, [r0, #0x18]

        mov r4, #0x1000000 
.loop:  sub r4, r4, #1
        cmp r4, #0
        bne .loop

        mov r1, #0x00400000 
        str r1, [r0, #0x24]

        mov r4, #0x1000000 
.loopb: sub r4, r4, #1
        cmp r4, #0
        bne .loopb

        b .loopa         

.end:   b .end
