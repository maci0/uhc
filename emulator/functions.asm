call label1
;pop r3
hlt

label2:
    sub 1 r1
    ;push r1
    mov 420 r2
    ret

label1:
    mov 70 r1
    push r1
    call label2
    pop r5
    ret