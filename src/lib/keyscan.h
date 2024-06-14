#ifndef __KEYSCAN_H_
#define __KEYSCAN_H_

#include "bsp.h"

#define ROW_NUM 5
#define COL_NUM 3

// GPIO定义 高电平触发
// PA 0-3为行，PB 3-5为列，OE 为PB10，当做额外行
#define ROW1 GPIO_Pin_0
#define ROW2 GPIO_Pin_1
#define ROW3 GPIO_Pin_2
#define ROW4 GPIO_Pin_3

#define OE   GPIO_Pin_10

#define COL1 GPIO_Pin_3
#define COL2 GPIO_Pin_4
#define COL3 GPIO_Pin_5

// Coin PA4 低电平触发，当做按键 Row x Col，当前是15
#define COIN      GPIO_Pin_4

#define KEY_COUNT (ROW_NUM * COL_NUM + 1)

void KeyScan_Init();
void KeyScan_Scan();
uint8_t KeyScan_GetKeyDebouncedStatus(uint8_t keyIndex);
uint16_t KeyScan_GetAllKeyDebouncedStatus();

#endif // !__KEYSCAN_H_