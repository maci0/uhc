nop

init:
    mov 0 r9  ; Initialize r9 to 3
    mov 5 r8 ; Initialize r8 to 5

loop:
    add 1 r9  ; Increment r9 by 1
    cmp r8 r9  ; Compare r9 with 0
    jeq exit  ; If r9 is 3, jump to exit
    jmp loop  ; Otherwise, jump back to the start of the loop

exit:
    mov 420 r9 ; Move 420 to r9 to indicate completion
    hlt        ; Halt execution