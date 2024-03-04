nop
nop
init:
    mov 0 r1  ; Initialize r1 to 0
    mov 3 r2 ; Initialize r2 to 5

loop:
    add 1 r1  ; Increment r1 by 1
    sub 0 r1
    cmp r1 r2  ; Compare r1 with r2
    jeq exit  ; If r1 is 5, jump to exit
    jmp loop  ; Otherwise, jump back to the start of the loop

exit:
    mov 420 r1 ; Move 420 to r9 to indicate completion
    hlt        ; Halt execution