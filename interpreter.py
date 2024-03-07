#!/usr/bin/env python3
import isa
import inspect
import time


def __func__():
    """Returns the name of the calling function using inspect."""
    return inspect.stack()[1].function


file = open("test.bin", "rb")
instruction: isa.Instruction = 0
running = True

registers = [0] * 64
stack = [0] * 1024  # 1 megabyte of memory
mem  = [0] * 8192  # 8 MB of memory
sp = 8000  # Stack Pointer
pc = 0  # Program Counter
sr = 0  # Status Register
ir = [0] * 5 # Instruction register
ra = 0 # return address register


def reset_state():
    """Resets the state of the CPU."""
    global pc, sr, sp, registers, stack, mem, ir, ra
    pc = 0
    sr = 0
    sp = 8000
    registers = [0] * 64
    stack = [0] * 1024
    mem = [0] * 8192
    ir = [0] * 5
    ra = 0


def halt():
    """Stops the execution of the program."""
    global running
    print("Halting")
    running = False

def fetch_instruction(file):
    global pc, ir

    file.seek(pc * isa.INSTRUCTION_WIDTH)

    ir = file.read(isa.INSTRUCTION_WIDTH)
    if not ir:
        return None
        exit (1)

def validate_instruction(instruction):
    cache = isa.Instruction.validate
    return cache

def decode_instruction():
    global ir
    cache = isa.Instruction.decode(ir)
    return cache 

def get_value(mode, operand):
    if mode & isa.AddressingMode.ALL == 0:        assert False, f"{__func__()}: Invalid addressing mode: {mode} for operand {operand}"
    elif mode == isa.AddressingMode.IMMEDIATE:    return operand
    elif mode == isa.AddressingMode.REGISTER:     return registers[operand]
    elif mode == isa.AddressingMode.DIRECT:       return mem[operand]
    elif mode == isa.AddressingMode.INDIRECT:     assert False, f"{__func__()}: Not implemented yet"

def set_value(mode, operand, value):
    if mode & isa.AddressingMode.ALL == 0:        assert False, f"{__func__()}: Invalid addressing mode: {mode} for operand {operand}"
    elif mode == isa.AddressingMode.IMMEDIATE:    assert False, f"{__func__()}: Invalid addressing mode: {mode} for operand {operand}"
    elif mode == isa.AddressingMode.REGISTER:     registers[operand] = value
    elif mode == isa.AddressingMode.DIRECT:       mem[operand] = value
    elif mode == isa.AddressingMode.INDIRECT:     assert False, f"{__func__()}: Not implemented yet"
    
def execute_instruction(instruction):
    global pc, sr, ra, sp
    pc +=1

    if instruction.opcode == isa.Opcode.NOP:
        pass

    elif instruction.opcode == isa.Opcode.MOV:
        set_value(instruction.addressing_mode2, instruction.operand2, get_value(instruction.addressing_mode1, instruction.operand1))

    elif instruction.opcode == isa.Opcode.PUSH:
        sp -= 8
        set_value(isa.AddressingMode.DIRECT, sp, get_value(instruction.addressing_mode1, instruction.operand1))

    elif instruction.opcode == isa.Opcode.POP:
        set_value(instruction.addressing_mode1, instruction.operand1, get_value(isa.AddressingMode.DIRECT, sp))
        sp += 8

    elif instruction.opcode == isa.Opcode.ADD:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) + get_value(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.SUB:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) - get_value(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.MUL:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) * get_value(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.DIV:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) / get_value(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.JMP:
        pc = get_value(instruction.addressing_mode1, instruction.operand1)

    elif instruction.opcode == isa.Opcode.CALL:
        ra=pc+1
        pc = get_value(instruction.addressing_mode1, instruction.operand1)

    elif instruction.opcode == isa.Opcode.RET:
        pc=ra
        ra=0

    elif instruction.opcode == isa.Opcode.CMP:
        if get_value(instruction.addressing_mode1, instruction.operand1) == get_value(instruction.addressing_mode2, instruction.operand2):
            #print ("equal")
            sr |= 0x1
        else:
            #print ("not equal")
            sr &= ~0x1

    elif instruction.opcode == isa.Opcode.JEQ:
        if sr & 0x1:
            pc = get_value(instruction.addressing_mode1, instruction.operand1)
            sr &= ~0x1

    elif instruction.opcode == isa.Opcode.RST:
        print("Resetting")
        reset_state()

    elif instruction.opcode == isa.Opcode.HLT:
        halt()

    else:
        print("Invalid Opcode", instruction.opcode)
        exit(1)



def print_status():
    print("PC: %u | SP: %u | R[0-10]: %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu"  %(
        pc, sp, ra,
        registers[0], registers[1], registers[2], registers[3], registers[4],
        registers[5], registers[6], registers[7], registers[8], registers[9], registers[10]))

    # Print the first 10 elements of mem[]
    print("SR: %u | ##### |" %(sr), end="")
    print(" M[0-15]: ", end="")
    for i in range(16):
        print("%02X " % mem[i], end="")
    print("")
    
    pass

while running:
    

    #print_status()
    fetch_instruction(file)
    instruction = decode_instruction()
    print("Executing:", instruction)
    execute_instruction(instruction)
    # dump state to file

    dump_file = open("test.dump", "w")



    dump_file.write(str(pc))
    dump_file.write(str(sp))
    dump_file.write(str(sr))
    dump_file.write(str(ra))
    dump_file.write(str(ir))
    dump_file.write(str(registers))
    dump_file.write(str(mem))

    # Close files
    dump_file.close()

    #print_status()
    #

    # print(cache.prettyprint())
    #time.sleep(0.5)


file.close()
