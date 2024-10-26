#include "pn532_uart.h"
#include "timeout.h"

#include <string.h>

#include "cdc.h"

#include "led.h"

#define _SendByte(dat)           \
  (USART_SendData(USART1, dat)); \
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)

uint8_t command;

#define RX_BUFFER_SIZE 128

uint8_t RxBuffer[RX_BUFFER_SIZE];
uint8_t RxIndex = 0;

uint8_t RxBufferSize = 0;

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void PN532_UART_WriteCommand(void (*callback)(uint8_t), const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
void PN532_UART_ReadResponse(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout);
void PN532_UART_Receive(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout);
void PN532_UART_ReadAckFrame(void (*callback)(uint8_t), uint16_t timeout);
void PN532_UART_Init();
void PN532_UART_Wakeup();

PN532_Interface PN532_UART = {
    .begin        = PN532_UART_Init,
    .wakeup       = PN532_UART_Wakeup,
    .writeCommand = PN532_UART_WriteCommand,
    .readResponse = PN532_UART_ReadResponse,
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
    RxBuffer[RxIndex++] = USART_ReceiveData(USART1);
#endif
  }
  if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
    RxBufferSize = RxIndex;
    RxIndex      = 0;
  }
}

void PN532_UART_Check()
{
  if (RxBufferSize > 0) {
    // Do something
    // RxBuffer 为接收到的数据
    // RxBufferSize 为接收到的数据长度
  }
}

void PN532_UART_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure   = {0};
  USART_InitTypeDef USART_InitStructure = {0};
  NVIC_InitTypeDef NVIC_InitStructure   = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* USART1 TX-->A.9   RX-->A.10 */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate            = 115200;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(USART1, ENABLE);
}

void PN532_UART_Wakeup()
{
  _SendByte(0x55);
  _SendByte(0x55);
  _SendByte(0);
  _SendByte(0);
  _SendByte(0);
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

/**
    @brief receive data .
    @param buf --> return value buffer.
    @param len --> length expect to receive.
    @param timeout --> time of reveiving
*/
void PN532_UART_Receive(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout)
{
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  RxIndex               = 0;
  __receive_handler_buf = buf;
  __receive_handler_len = len;
  clearTimeout(__receive_handler_timeout_id);
  clearInterval(__receive_handler_interval_id);
  __receive_handler_timeout_id  = setTimeout(__PN532_UART_Receive_Timeout, timeout);
  __receive_handler_interval_id = setInterval(__PN532_UART_Receive_Handler, 1);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
