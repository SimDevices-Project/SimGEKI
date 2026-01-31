#ifndef __TIMEOUT_H_
#define __TIMEOUT_H_

#include "bsp.h"

void Timeout_Init();

uint8_t setInterval(void (*callback)(void), uint32_t period);
void clearInterval(uint8_t id);

uint8_t setTimeout(void (*callback)(void), uint32_t period);
void clearTimeout(uint8_t id);

void resetInterval(uint8_t id);
void resetTimeout(uint8_t id);

void Timer_Process();

#endif // __TIMEOUT_H_