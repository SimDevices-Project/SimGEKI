#ifndef __PN532_UART_H
#define __PN532_UART_H

#include "bsp.h"

#define PN532_UART_TIMEOUT 1000

/**
 * @brief 是否启用串口直通模式
 * 0 - 禁用
 * 1 - 模式1
 * 2 - 模式2
 */
#define PN532_UART_DIRECT 0

/**
 * @brief 是否跳过FELICA读写
 * 0 - 否
 * 1 - 是
 */
#define PN532_DISABLE_FELICA_CHECK 1

void PN532_UART_Check(uint8_t *buffer, uint8_t *size);

typedef struct
{
  void (*begin)(void);
  void (*wakeup)(void);
  void (*writeCommand)(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
  void (*getResponse)(uint8_t *buf, uint8_t *len);
} __packed PN532_Interface;

extern PN532_Interface PN532_UART;

#define PN532_INTERFACE_UART

#endif // !__PN532_UART_H