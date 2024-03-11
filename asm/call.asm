mov 69 r1
call addonetor1
mov r1 r2
hlt

addonetor1:
    add 1 r1
    call mov2tor3 ; nested function calls don't work yet
    ret

mov2tor3:
    add 2 r3
    ret
