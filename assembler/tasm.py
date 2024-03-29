#!/usr/bin/env python3
import isa
import argparse

# tasm - the tiny assembler


def preprocess_lines(assembly_code):
    # Split the code into lines
    lines = assembly_code.strip().split("\n")

    # Ignore comments
    lines = [line.split(";")[0].strip() for line in lines]

    # Ignore empty lines
    lines = [line for line in lines if line.strip()]

    # Uppercase all lines
    lines = [line.upper() for line in lines]

    return lines


def parse_directive(line):
    parts = line.split()
    directive = parts[0]
    if directive == ".ORG":
        offset = isa.Operand.to_int(parts[1])
        return directive, offset
    elif directive == ".EQU":
        # Extract the constant name and value
        constant_name, constant_value = parts[1], parts[2]
        return directive, constant_name, constant_value
    # Add more elif conditions here for other directives


def parse_file(filename):
    offset = 0

    assembly_code = filename.read()
    # Split the code into lines
    lines = assembly_code.strip().split("\n")

    # ignore comments
    lines = [line.split(";")[0].strip() for line in lines]

    # ignore empty lines
    lines = [line for line in lines if line.strip()]

    # uppercase all lines
    lines = [line.upper() for line in lines]

    # Data structure to hold parsed instructions and labels
    instructions = []
    labels = {}
    constants = {}
    data = []

    # State variable to track whether we're parsing instructions or data
    parsing_data = False

    # fixed constants
    constants["SP"] = "R65"

    # Parse each line
    for i, line in enumerate(lines):
        if not line:
            return None

        # Assembler directives
        if line.startswith("."):
            parts = line.split()
            directive = parts[0]
            if directive == ".ORG":
                # print ("DIRECTIVE:", directive)
                offset = isa.Operand.to_int(parts[1])
                # print("OFFSET:", offset)
                continue
            elif directive == ".EQU":
                # Extract the constant name and value
                constant_name, constant_value = parts[1], parts[2]
                if constant_name in constant_value:
                    print("Error: constant already defined")
                    return None
                constants[constant_name] = constant_value
                print(
                    f"DIRECTIVE: {directive}, Name: {constant_name}, Value: {constant_value}"
                )
                continue
            elif directive == ".DATA":
                print("DATA PARSING")
                parsing_data = True

                continue
            if directive == ".END":
                # print ("Encountered directive .END, ending assembly")
                break

        # Check if the line is a label
        if line.endswith(":"):
            label_name = line[:-1]  # Remove the colon

            # Check if the label is already defined
            if label_name in labels:
                print("Error: Label already defined")
                return None

            labels[label_name] = len(instructions * isa.INSTRUCTION_WIDTH)
        else:
            # Split the line into components (instruction and operands)
            parts = line.split()
            instruction = parts[0]
            operands = parts[1:]
            instructions.append((instruction, operands, offset))

    # Now you have a list of instructions and a dictionary of labels with their associated instruction indices
    print("Instructions:", instructions)
    print("Labels:", labels)
    print("Constants:", constants)

    parsed_instructions = []

    for instr, ops, offset in instructions:
        mnemonic = instr
        operands = []

        # Check if any operand is a label
        for op in ops:
            if op in labels:
                operands.append(str(labels[op]))
            elif op.startswith("$") and op[1:] in labels:
                # This is a byte address label
                byte_address = labels[
                    op[1:]
                ]  # Look up the rest of the label in the labels dictionary
                operands.append("$" + str(byte_address))
            elif op in constants:
                operands.append(str(constants[op]))
            else:
                operands.append(op)

        # Print the instruction and operands
        print(
            f"Instruction: {mnemonic}, Operands: {', '.join(operands)}, Offset: {offset}"
        )
        parsed_instructions.append((mnemonic, operands, offset))

    # Find DB and store the value in memory
    # Find DW and store the value in memory
    # Find DS and reserve space in memory
    # Find INCLUDE and include the file
    # Find MACRO and define a macro
    # Find ENDM and end the macro
    # Find IF and check the condition

    return parsed_instructions


def assemble_instruction(asm):
    mnemonic, args, offset = asm
    if not hasattr(isa.Opcode, mnemonic):
        print("Error: Invalid mnemonic ")
        return

    # Default addressing modes and operands
    opcode: isa.Opcode = getattr(isa.Opcode, mnemonic)
    srcMode: isa.AddressingMode = isa.AddressingMode.NONE
    destMode: isa.AddressingMode = isa.AddressingMode.NONE
    srcOperand: isa.Operand = isa.Operand.NONE
    destOperand: isa.Operand = isa.Operand.NONE

    # If only one argument is provided, it is the destination operand
    if len(args) == 1:
        srcMode = isa.AddressingMode.NONE
        destMode = isa.Operand.infer_addressing_mode(args[0])
        srcOperand: isa.Operand = isa.Operand.NONE
        destOperand: isa.Operand = isa.Operand.to_int(args[0])

    if len(args) == 2:
        srcMode = isa.Operand.infer_addressing_mode(args[0])
        destMode = isa.Operand.infer_addressing_mode(args[1])
        srcOperand: isa.Operand = isa.Operand.to_int(args[0])
        destOperand: isa.Operand = isa.Operand.to_int(args[1])

    print("OFFSET:", offset)
    if srcMode == isa.AddressingMode.DIRECT:
        srcOperand += offset
    if destMode == isa.AddressingMode.DIRECT:
        destOperand += offset

    print("assemble:", opcode, srcMode, destMode, srcOperand, destOperand)

    return (opcode, srcMode, destMode, srcOperand, destOperand)


def main():
    # Read filename from cmd line argument

    input_file = open(args.input, "r")
    output_file = open(args.output, "wb")

    # Write initial NOP
    # output_file.write(isa.Instruction.encode(isa.Opcode.NOP, isa.AddressingMode.NONE, isa.AddressingMode.NONE, isa.Operand.NONE, isa.Operand.NONE))

    for instruction in parse_file(input_file):
        opcode, am1, am2, op1, op2 = assemble_instruction(instruction)
        output_file.write(isa.Instruction.encode(opcode, am1, am2, op1, op2))
        print(isa.Instruction.encode(opcode, am1, am2, op1, op2))

    # Write trailing HLT
    output_file.write(
        isa.Instruction.encode(
            isa.Opcode.HLT,
            isa.AddressingMode.NONE,
            isa.AddressingMode.NONE,
            isa.Operand.NONE,
            isa.Operand.NONE,
        )
    )

    # Close files
    input_file.close()
    output_file.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Assembler script to process assembly files."
    )
    parser.add_argument("-o", "--output", help="Output binary file", default="test.bin")
    parser.add_argument("input", help="Input assembly file", default="test.asm")
    args = parser.parse_args()
    main()
