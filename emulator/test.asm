nop
call label1
hlt

label2:
    sub 1 r1
    ret

label1:
    mov 70 r1
    call label2
    ret
