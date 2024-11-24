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

uint8_t RxBuffer[RX_BUFFER_COUNT][RX_BUFFER_SIZE];
uint8_t RxIndex[RX_BUFFER_COUNT] = {0};

uint8_t RxBufferIndex = 0;

uint8_t RxBufferCount = 0;

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void PN532_UART_WriteCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
void PN532_UART_ReadResponse(uint8_t *buf, uint8_t len, uint16_t timeout);
void PN532_UART_Receive(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout);
void PN532_UART_ReadAckFrame(void (*callback)(uint8_t), uint16_t timeout);
void PN532_UART_RxDataCheck();
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
    RxBuffer[RxBufferIndex][RxIndex[RxBufferIndex]++] = USART_ReceiveData(USART1);
#endif
  }
#if PN532_UART_DIRECT != 1
  if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
    (void)USART1->STATR; // 读取 STATR 寄存器以清除空闲中断标志
    (void)USART1->DATAR; // 读取 DATAR 寄存器以清除空闲中断标志
    // USART_ClearFlag(USART1, USART_FLAG_IDLE); // 不需要这么清除
    RxBufferIndex++;
    if (RxBufferIndex >= RX_BUFFER_COUNT) {
      RxBufferIndex = 0;
    }
    RxIndex[RxBufferIndex] = 0;
    RxBufferCount++;
    // PN532_UART_RxDataCheck();
  }
#endif
}

uint8_t __GetNextRxBuffer(uint8_t **buf, uint8_t *len)
{
  if (RxBufferCount == 0) {
    return 0;
  }
  uint8_t next;
//   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
// #if PN532_UART_DIRECT != 1
//   USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
// #endif
  if (RxBufferCount > RxBufferIndex) {
    next = (RX_BUFFER_COUNT - RxBufferCount) + RxBufferIndex;
  } else {
    next = RxBufferIndex - RxBufferCount;
  }
  *buf = RxBuffer[next];
  *len = RxIndex[next];
  RxBufferCount--;
//   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
// #if PN532_UART_DIRECT != 1
//   USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
// #endif
  return 1;
}

void PN532_UART_RxDataCheck()
{
  uint8_t *buffer;
  uint8_t size;
  while (__GetNextRxBuffer(&buffer, &size)) {
#if PN532_UART_DIRECT == 2
    for (uint8_t i = 0; i < size; i++) {
      CDC_CARD_IO_PutChar(buffer[i]);
    }
#elif PN532_UART_DIRECT == 0
    // Do something
    // buffer 为接收到的数据
    // size 为接收到的数据长度
#endif
  }
}

void PN532_UART_Check()
{
  PN532_UART_RxDataCheck();
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
#if PN532_UART_DIRECT != 1
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#endif
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

uint8_t __read_response_tmp_buf[64];
void (*__read_response_callback)(uint8_t);
uint8_t *__read_response_buf;
uint8_t __read_response_len;

void __PN532_UART_ReadResponse_Clear()
{
  __read_response_buf      = NULL;
  __read_response_len      = 0;
  __read_response_callback = NULL;
}

void __PN532_UART_ReadResponse_Handler(uint8_t len)
{
  if (len == __read_response_len) {
    if (memcmp("\x00,\x00,\xff", __read_response_buf, 3) != 0) {
      __read_response_callback(PN532_INVALID_FRAME);
      __PN532_UART_ReadResponse_Clear();
      return;
    }
    if (__read_response_buf[3] + __read_response_buf[4] != 0) {
      __read_response_callback(PN532_INVALID_FRAME);
      __PN532_UART_ReadResponse_Clear();
      return;
    }
    uint8_t cmd = command + 1;
    if (__read_response_buf[5] != PN532_PN532TOHOST || __read_response_buf[6] != cmd) {
      __read_response_callback(PN532_INVALID_FRAME);
      __PN532_UART_ReadResponse_Clear();
      return;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for (uint8_t i = 7; i < len - 2; i++) {
      sum += __read_response_buf[i];
    }
    if (__read_response_buf[len - 2] != ~sum + 1 || __read_response_buf[len - 1] != PN532_POSTAMBLE) {
      __read_response_callback(PN532_INVALID_FRAME);
      __PN532_UART_ReadResponse_Clear();
      return;
    }
    for (uint8_t i = 0; i < len - 7; i++) {
      __read_response_buf[i] = __read_response_tmp_buf[i + 6];
    }
    __read_response_callback(len - 7);
  }
  if (len > 0xF0) {
    __read_response_callback(len);
    __PN532_UART_ReadResponse_Clear();
    __PN532_UART_ReadResponse_Clear();
  }
  __read_response_callback(PN532_INVALID_FRAME);
  __PN532_UART_ReadResponse_Clear();
}

void PN532_UART_ReadResponse(uint8_t *buf, uint8_t len, uint16_t timeout)
{
  // __read_response_callback = callback;
  __read_response_buf = buf;
  __read_response_len = len;
  // PN532_UART_Receive(__PN532_UART_ReadResponse_Handler, __read_response_tmp_buf, len, timeout);
}

void (*__read_ack_frame_handler_callback)(uint8_t);
uint8_t __ack_frame_buf[6];

void __PN532_UART_ReadAckFrame_Handler(uint8_t len)
{
  const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
  if (len != 6) {
    __read_ack_frame_handler_callback(len);
    __read_ack_frame_handler_callback = NULL;
    return;
  }
  if (memcmp(PN532_ACK, __ack_frame_buf, 6) == 0) {
    __read_ack_frame_handler_callback(PN532_OK);
    __read_ack_frame_handler_callback = NULL;
  } else {
    __read_ack_frame_handler_callback(PN532_INVALID_ACK);
    __read_ack_frame_handler_callback = NULL;
  }
}

void PN532_UART_ReadAckFrame(void (*callback)(uint8_t), uint16_t timeout)
{
  __read_ack_frame_handler_callback = callback;
  // PN532_UART_Receive(__PN532_UART_ReadAckFrame_Handler, __ack_frame_buf, 6, timeout);
}

// uint8_t *__receive_handler_buf;
// uint8_t __receive_handler_len;
// void (*__receive_handler_callback)(uint8_t);
// uint8_t __receive_handler_timeout_id  = 0xFF;
// uint8_t __receive_handler_interval_id = 0xFF;

// void __PN532_UART_Receive_Timeout()
// {
//   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
//   clearTimeout(__receive_handler_timeout_id);
//   clearInterval(__receive_handler_interval_id);
//   __receive_handler_timeout_id  = 0xFF;
//   __receive_handler_interval_id = 0xFF;
//   __receive_handler_buf         = NULL;
//   __receive_handler_len         = 0;
//   __receive_handler_callback(PN532_TIMEOUT);
//   __receive_handler_callback = NULL;
// }

// void __PN532_UART_Receive_Handler()
// {
//   if (RxIndex >= __receive_handler_len) {
//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
//     clearTimeout(__receive_handler_timeout_id);
//     clearInterval(__receive_handler_interval_id);
//     __receive_handler_timeout_id  = 0xFF;
//     __receive_handler_interval_id = 0xFF;
//     for (uint8_t i = 0; i < __receive_handler_len; i++) {
//       __receive_handler_buf[i] = RxBuffer[i];
//     }
//     __receive_handler_callback(__receive_handler_len);
//     __receive_handler_buf      = NULL;
//     __receive_handler_len      = 0;
//     __receive_handler_callback = NULL;
//   }
// }

/**
    @brief receive data .
    @param buf --> return value buffer.
    @param len --> length expect to receive.
    @param timeout --> time of reveiving
*/
void PN532_UART_Receive(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout)
{
  // USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  // // RxIndex               = 0;
  // __receive_handler_buf = buf;
  // __receive_handler_len = len;
  // clearTimeout(__receive_handler_timeout_id);
  // clearInterval(__receive_handler_interval_id);
  // __receive_handler_timeout_id  = setTimeout(__PN532_UART_Receive_Timeout, timeout);
  // __receive_handler_interval_id = setInterval(__PN532_UART_Receive_Handler, 1);
  // USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
