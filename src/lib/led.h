#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

typedef enum {
  LED_RGB_PORT_LEFT  = 0x01,
  LED_RGB_PORT_RIGHT = 0x00,
  LED_RGB_PORT_UART  = 0x02,
} __packed LED_RGB_Port;

typedef enum {
  LED_7C_L1 = 0x00,
  LED_7C_L2 = 0x02,
  LED_7C_L3 = 0x04,
  LED_7C_R1 = 0x01,
  LED_7C_R2 = 0x03,
  LED_7C_R3 = 0x05,
} __packed LED_7C_Tag;

typedef enum {
  LED_OFF = 0x00,
  LED_ON  = 0x01,
} __packed LED_State;

// 初始化RGB灯光
void LED_Init(void);

// 设置单个LED的RGB颜色
void LED_RGB_Set(LED_RGB_Port port, uint8_t index, uint8_t r, uint8_t g, uint8_t b);

// 设置所有LED的RGB颜色
void LED_RGB_SetAll(uint8_t r, uint8_t g, uint8_t b);

// 设置某个端口的所有LED的RGB颜色
void LED_RGB_SetPort(LED_RGB_Port port, uint8_t r, uint8_t g, uint8_t b);

// 显示RGB颜色
void LED_Refresh(void);

void LED_7C_Set(LED_7C_Tag index, LED_State r, LED_State g, LED_State b);

#ifdef __cplusplus
}
#endif

#endif // !__LED_H