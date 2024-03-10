nop

mov 0x48 r1
call write_char
mov 0x65 r1
call write_char
mov 0x6C r1
call write_char
mov 0x6C r1
call write_char
mov 0x6F r1
call write_char
mov 0x20 r1
call write_char
mov 0x57 r1
call write_char
mov 0x6F r1
call write_char
mov 0x72 r1
call write_char
mov 0x6C r1
call write_char
mov 0x64 r1
call write_char
mov 0x21 r1
call write_char
mov 0x0A r1
call write_char

ldr $0x01000000 r3 ; read from this rom address to r3

hlt

.EQU mmio_writer $0x01100008

write_char:
    str r1 mmio_writer
    mov 1 r1
    str r1 $0x01100000 ; enable write
    str r0 $0x01100000 ; disable write
    ret