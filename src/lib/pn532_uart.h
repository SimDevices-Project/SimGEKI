#ifndef __PN532_UART_H
#define __PN532_UART_H

#include "bsp.h"

#define PN532_UART_TIMEOUT 1000

// 是否启用 UART 直通模式
#define PN532_UART_DIRECT 0

void PN532_UART_Check();

void PN532_UART_Init();

#include "pn532.h"

extern PN532_Interface PN532_UART;

#define PN532_INTERFACE_UART

#endif // !__PN532_UART_H