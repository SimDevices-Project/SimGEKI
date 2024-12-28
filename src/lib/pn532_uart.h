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

void PN532_UART_Check(uint8_t *buffer,uint8_t *size);
void PN532_UART_Init();
void PN532_UART_WriteCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);

#include "pn532.h"

typedef struct
{
  void (*begin)(void);
  void (*wakeup)(void);
  void (*writeCommand)(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
  void (*readResponse)(uint8_t *buf, uint8_t len, uint16_t timeout);
} __packed PN532_Interface;

extern PN532_Interface PN532_UART;

#define PN532_INTERFACE_UART

#endif // !__PN532_UART_H