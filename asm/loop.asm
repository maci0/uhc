loop:
jmp loop
mov 69 r2
mov 0x48 r1
call write_char
jmp loop



.EQU control_register $0x01100000
.EQU data_register $0x01100008


write_char:
    str r1 data_register
    mov 1 r1
    str r1 control_register ; enable write
    ret