#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

#define RGB_PORT_LEFT 1
#define RGB_PORT_RIGHT 0
#define RGB_PORT_UART 2

// 初始化RGB灯光
void LED_Init(void);

// 设置单个LED的RGB颜色
void LED_RGB_Set(uint8_t port, uint8_t index, uint8_t r, uint8_t g, uint8_t b);

// 设置所有LED的RGB颜色
void LED_RGB_SetAll(uint8_t r, uint8_t g, uint8_t b);

// 设置某个端口的所有LED的RGB颜色
void LED_RGB_SetPort(uint8_t port, uint8_t r, uint8_t g, uint8_t b);

// 显示RGB颜色
void LED_RGB_Refresh(void);

#ifdef __cplusplus
}
#endif

#endif // !__LED_H