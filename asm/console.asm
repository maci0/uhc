start:
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

hlt

.EQU control_register $0x01100000
.EQU data_register $0x01100008


write_char:
    str r1 data_register
    mov 1 r1
    str r1 control_register ; enable write
    ret