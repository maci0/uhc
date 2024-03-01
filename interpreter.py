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
mem = [0] * 8192  # 8 MB of memory
sp = 0  # Stack Pointer
pc = 0  # Program Counter
sr = 0  # Status Register


def reset_state():
    """Resets the state of the CPU."""
    global pc, sr, sp, registers, stack, mem
    pc = 0
    sr = 0
    sp = 0
    registers = [0] * 64
    stack = [0] * 1024
    mem = [0] * 8192


def halt():
    """Stops the execution of the program."""
    global running
    print("Halting")
    running = False

def fetch_instruction(file):
    global pc

    file.seek(pc * isa.INSTRUCTION_WIDTH)

    buf = file.read(isa.INSTRUCTION_WIDTH)
    if not buf:
        return None
        exit (1)
    else:
        return buf

def validate_instruction(instruction):
    cache = isa.Instruction.validate
    return cache

def decode_instruction(instruction):
    cache = isa.Instruction.decode(instruction)
    return cache 

def get_byte(mode, operand):
    if mode & isa.AddressingMode.ALL == 0:        assert False, f"{__func__()}: Invalid addressing mode: {mode} for operand {operand}"
    elif mode == isa.AddressingMode.IMMEDIATE:    return operand
    elif mode == isa.AddressingMode.REGISTER:     return registers[operand]
    elif mode == isa.AddressingMode.DIRECT:       return mem[operand]
    elif mode == isa.AddressingMode.INDIRECT:     assert False, f"{__func__()}: Not implemented yet"

def set_byte(mode, operand, value):
    if mode & isa.AddressingMode.ALL == 0:        assert False, f"{__func__()}: Invalid addressing mode: {mode} for operand {operand}"
    elif mode == isa.AddressingMode.IMMEDIATE:    assert False, f"{__func__()}: Invalid addressing mode: {mode} for operand {operand}"
    elif mode == isa.AddressingMode.REGISTER:     registers[operand] = value
    elif mode == isa.AddressingMode.DIRECT:       mem[operand] = value
    elif mode == isa.AddressingMode.INDIRECT:     assert False, f"{__func__()}: Not implemented yet"
    
def execute_instruction(instruction):
    global pc, sr

    if instruction.opcode == isa.Opcode.NOP:
        pass

    elif instruction.opcode == isa.Opcode.MOV:
        set_byte(instruction.addressing_mode2, instruction.operand2, get_byte(instruction.addressing_mode1, instruction.operand1))

    elif instruction.opcode == isa.Opcode.ADD:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) + get_byte(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.SUB:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) - get_byte(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.MUL:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) * get_byte(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.DIV:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) / get_byte(instruction.addressing_mode1, instruction.operand1)))

    elif instruction.opcode == isa.Opcode.JMP:
        pc = get_byte(instruction.addressing_mode1, instruction.operand1)

    elif instruction.opcode == isa.Opcode.CMP:
        if get_byte(instruction.addressing_mode1, instruction.operand1) == get_byte(instruction.addressing_mode2, instruction.operand2):
            sr |= 0x1
        else:
            sr &= ~0x1

    elif instruction.opcode == isa.Opcode.JEQ:
        if sr & 0x1:
            pc = get_byte(instruction.addressing_mode1, instruction.operand1)
            sr &= ~0x1

    elif instruction.opcode == isa.Opcode.RST:
        print("Resetting")
        reset_state()

    elif instruction.opcode == isa.Opcode.HLT:
        halt()

    else:
        print("Invalid Opcode", instruction.opcode)
        exit(1)


while running:
    print("PC:", pc, "SR", sr, "Registers 0-10:", registers[0:10])
    print("MEM 0-10:", mem[0:10])

    # print("Fetching Instruction")
    instruction = fetch_instruction(file)
    instruction = decode_instruction(instruction)
    print("Executing:", instruction)
    execute_instruction(instruction)
    pc +=1


    # print(cache.prettyprint())
    #time.sleep(0.5)


file.close()
