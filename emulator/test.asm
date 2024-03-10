nop

mov 0x48 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x65 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x6C r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x6C r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x6F r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x20 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x57 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write


mov 0x6F r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x72 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x6C r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x64 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x21 r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write

mov 0x0A r1
str r1 $0x01100008
mov 1 r1
str r1 $0x01100000 ; enable write
str r0 $0x01100000 ; disable write


ldr $0x01000000 r3