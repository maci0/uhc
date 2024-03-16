from dataclasses import dataclass
import logging

#!/usr/bin/env python3


# Define the width of an instruction in bytes
INSTRUCTION_WIDTH = 1 + 1 + 1 + 8 + 8

# Define the available opcodes as an enumeration
@dataclass
class Opcode:
    NONE: int = 0
    NOP: int = 1
    MOV: int = 2
    PUSH: int = 3
    POP: int = 4
    ADD: int = 5
    SUB: int = 6
    MUL: int = 7
    DIV: int = 8
    AND: int = 9
    OR: int = 10
    XOR: int = 11
    NOT: int = 12
    LSH: int = 13
    RSH: int = 14
    JMP: int = 15
    CMP: int = 16
    JEQ: int = 17
    CALL: int = 200
    RET: int = 201
    LDR: int = 210
    STR: int = 211
    LD8: int = 240
    LD16: int = 241
    LD32: int = 242
    LD64: int = 243
    ST8: int = 244
    ST16: int = 245
    ST32: int = 246
    ST64: int = 247
    RST: int = 254
    HLT: int = 255

# Define the available addressing modes as an enumeration
@dataclass
class AddressingMode:
    NONE: int = 0
    IMMEDIATE: int = 1
    REGISTER: int = 2
    DIRECT: int = 4
    INDIRECT: int = 8
    ALL: int = IMMEDIATE | REGISTER | DIRECT | INDIRECT
    ALL_RW: int = REGISTER | DIRECT | INDIRECT

# Define the available operands as an enumeration
@dataclass
class Operand:
    NONE: int = 0
    ADDRESS: int = 1
    REGISTER: int = 2
    IMMEDIATE: int = 4
    ALL: int = ADDRESS | REGISTER | IMMEDIATE

    # Infer the addressing mode of an operand based on its string representation
    @staticmethod
    def infer_addressing_mode(operand: str):
        print(operand, type(operand))
        operand = operand.upper()
        if operand.startswith('R'):
            return AddressingMode.REGISTER
        elif operand.startswith('$'):
            return AddressingMode.DIRECT
        elif operand.startswith('*'):
            return AddressingMode.INDIRECT
        elif operand.startswith('0X') or operand.isdigit():
            return AddressingMode.IMMEDIATE
        else:
            raise ValueError(f"Error: Invalid addressing mode for operand '{operand}'")

    # Convert an operand from its string representation to an integer value
    @staticmethod
    def to_int(operand: str):
        operand = operand.upper()
        for i, char in enumerate(operand):
            if char.isdigit():
                operand = operand[i:]
                break
        if operand.startswith('0X'):
            return int(operand[2:], 16)
        elif operand.isdigit():
            return int(operand)
        else:
            raise ValueError(f"Error: Invalid operand: '{operand}'")

# Define the Instruction class
@dataclass
class Instruction:
    opcode: Opcode
    srcMode: AddressingMode
    destMode: AddressingMode
    srcOperand: Operand
    destOperand: Operand

    # Print a string representation of the instruction
    def print(self):
        print(self.__repr__)
        print(f"Opcode: {self.opcode}, Allowed addressing modes operand 1: {self.srcMode}, Allowed addressing modes operand 2: {self.destMode}, Operand 1: {self.srcOperand}, Operand 2: {self.destOperand}")

    # Get the allowed addressing modes for the instruction
    @staticmethod
    def get_allowed_addressing_modes(bitmask: int):
        logging.info(f"Allowed addressing modes: {bin(bitmask)[2:].zfill(8)}")
        if bitmask & AddressingMode.IMMEDIATE:
            logging.info("Immediate Value")
        if bitmask & AddressingMode.REGISTER:
            logging.info("Register Addressing Mode")
        if bitmask & AddressingMode.DIRECT:
            logging.info("Direct Memory Addressing Mode")
        if bitmask & AddressingMode.INDIRECT:
            logging.info("Indirect Memory Addressing Mode")

    # Generate a pretty-printed string representation of the instruction
    def prettyprint(self):
        buffer = ""
        for opcode in Opcode.__dict__.keys():
            if Opcode.__dict__[opcode] == self.opcode:
                buffer += f"Opcode: {opcode}\n"
                break
        for addressing_mode in AddressingMode.__dict__.keys():
            if AddressingMode.__dict__[addressing_mode] == self.srcMode:
                buffer += f"Addressing Mode 1: {addressing_mode}\n"
                break
        for addressing_mode in AddressingMode.__dict__.keys():
            if AddressingMode.__dict__[addressing_mode] == self.destMode:
                buffer += f"Addressing Mode 2: {addressing_mode}\n"
                break
        buffer += f"Operand 1: {self.srcOperand}\n"
        buffer += f"Operand 2: {self.destOperand}\n"
        return buffer
    
    # Encode the instruction into a bytearray
    @staticmethod
    def encode(opcode, srcMode, destMode, srcOperand, destOperand):
        ba = bytearray()
        ba.extend(int(opcode).to_bytes(1, byteorder="little"))
        ba.extend(int(srcMode).to_bytes(1, byteorder="little"))
        ba.extend(int(destMode).to_bytes(1, byteorder="little"))
        ba.extend(int(srcOperand).to_bytes(8, byteorder="little"))
        ba.extend(int(destOperand).to_bytes(8, byteorder="little"))
        return ba

    # Decode a bytearray into an Instruction object
    @staticmethod
    def decode(ba: bytearray):
        opcode = int.from_bytes(ba[0:1], byteorder="little")
        srcMode = int.from_bytes(ba[1:2], byteorder="little")
        destMode = int.from_bytes(ba[2:3], byteorder="little")
        srcOperand = int.from_bytes(ba[3:11], byteorder="little")
        destOperand = int.from_bytes(ba[11:19], byteorder="little")
        return Instruction(opcode, srcMode, destMode, srcOperand, destOperand)

    # Validate the values of the instruction's fields
    @staticmethod
    def validate(opcode, srcMode, destMode, srcOperand, destOperand):      
        if not (0 <= opcode <= 0xFF):
            raise ValueError(f"Opcode {opcode} is out of uint8 range.")
       
        if not (0 <= srcMode <= 0xFF):
            raise ValueError(f"Source mode {srcMode} is out of uint8 range.")
        
        if not (0 <= destMode <= 0xFF):
            raise ValueError(f"Destination mode {destMode} is out of uint8 range.")
        
        if not (0 <= srcOperand <= 0xFFFFFFFFFFFFFFFF):
            raise ValueError(f"Source operand {srcOperand} is out of uint64 range.")

        if not (0 <= destOperand <= 0xFFFFFFFFFFFFFFFF):
            raise ValueError(f"Destination operand {destOperand} is out of uint64 range.")
        
        # Ensure opcode is defined within Opcode
        if opcode not in Opcode.__annotations__.values():
            raise ValueError(f"Invalid Opcode: {opcode}")
        
        # Ensure source mode is defined within AddressingMode
        if srcMode not in AddressingMode.__annotations__.values():
            raise ValueError(f"Invalid Source Mode: {srcMode}")
        
        # Ensure destination mode is defined within AddressingMode
        if destMode not in AddressingMode.__annotations__.values():
            raise ValueError(f"Invalid Destination Mode: {destMode}")        

        logging.info(f"Instruction validated: Opcode {opcode}, Source Mode {srcMode}, Destination Mode {destMode}, Source Operand {srcOperand}, Destination Operand {destOperand}")

# Define the instruction set
InstructionSet = {
    "NOP": Instruction(Opcode.NOP, AddressingMode.NONE, AddressingMode.NONE, Operand.NONE, Operand.NONE),
    "MOV": Instruction(Opcode.MOV, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, AddressingMode.REGISTER, Operand, Operand),
    "PUSH": Instruction(Opcode.PUSH, AddressingMode.NONE, AddressingMode.REGISTER, Operand.NONE, Operand),
    "POP": Instruction(Opcode.POP, AddressingMode.NONE, AddressingMode.REGISTER, Operand.NONE, Operand),
    "ADD": Instruction(Opcode.ADD, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, AddressingMode.REGISTER, Operand, Operand),
    "SUB": Instruction(Opcode.SUB, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, AddressingMode.REGISTER, Operand, Operand),
    "MUL": Instruction(Opcode.MUL, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, AddressingMode.REGISTER, Operand, Operand),
    "DIV": Instruction(Opcode.DIV, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, AddressingMode.REGISTER, Operand, Operand),
    "JMP": Instruction(Opcode.JMP, AddressingMode.NONE, AddressingMode.IMMEDIATE, Operand.NONE, Operand),
    "CMP": Instruction(Opcode.CMP, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, AddressingMode.IMMEDIATE | AddressingMode.REGISTER, Operand, Operand),
    "JEQ": Instruction(Opcode.JEQ, AddressingMode.NONE, AddressingMode.IMMEDIATE, Operand.NONE, Operand),
    "CALL": Instruction(Opcode.CALL, AddressingMode.NONE, AddressingMode.IMMEDIATE, Operand.NONE, Operand),
    "RET": Instruction(Opcode.RET, AddressingMode.NONE, AddressingMode.NONE, Operand.NONE, Operand),
    "RST": Instruction(Opcode.RST, AddressingMode.NONE, AddressingMode.NONE, Operand.NONE, Operand),
    "HLT": Instruction(Opcode.HLT, AddressingMode.NONE, AddressingMode.NONE, Operand.NONE, Operand),
    "LDR": Instruction(Opcode.LDR, AddressingMode.DIRECT, AddressingMode.REGISTER, Operand, Operand),
    "STR": Instruction(Opcode.STR, AddressingMode.REGISTER, AddressingMode.DIRECT, Operand, Operand),
}