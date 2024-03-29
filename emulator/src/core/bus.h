#ifndef BUS_H
#define BUS_H

#include <stdint.h>

// Memory Layout

//#define INITIAL_SP (uint64_t) 0
//#define RESET_VECTOR (uint64_t) 8
//#define INTERRUPT_VECTOR (uint64_t) 16


#define RAM_START 0x00000000
#define RAM_END 0x00FFFFFF // 16 MB RAM

#define ROM_START 0x01000000
#define ROM_END 0x010FFFFF // 1 MB ROM

#define MMIO_START 0x01100000
#define MMIO_END 0x0110FFFF // 64 KB MMIO

#define CONSOLE_START 0xC011501E
#define CONSOLE_CONTROL_REGISTER CONSOLE_START
#define CONSOLE_DATA_REGISTER (CONSOLE_CONTROL_REGISTER + 8)
#define CONSOLE_END CONSOLE_DATA_REGISTER

#define FILEOUT_START 0x01100000
#define FILEOUT_CONTROL_REGISTER FILEOUT_START
#define FILEOUT_DATA_REGISTER (FILEOUT_CONTROL_REGISTER + 8)
#define FILEOUT_END FILEOUT_DATA_REGISTER

#define PTY_START 0x01100010
#define PTY_END (PTY_START + 256)


uint64_t BUS_Read(uint64_t address);
uint64_t BUS_Write(uint64_t buf, uint64_t address);
uint64_t BUS_SendInterrupt(uint8_t interrupt);



#endif // BUS_H