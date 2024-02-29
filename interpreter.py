#!/usr/bin/env python3
import isa
import time

file = open("test.bin", "rb")
cache: isa.Instruction = 0

running = True

registers = [0] * 64
stack = [0] * (2**20)  # 1 megabyte of memory
sp = 0  # Stack Pointer
pc = 0  # Program Counter
sr = 0  # Status Register


def fetch_instruction():
    global cache
    global pc

    file.seek(pc * isa.INSTRUCTION_WIDTH)

    instruction = file.read(isa.INSTRUCTION_WIDTH)
    if not instruction:
        return None
    else:
        cache = isa.Instruction.decode(instruction)
        # print(instruction)

        return instruction


def execute_instruction():
    global registers
    global cache
    global pc
    global sr
    global running

    pc += 1

    # Todo: make source and destination aware of addressing modes
    # for now just implement IMMEIDATE and REGISTER addressing modes
    if cache.addressing_mode1 == isa.AddressingMode.IMMEDIATE:
        source_value = cache.operand1
    elif cache.addressing_mode1 == isa.AddressingMode.REGISTER:
        source_value = registers[cache.operand1]
    elif cache.addressing_mode1 == isa.AddressingMode.DIRECT:
        assert False, "not implemented yet"
    elif cache.addressing_mode1 == isa.AddressingMode.INDIRECT:
        assert False, "not implemented yet"

    if cache.addressing_mode2 == isa.AddressingMode.IMMEDIATE:
        assert False, "Invalid addressing mode for destination"
    elif cache.addressing_mode2 == isa.AddressingMode.REGISTER:
        destination = registers[cache.operand2]
    elif cache.addressing_mode2 == isa.AddressingMode.DIRECT:
        assert False, "not implemented yet"
    elif cache.addressing_mode2 == isa.AddressingMode.INDIRECT:
        assert False, "not implemented yet"

    if cache.opcode == isa.Opcode.MOV:
        registers[cache.operand2] = source_value

    elif cache.opcode == isa.Opcode.ADD:
        registers[cache.operand2] = source_value + destination

    elif cache.opcode == isa.Opcode.SUB:
        registers[cache.operand2] = registers[cache.operand2] - source_value

    elif cache.opcode == isa.Opcode.MUL:
        registers[cache.operand2] = source_value * destination

    elif cache.opcode == isa.Opcode.DIV:
        registers[cache.operand2] = source_value / destination

    elif cache.opcode == isa.Opcode.JMP:
        pc = source_value

    elif cache.opcode == isa.Opcode.NOP:
        pass

    elif cache.opcode == isa.Opcode.CMP:
        if source_value == destination:
            #print("Equal")
            sr |= 0x1
        else:
            #print("Not Equal")
            sr &= ~0x1

    elif cache.opcode == isa.Opcode.JEQ:
        if sr & 0x1:
            #print("Jumping to", source_value)
            pc = source_value
            sr &= ~0x1

    elif cache.opcode == isa.Opcode.HLT:
        print("Halting")
        running = False

    elif cache.opcode == isa.Opcode.RST:
        print("Resetting")
        pc = 0
        sr = 0
        sp = 0
        registers = [0] * 64

    else:
        print("Invalid Opcode", cache.opcode)
        exit(1)
        return False


while running:
    print("PC:", pc, "SR", sr, "Registers 0-10:", registers[0:10])

    # print("Fetching Instruction")
    instruction = fetch_instruction()
    if instruction is None:
        break
    print(cache)
    # print(cache.prettyprint())
    execute_instruction()
    #time.sleep(0.5)


file.close()
