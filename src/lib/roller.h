#ifndef __ROLLER_H_
#define __ROLLER_H_

#include "bsp.h"

void Roller_Update();
void Roller_Init();

uint16_t Roller_GetValue();
uint16_t Roller_GetRawValue();

#endif // !__ROLLER_H_
