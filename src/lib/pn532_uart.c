#include "pn532.h"
#include "pn532_uart.h"
#include "timeout.h"

#include <string.h>

#include "cdc.h"

#include "led.h"

#define _SendByte(dat)           \
  (USART_SendData(USART1, dat)); \
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)

uint8_t command;

#define RX_BUFFER_SIZE  128
#define RX_BUFFER_COUNT 4

#define UARTOE              GPIO_Pin_8

uint8_t RxBuffer[RX_BUFFER_COUNT][RX_BUFFER_SIZE];
volatile uint8_t RxIndex[RX_BUFFER_COUNT] = {0};

volatile uint8_t RxWriteIndex = 0;
volatile uint8_t RxReadIndex  = 0;

volatile uint8_t RxBufferCount = 0;
volatile uint8_t RxDropCurrentFrame = 0;

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void PN532_UART_WriteCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
void PN532_UART_GetResponse(uint8_t *buf, uint8_t *len);
void PN532_UART_Init();
void PN532_UART_Wakeup();

PN532_Interface PN532_UART = {
    .begin        = PN532_UART_Init,
    .wakeup       = PN532_UART_Wakeup,
    .writeCommand = PN532_UART_WriteCommand,
    .getResponse  = PN532_UART_GetResponse,
};

/*********************************************************************
 * @fn      USART1_IRQHandler
 *
 * @brief   This function handles USART1 global interrupt request.
 *
 * @return  none
 */
void USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
#if PN532_UART_DIRECT == 1
    CDC_CARD_IO_PutChar(USART_ReceiveData(USART1));
    return;
#else
    uint8_t data = USART_ReceiveData(USART1);
    if (RxDropCurrentFrame == 0) {
      uint8_t index = RxIndex[RxWriteIndex];
      if (index < RX_BUFFER_SIZE) {
        RxBuffer[RxWriteIndex][index] = data;
        RxIndex[RxWriteIndex]         = index + 1;
      } else {
        // Current frame is longer than one software buffer, drop it safely.
        RxDropCurrentFrame = 1;
      }
    }
#endif
  }
#if PN532_UART_DIRECT != 1
  if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
    (void)USART1->STATR; // 读取 STATR 寄存器以清除空闲中断标志
    (void)USART1->DATAR; // 读取 DATAR 寄存器以清除空闲中断标志
    // USART_ClearFlag(USART1, USART_FLAG_IDLE); // 不需要这么清除
    if ((RxDropCurrentFrame == 0) && (RxIndex[RxWriteIndex] > 0)) {
      if (RxBufferCount < (RX_BUFFER_COUNT - 1)) {
        RxBufferCount++;
      } else {
        // Queue full: drop the oldest completed frame to keep latest frames.
        RxReadIndex++;
        if (RxReadIndex >= RX_BUFFER_COUNT) {
          RxReadIndex = 0;
        }
      }

      RxWriteIndex++;
      if (RxWriteIndex >= RX_BUFFER_COUNT) {
        RxWriteIndex = 0;
      }
    }
    RxIndex[RxWriteIndex]  = 0;
    RxDropCurrentFrame     = 0;
    // PN532_UART_RxDataCheck();
  }
#endif
}

uint8_t __GetNextRxBuffer(uint8_t **buf, uint8_t *len)
{
  uint8_t next;
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
#if PN532_UART_DIRECT != 1
  USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
#endif
  if (RxBufferCount == 0) {
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#if PN532_UART_DIRECT != 1
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#endif
    return 0;
  }

  next = RxReadIndex;
  *buf = RxBuffer[next];
  *len = RxIndex[next];
  RxReadIndex++;
  if (RxReadIndex >= RX_BUFFER_COUNT) {
    RxReadIndex = 0;
  }
  RxBufferCount--;
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#if PN532_UART_DIRECT != 1
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#endif
  return 1;
}

void PN532_UART_Check(uint8_t *_buffer, uint8_t *_size)
{
  uint8_t *buffer;
  uint8_t size;
  while (__GetNextRxBuffer(&buffer, &size)) {
#if PN532_UART_DIRECT == 2
    for (uint8_t i = 0; i < size; i++) {
      CDC_CARD_IO_PutChar(buffer[i]);
    }
#elif PN532_UART_DIRECT == 0
    memcpy(_buffer, buffer, size);
    *_size = size;
#endif
  }
}

void PN532_UART_GetResponse(uint8_t *buf, uint8_t *len)
{
  return PN532_UART_Check(buf, len);
}

void PN532_UART_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure   = {0};
  USART_InitTypeDef USART_InitStructure = {0};
  NVIC_InitTypeDef NVIC_InitStructure   = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* USART1 TX-->A.9   RX-->A.10  UARTOE-->A.8 */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = UARTOE;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate            = 115200;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#if PN532_UART_DIRECT != 1
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#endif
NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(USART1, ENABLE);
  GPIO_SetBits(GPIOA, UARTOE); // 使能UART电平转换芯片
}

void PN532_UART_Wakeup()
{
  const uint8_t startup532[] = {
      0x55, 0x55, 0x00, 0x00, 0x00, // 5 Bytes dummy bytes to make sure PN532 wakes up
      0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x14, 0x01, 0x17, 0x00};

  for (uint8_t i = 0; i < sizeof(startup532); i++) {
    _SendByte(startup532[i]);
  }
}

void PN532_UART_WriteCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
  // 00 - 00 FF - LEN - LCS - TFI -  PD0...PDn - DCS - 00
  command = header[0];

  _SendByte(PN532_PREAMBLE);
  _SendByte(PN532_STARTCODE1);
  _SendByte(PN532_STARTCODE2);

  uint8_t LEN = hlen + blen + 1; // length of data field: TFI + DATA
  uint8_t LCS = ~LEN + 1;        // checksum of length
  _SendByte(LEN);
  _SendByte(LCS); // checksum of length

  _SendByte(PN532_HOSTTOPN532);

  uint8_t sum = PN532_HOSTTOPN532;
  for (uint8_t i = 0; i < hlen; i++) {
    _SendByte(header[i]);
    sum += header[i];
  }

  for (uint8_t i = 0; i < blen; i++) {
    _SendByte(body[i]);
    sum += body[i];
  }

  uint8_t DCS = ~sum + 1; // checksum of data
  _SendByte(DCS);
  _SendByte(PN532_POSTAMBLE);
}
