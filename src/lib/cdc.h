#ifndef __CDC_H_
#define __CDC_H_

#include "bsp.h"

#define LINECODING_SIZE    7
#define CDC_PUTCHARBUF_LEN 64

#define CDC_LED_IO_EP ENDP2

extern uint8_t LineCoding[LINECODING_SIZE];

typedef struct {
  uint8_t PutCharBuff[CDC_PUTCHARBUF_LEN];
  uint8_t PutCharBuff_Last;
  uint8_t PutCharBuff_First;
  uint8_t Tx_Busy;
  uint8_t Tx_Full;
  uint8_t Rx_Pending;
  uint8_t Rx_PendingBuf[64];
  uint8_t Rx_CurPos;
  uint8_t Req_PacketPos;
  uint8_t *Req_PacketBuf;
} __packed CDC_Struct;

extern CDC_Struct cdc_led_io;
extern CDC_Struct cdc_card_io;


void CDC_Poll();

#endif // __CDC_H_