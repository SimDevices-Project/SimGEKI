#ifndef __CDC_H_
#define __CDC_H_

#include "bsp.h"

#define LINECODING_SIZE    7

#define CDC_USB_BUFF_SIZE 64

#define CDC_LED_IO_EP  ENDP2
#define CDC_CARD_IO_EP ENDP3

extern uint8_t LineCoding[LINECODING_SIZE];

typedef struct {
  uint8_t *PutCharBuff;
  uint8_t PutCharBuff_Last;
  uint8_t PutCharBuff_First;
  uint8_t PutCharBuff_Len;
  uint8_t USB_EndPoint;
  uint8_t Tx_Busy;
  uint8_t Tx_Full;
  uint8_t Rx_Pending;
  uint8_t *Rx_PendingBuf;
  uint8_t Rx_CurPos;
  uint8_t Req_PacketPos;
  uint8_t *Req_PacketBuf;
} __packed CDC_Struct;

extern CDC_Struct cdc_led_io;
extern CDC_Struct cdc_card_io;

void CDC_Init();
void CDC_Check();

void CDC_LED_IO_PutChar(uint8_t tdata);
void CDC_CARD_IO_PutChar(uint8_t tdata);

void CDC_CARD_IO_SendDataReady();

#endif // __CDC_H_