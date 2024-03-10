#!/usr/bin/env python3

from dataclasses import dataclass

import ctypes

uint8_t = ctypes.c_uint64
uint64_t = ctypes.c_uint64

# | Opcode (8 bits) | Addressing Mode 1 (8 bits) | Addressing Mode 2 (8 bits) | Operand 1 (64 bits) | Operand 2 (64 bits) |
# |-----------------|----------------------------|----------------------------|----------------------|----------------------|
# | 1 byte          | 1 byte                     | 1 byte                     | 8 bytes              | 8 bytes              |
# |-----------------|----------------------------|----------------------------|----------------------|----------------------|
# | 0x01            | 0x01                       | 0x01                       | 0x0000000000000000   | 0x0000000000000000   |
# |-----------------|----------------------------|----------------------------|----------------------|----------------------|

INSTRUCTION_WIDTH = 1 + 1 + 1 + 8 + 8

@dataclass
class Opcode:
    # Illegal
    NONE: int = 0x00

    # No Operation
    NOP: int = 0x01
    
    # Data Movement
    MOV: int = 0x02
    PUSH: int = 0x03
    POP: int =  0x04
    
    # Arithmetic Operations
    ADD: int = 0x05
    SUB: int = 0x06
    MUL: int = 0x07
    DIV: int = 0x08
    
    # Bitwise Operations
    AND: int = 0x09
    OR: int  = 0x0A
    XOR: int = 0x0B
    NOT: int = 0x0C
    LSH: int = 0x0D
    RSH: int = 0x0E
    
    # Comparison and Conditional Branching
    JMP: int = 0x0F
    CMP: int = 0x10
    JEQ: int = 0x11

    # Function calls
    CALL: int = 200
    RET: int  = 201

    # Memory Operations
    LD8:  int = 240
    LD16: int = 241
    LD32: int = 242
    LD64: int = 243
    ST8:  int = 244
    ST16: int = 245
    ST32: int = 246
    ST64: int = 247
    
    # System Control
    RST: int = 0xFE
    HLT: int = 0xFF 

@dataclass
class AddressingMode:
    NONE: int = 0       # No Addressing Mode
    IMMEDIATE = 1  # Immediate Value - A fixed value, eg. '123' '0x123' '0b1010'
    REGISTER = 2   # Register Addressing Mode - A register, eg. r1 r2 r3
    DIRECT = 4     # Direct Memory Addressing Mode - A memory address, eg. 0x12345678
    INDIRECT = 8   # Indirect Memory Addressing Mode - A memory address contained in a register, eg. *r1 *r2 *r3
    ALL = IMMEDIATE | REGISTER | DIRECT | INDIRECT # All addressing modes
    ALL_RW = REGISTER | DIRECT | INDIRECT # All addressing modes except immediate

@dataclass
class Operand:
    NONE = 0 # No operand
    ADDRESS = 1 # Memory Address
    REGISTER = 2 # Register
    IMMEDIATE = 4 # Immediate Value
    ALL = ADDRESS | REGISTER | IMMEDIATE
    def infer_addressing_mode(operand: str):
        # set the addressing mode based on the value of the operand
        # 0. Immediate Value, eg. '123' '0x123' '0b1010' - example: ADD '123' r1 - adds 123 to r1, overwrites r1, stores result in r1
        # 1. Register Addressing Mode, eg. r1 r2 r3 - example: ADD r1 r2 - adds r1 to r2, overwrites r2, stores result in r2
        # 2. Direct Memory Addressing Mode eg. 0x12345678 - address of memory location - example: ADD 0x12345678 r1 - adds value at memory location 0x12345678 to r1, overwrites r1, stores result in r1
        # 3. Indirect Memory Addressing Mode eg. *r1 *r2 *r3 - address contained in register r1 r2 r3 - example: ADD *r1 r2 - adds value at memory location contained in r1 to r2, overwrites r2, stores result in r2
  

        if operand.startswith('R'):
            return AddressingMode.REGISTER
        elif operand.startswith('$'):
            return AddressingMode.DIRECT
        elif operand.startswith('*'):
            return AddressingMode.INDIRECT
        if operand.startswith('0X'):
            return AddressingMode.IMMEDIATE
        if operand.isdigit():
            return AddressingMode.IMMEDIATE
        else:
            print("Error: Invalid addressing mode")
            return None

    def to_int(operand: str):
        #strip operand up to the first digit, then check if it's a 0x value
        for i, char in enumerate(operand):
            if char.isdigit():
                operand = operand[i:]
                break

        #print ("operand", operand)

        if operand.startswith('0X'):
            return int(operand[2:], 16)
        elif operand.isdigit():
            return int(operand)
        else:
            print("Error: Invalid operand")
            return None

@dataclass
class Instruction:
    opcode: Opcode
    srcMode: AddressingMode
    destMode: AddressingMode
    srcOperand: Operand
    destOperand: Operand

    def print(self):
        print (self.__repr__)
        print ("Opcode:", self.opcode, "\nAllowed addressing modes operand 1:", self.srcMode,"\nAllowed addressing modes operand 2:", self.destMode,"\nOperand 1:", self.srcOperand, "\nOperand 2:",self.destOperand)

    def get_allowed_addressing_modes(bitmask: int):
        print ("Allowed addressing modes:", bin(bitmask)[2:].zfill(8))
        if bitmask & AddressingMode.IMMEDIATE:
            print ("Immediate Value")
        if bitmask & AddressingMode.REGISTER:
            print ("Register Addressing Mode")
        if bitmask & AddressingMode.DIRECT:
            print ("Direct Memory Addressing Mode")
        if bitmask & AddressingMode.INDIRECT:
            print ("Indirect Memory Addressing Mode")
    
    def prettyprint(self):
        buffer = ""
        for opcode in Opcode.__dict__.keys():
            if Opcode.__dict__[opcode] == self.opcode:
                buffer += "Opcode: " + opcode + "\n"
                break
        for addressing_mode in AddressingMode.__dict__.keys():
            if AddressingMode.__dict__[addressing_mode] == self.srcMode:
                buffer += "Addressing Mode 1: " + addressing_mode + "\n"
                break
        for addressing_mode in AddressingMode.__dict__.keys():
            if AddressingMode.__dict__[addressing_mode] == self.destMode:
                buffer += "Addressing Mode 2: " + addressing_mode + "\n"
                break
        #buffer += "Addressing Mode 1: " + str(self.srcMode) + "\n"
        #buffer += "Addressing Mode 2: " + str(self.destMode) + "\n"
        buffer += "Operand 1: " + str(self.srcOperand) + "\n"
        buffer += "Operand 2: " + str(self.destOperand) + "\n"
        return buffer
    
    def encode(opcode, srcMode, destMode, srcOperand, destOperand):
        ba = bytearray()
        ba.extend(opcode.to_bytes(1, byteorder="little"))
        ba.extend(srcMode.to_bytes(1, byteorder="little"))
        ba.extend(destMode.to_bytes(1, byteorder="little"))
        ba.extend(srcOperand.to_bytes(8, byteorder="little"))
        ba.extend(destOperand.to_bytes(8, byteorder="little"))
        return (ba)

    def decode(ba: bytearray):
        opcode = int.from_bytes(ba[0:1], byteorder="little")
        srcMode = int.from_bytes(ba[1:2], byteorder="little")
        destMode = int.from_bytes(ba[2:3], byteorder="little")
        srcOperand = int.from_bytes(ba[3:11], byteorder="little")
        destOperand = int.from_bytes(ba[11:19], byteorder="little")
        return Instruction(opcode, srcMode, destMode, srcOperand, destOperand)

    def validate(opcode, srcMode, destMode, srcOperand, destOperand):
        # check if it matches with instruction set
        pass    


# Define the instruction set
instruction_set = {
    "MOV": Instruction(Opcode.MOV, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "PUSH": Instruction(Opcode.PUSH, AddressingMode.ALL, AddressingMode.NONE, Operand, Operand),
    "POP": Instruction(Opcode.POP, AddressingMode.ALL, AddressingMode.NONE, Operand, Operand),
    "ADD": Instruction(Opcode.ADD, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "SUB": Instruction(Opcode.SUB, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "MUL": Instruction(Opcode.MUL, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "DIV": Instruction(Opcode.DIV, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "JMP": Instruction(Opcode.JMP, AddressingMode.ALL_RW, AddressingMode.NONE, Operand, Operand),
    "CMP": Instruction(Opcode.CMP, AddressingMode.ALL_RW, AddressingMode.ALL_RW, Operand, Operand),
    "JEQ": Instruction(Opcode.JEQ, AddressingMode.ALL_RW, AddressingMode.NONE, Operand, Operand),
    "CALL": Instruction(Opcode.CALL, AddressingMode.ALL_RW, AddressingMode.NONE, Operand, Operand),
    "RET": Instruction(Opcode.RET, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
    "RST": Instruction(Opcode.RST, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
    "NOP": Instruction(Opcode.NOP, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
    "HLT": Instruction(Opcode.HLT, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
}