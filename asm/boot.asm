start:
    str sp $0 ; store initial sp to $0
    mov 1000 r1 ; set r1 to 1000 
    str r1 $8 ; store r1 to reset vector
    mov 256 r1 ; set r1 to 256
    str r1 $16 ; store r1 to NMI interrupt vector
    mov 264 r1 ; set r1 to 264
    str r1 $24 ; store r1 HardFault interrupt vector
    mov r0 r1
    jmp 1000



.org 256
    hlt

.org 1000
mov 420 r1
jmp $0