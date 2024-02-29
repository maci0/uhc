


# Instruction Format

| Opcode (8 bits) | Addressing Mode 1 (8 bits) | Addressing Mode 2 (8 bits) | Operand 1 (64 bits) | Operand 2 (64 bits)   |
|-----------------|----------------------------|----------------------------|----------------------|----------------------|
| 1 byte          | 1 byte                     | 1 byte                     | 8 bytes              | 8 bytes              |
| 0x01            | 0x01                       | 0x01                       | 0x0000000000000000   | 0x0000000000000000   |


1. Opcode (8 bits)
 - Operation to be performed (8 bits)

# Addressing modes
0. Immediate Value, eg. '123' '0x123' '0b1010' - example: ADD '123' r1 - adds 123 to r1, overwrites r1, stores result in r1
1. Register Addressing Mode, eg. r1 r2 r3 - example: ADD r1 r2 - adds r1 to r2, overwrites r2, stores result in r2
2. Direct Memory Addressing Mode eg. 0x12345678 - address of memory location - example: ADD 0x12345678 r1 - adds value at memory location 0x12345678 to r1, overwrites r1, stores result in r1
3. Indirect Memory Addressing Mode eg. *r1 *r2 *r3 - address contained in register r1 r2 r3 - example: ADD *r1 r2 - adds value at memory location contained in r1 to r2, overwrites r2, stores result in r2

Addressing Operand 1 (4 bits)
Addressing Operand 2 (4 bits)


Operand 1 (up to 64 bits)
Operand 2 (up to 64 bits)

 2. Source Register or Immediate Value or Memory Address 
 3. Destination Register or Immediate Value or Memory Address

 Opcode (4 bits) # Addressing Mode (2 bits) # Addressing Mode (2 bits) # Operand 1 (64 bits) # Operand 2 (64 bits)
 copy 64bit immediate value to register r1
 0x01 0x00 0x01 0xFFFFFFFFFFFFF 0x1


 Class CPU
 1. Registers
 2. Memory
 3. Instructions
 4. Fetch-Decode-Execute Cycle
 5. Run method
 6. Load method
 7. Store method
 8. Fetch method
 9. Decode method
 10. Execute method


 Memory Map
 0x00000000 - 0x000000FF: Interrupt Vector Table
 0x00000100 - 0x000001FF: BIOS
 0x00000200 - 0x000002FF: Bootloader
 0x00000300 - 0x000003FF: Kernel
 0x00000400 - 0x0000FFFF: Reserved
 0x00010000 - 0x7FFFFFFF: User Space
 0x80000000 - 0xFFFFFFFF: Kernel Space
