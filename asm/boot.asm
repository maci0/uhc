boot:
    mov initial_sp sp ; set the stack pointer
    str sp $0 ;
    
    mov start r1 ;
    str r1 reset_vector ; 
    
    mov nmi_handler r1 
    str r1 nmi_vector ; 
    
    mov r0 r1 ; clean up the registers
    jmp start

nmi_handler:
    mov 0x48 r1
    call write_char

start:
jmp start

.EQU reset_vector $8
.EQU nmi_vector $16
.EQU initial_sp 0x8000



write_char:
    str r1 data_register
    mov 1 r1
    str r1 control_register ; enable write
    ret

.EQU control_register $0x01100000 ; fileout device
.EQU data_register $0x01100008 ; fileout device