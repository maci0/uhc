#!/usr/bin/env python3

from dataclasses import dataclass

# | Opcode (8 bits) | Addressing Mode 1 (8 bits) | Addressing Mode 2 (8 bits) | Operand 1 (64 bits) | Operand 2 (64 bits) |
# |-----------------|----------------------------|----------------------------|----------------------|----------------------|
# | 1 byte          | 1 byte                     | 1 byte                     | 8 bytes              | 8 bytes              |
# |-----------------|----------------------------|----------------------------|----------------------|----------------------|
# | 0x01            | 0x01                       | 0x01                       | 0x0000000000000000   | 0x0000000000000000   |
# |-----------------|----------------------------|----------------------------|----------------------|----------------------|

INSTRUCTION_WIDTH = 1 + 1 + 1 + 8 + 8

@dataclass
class Opcode:
    # No Operation
    NOP: int = 0x00
    
    # Data Movement
    MOV: int = 0x01
    
    # Arithmetic Operations
    ADD: int = 0x02
    SUB: int = 0x03
    MUL: int = 0x04
    DIV: int = 0x05
    
    # Bitwise Operations
    AND: int = 0x06
    OR: int = 0x07
    XOR: int = 0x08
    NOT: int = 0x09
    LSH: int = 0x0A
    RSH: int = 0x0B
    
    # Comparison and Conditional Branching
    JMP: int = 0x0C
    CMP: int = 0x0D
    JEQ: int = 0x0E
    
    # System Control
    RST: int = 0xFE
    HLT: int = 0xFF 

@dataclass
class AddressingMode:
    NONE = 0       # No Addressing Mode
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
        elif operand.startswith('*'):
            return AddressingMode.INDIRECT
        elif operand.startswith('0X'):
            return AddressingMode.DIRECT
        elif operand.isalnum():
            return AddressingMode.IMMEDIATE
        else:
            print("Error: Invalid addressing mode")
            return None
    def to_int(operand: str):
        if operand.startswith('R'):
            return int(operand[1:])
        elif operand.startswith('*'):
            return int(operand[2:])
        elif operand.startswith('0X'):
            return int(operand, 16)
        elif operand.isalnum():
            return int(operand)
        else:
            print("Error: Invalid operand")
            return None

@dataclass
class Instruction:
    opcode: Opcode
    addressing_mode1: AddressingMode
    addressing_mode2: AddressingMode
    operand1: Operand
    operand2: Operand

    def print(self):
        print (self.__repr__)
        print ("Opcode:", self.opcode, "\nAllowed addressing modes operand 1:", self.addressing_mode1,"\nAllowed addressing modes operand 2:", self.addressing_mode2,"\nOperand 1:", self.operand1, "\nOperand 2:",self.operand2)

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
            if AddressingMode.__dict__[addressing_mode] == self.addressing_mode1:
                buffer += "Addressing Mode 1: " + addressing_mode + "\n"
                break
        for addressing_mode in AddressingMode.__dict__.keys():
            if AddressingMode.__dict__[addressing_mode] == self.addressing_mode2:
                buffer += "Addressing Mode 2: " + addressing_mode + "\n"
                break
        #buffer += "Addressing Mode 1: " + str(self.addressing_mode1) + "\n"
        #buffer += "Addressing Mode 2: " + str(self.addressing_mode2) + "\n"
        buffer += "Operand 1: " + str(self.operand1) + "\n"
        buffer += "Operand 2: " + str(self.operand2) + "\n"
        return buffer
    
    def encode(opcode, addressing_mode1, addressing_mode2, operand1, operand2):
        ba = bytearray()
        ba.extend(opcode.to_bytes(1, byteorder="little"))
        ba.extend(addressing_mode1.to_bytes(1, byteorder="little"))
        ba.extend(addressing_mode2.to_bytes(1, byteorder="little"))
        ba.extend(operand1.to_bytes(8, byteorder="little"))
        ba.extend(operand2.to_bytes(8, byteorder="little"))
        return (ba)

    def decode(ba: bytearray):
        opcode = int.from_bytes(ba[0:1], byteorder="little")
        addressing_mode1 = int.from_bytes(ba[1:2], byteorder="little")
        addressing_mode2 = int.from_bytes(ba[2:3], byteorder="little")
        operand1 = int.from_bytes(ba[3:11], byteorder="little")
        operand2 = int.from_bytes(ba[11:19], byteorder="little")
        return Instruction(opcode, addressing_mode1, addressing_mode2, operand1, operand2)    


# Define the instruction set
instruction_set = {
    "MOV": Instruction(Opcode.MOV, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "ADD": Instruction(Opcode.ADD, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "SUB": Instruction(Opcode.SUB, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "MUL": Instruction(Opcode.MUL, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "DIV": Instruction(Opcode.DIV, AddressingMode.ALL, AddressingMode.ALL_RW, Operand, Operand),
    "JMP": Instruction(Opcode.JMP, AddressingMode.ALL_RW, AddressingMode.NONE, Operand, Operand),
    "JEQ": Instruction(Opcode.JEQ, AddressingMode.ALL_RW, AddressingMode.NONE, Operand, Operand),
    "RST": Instruction(Opcode.RST, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
    "NOP": Instruction(Opcode.NOP, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
    "HLT": Instruction(Opcode.HLT, AddressingMode.NONE, AddressingMode.NONE, Operand, Operand),
}