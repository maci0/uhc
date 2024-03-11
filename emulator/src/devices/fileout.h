

#ifndef FILEOUT_H
#define FILEOUT_H

#include <stdint.h>

void FO_Tick();
uint64_t FO_Read(uint64_t address);
void FO_Write(uint64_t address, uint64_t data);


#endif // FILEOUT_H