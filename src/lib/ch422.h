#ifndef __CH422_H_
#define __CH422_H_

#include "bsp.h"

// extern uint32_t ledData;

// void ch422Write(uint8_t byte1, uint8_t byte2);
void CH422_Init();
void CH422_Check();
void CH422_Set(uint32_t dat);

void CH422_Refresh();

#endif // __CH422_H_