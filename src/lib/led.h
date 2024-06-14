#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

// 初始化RGB灯光
void initRgbColor(void);

// 设置单个LED的RGB颜色
void setRgbColor(uint8_t port, uint8_t index, uint8_t r, uint8_t g, uint8_t b);

// 设置所有LED的RGB颜色
void setRgbColorAll(uint8_t r, uint8_t g, uint8_t b);

// 设置某个端口的所有LED的RGB颜色
void setRgbColorPort(uint8_t port, uint8_t r, uint8_t g, uint8_t b);

// 显示RGB颜色
void showRgbColor(void);

#ifdef __cplusplus
}
#endif

#endif // !__LED_H