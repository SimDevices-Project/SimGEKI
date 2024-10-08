#ifndef __ROLLER_H_
#define __ROLLER_H_

#include "bsp.h"

void Roller_Update();
void Roller_Init();

uint16_t Roller_GetValue();
uint16_t Roller_GetRawValue();

// uint16_t Roller_GetOffset();
// void Roller_SetOffset(uint16_t offset);

void Roller_ResetOffset();

#endif // !__ROLLER_H_
