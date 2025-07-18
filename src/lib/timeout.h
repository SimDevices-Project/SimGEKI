#ifndef __TIMEOUT_H_
#define __TIMEOUT_H_

#include "bsp.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 定时器系统说明：
 * 1. 定时器基于主循环轮询模式，Timer_Process()在main()的while循环中调用
 * 2. 定时器不要求严格准时，允许一定的时间误差
 * 3. 定时器回调函数可能在执行过程中被其他中断打断，这是正常行为
 * 4. 系统使用硬件定时器TIM4提供1ms时基，中断仅用于更新tick计数
 */

// 定时器函数
uint8_t setInterval(void (*callback)(void), uint16_t period);
uint8_t setTimeout(void (*callback)(void), uint16_t period);
void clearInterval(uint8_t id);
void clearTimeout(uint8_t id);
void Timer_Process(void);
void Timeout_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __TIMEOUT_H_