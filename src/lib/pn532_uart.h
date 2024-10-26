#ifndef __PN532_UART_H
#define __PN532_UART_H

#include "bsp.h"

#define PN532_UART_TIMEOUT 1000
#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK

#define PN532_INVALID_ACK             (-1)
#define PN532_TIMEOUT                 (-2)
#define PN532_INVALID_FRAME           (-3)
#define PN532_NO_SPACE                (-4)

#define REVERSE_BITS_ORDER(b)         b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; \
                                      b = (b & 0xCC) >> 2 | (b & 0x33) << 2; \
                                      b = (b & 0xAA) >> 1 | (b & 0x55) << 1
// 是否启用 UART 直通模式
#define PN532_UART_DIRECT 0

void PN532_UART_Check();

void PN532_UART_Init();

#include "pn532.h"

extern PN532_Interface PN532_UART;

typedef struct
{
  void (*begin)(void);
  void (*wakeup)(void);
  void (*writeCommand)(void (*callback)(uint8_t), const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
  void (*readResponse)(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout);
} __packed PN532_Interface;

#define PN532_INTERFACE_UART

#endif // !__PN532_UART_H