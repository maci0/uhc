;init:
;    mov 0 r1  ; Initialize r1 to 0
;    mov 5 r2 ; Initialize r2 to 5

;loop:
;    cmp r1 r2
;    jeq end
;    add 1 r1
;    ;push r2
;    jmp loop

mov 420 r5
call foo
sub 1 r2
hlt
foo:
    mov 70 r2
    ret