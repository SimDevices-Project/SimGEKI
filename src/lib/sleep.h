#ifndef __SLEEP_H_
#define __SLEEP_H_

#include "bsp.h"

xdata void Sleep_Init();
void Sleep_Alive();
uint8_t Sleep_RegisterSleep(void (*callback)(void));
void Sleep_UnregisterSleep(uint8_t index);
uint8_t Sleep_RegisterWakeup(void (*callback)(void));
void Sleep_UnregisterWakeup(uint8_t index);

#endif // !__SLEEP_H_