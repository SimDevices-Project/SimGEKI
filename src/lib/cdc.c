#include "cdc.h"
#include "comio.h"

#include "usb_pwr.h"
#include "usb_lib.h"
#include "usb_prop.h"

#include "led.h"

#include "string.h"

// 初始化波特率为115200，1停止位，无校验，8数据位。
uint8_t LineCoding[LINECODING_SIZE] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08};

uint8_t ledIO_PutCharBuf[CDC_PUTCHARBUF_LEN];
uint8_t cardIO_PutCharBuf[CDC_PUTCHARBUF_LEN];

uint8_t ledIO_Rx_PendingBuf[64];
uint8_t cardIO_Rx_PendingBuf[64];

uint8_t ledIO_PacketBuf[256];
uint8_t cardIO_PacketBuf[64];

uint8_t CDC_ResponseStringBuf[64];

CDC_Struct cdc_led_io;
CDC_Struct cdc_card_io;

void CDC_Init(){
  cdc_led_io.PutCharBuff = ledIO_PutCharBuf;
  cdc_led_io.PutCharBuff_Last = 0;
  cdc_led_io.PutCharBuff_First = 0;
  cdc_led_io.Tx_Busy = 0;
  cdc_led_io.Tx_Full = 0;
  cdc_led_io.Rx_Pending = 0;
  cdc_led_io.Rx_PendingBuf = ledIO_Rx_PendingBuf;
  cdc_led_io.Rx_CurPos = 0;
  cdc_led_io.Req_PacketPos = 0;
  cdc_led_io.Req_PacketBuf = ledIO_PacketBuf;

  cdc_card_io.PutCharBuff = cardIO_PutCharBuf;
  cdc_card_io.PutCharBuff_Last = 0;
  cdc_card_io.PutCharBuff_First = 0;
  cdc_card_io.Tx_Busy = 0;
  cdc_card_io.Tx_Full = 0;
  cdc_card_io.Rx_Pending = 0;
  cdc_card_io.Rx_PendingBuf = cardIO_Rx_PendingBuf;
  cdc_card_io.Rx_CurPos = 0;
  cdc_card_io.Req_PacketPos = 0;
  cdc_card_io.Req_PacketBuf = cardIO_PacketBuf;
}

void CDC_LED_IO_Upload(uint8_t length)
{
  USBD_ENDPx_DataUp(CDC_LED_IO_EP, CDC_ResponseStringBuf, length);
}

void CDC_USB_Poll()
{
  uint8_t usb_tx_len;
  if (bDeviceState == CONFIGURED) {
    if (!cdc_led_io.Tx_Busy) {
      if (cdc_led_io.PutCharBuff_First == cdc_led_io.PutCharBuff_Last) {
        if (cdc_led_io.Tx_Full) {
          // Buffer is full

          cdc_led_io.Tx_Busy = 1;

          // length (the first byte to send, the end of the buffer)
          usb_tx_len = CDC_PUTCHARBUF_LEN - cdc_led_io.PutCharBuff_First;
          memcpy(CDC_ResponseStringBuf, &cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_First], usb_tx_len);

          // length (the first byte in the buffer, the last byte to send), if any
          if (cdc_led_io.PutCharBuff_Last > 0)
            memcpy(&CDC_ResponseStringBuf[usb_tx_len], cdc_led_io.PutCharBuff, cdc_led_io.PutCharBuff_Last);

          // Send the entire buffer
          CDC_LED_IO_Upload(CDC_PUTCHARBUF_LEN);

          // A 64-byte packet is going to be sent, according to USB specification, USB uses a less-than-max-length packet to demarcate an end-of-transfer
          // As a result, we need to send a zero-length-packet.
          // return;
        }

        // Otherwise buffer is empty, nothing to send
        // return;
      } else {
        cdc_led_io.Tx_Busy = 1;

        // CDC1_PutChar() is the only way to insert into CDC1_PutCharBuf, it detects buffer overflow and notify the CDC_USB_Poll().
        // So in this condition the buffer can not be full, so we don't have to send a zero-length-packet after this.

        if (cdc_led_io.PutCharBuff_First > cdc_led_io.PutCharBuff_Last) {
          // Rollback
          // length (the first byte to send, the end of the buffer)
          usb_tx_len = CDC_PUTCHARBUF_LEN - cdc_led_io.PutCharBuff_First;
          memcpy(CDC_ResponseStringBuf, &cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_First], usb_tx_len);

          // length (the first byte in the buffer, the last byte to send), if any
          if (cdc_led_io.PutCharBuff_Last > 0) {
            memcpy(&CDC_ResponseStringBuf[usb_tx_len], cdc_led_io.PutCharBuff, cdc_led_io.PutCharBuff_Last);
            usb_tx_len += cdc_led_io.PutCharBuff_Last;
          }

        } else {
          usb_tx_len = cdc_led_io.PutCharBuff_Last - cdc_led_io.PutCharBuff_First;
          memcpy(CDC_ResponseStringBuf, &cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_First], usb_tx_len);
        }

        cdc_led_io.PutCharBuff_First += usb_tx_len;
        if (cdc_led_io.PutCharBuff_First >= CDC_PUTCHARBUF_LEN)
          cdc_led_io.PutCharBuff_First -= CDC_PUTCHARBUF_LEN;

        // ACK next IN transfer
        CDC_LED_IO_Upload(usb_tx_len);
      }
    }
  }
}

void CDC_LED_IO_PutChar(uint8_t tdata)
{
  // Add new data to CDC1_PutCharBuf
  cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_Last++] = tdata;
  if (cdc_led_io.PutCharBuff_Last >= CDC_PUTCHARBUF_LEN) {
    // Rotate the tail to the beginning of the buffer
    cdc_led_io.PutCharBuff_Last = 0;
  }

  if (cdc_led_io.PutCharBuff_Last == cdc_led_io.PutCharBuff_First) {
    // Buffer is full
    cdc_led_io.Tx_Full = 1;

    while (cdc_led_io.Tx_Full) // Wait until the buffer has vacancy
      CDC_USB_Poll();
  }
}

void LED_IO_Handler()
{
  uint8_t checksum, i; // Response flag, also use for checksum & i
  IO_Packet *reqPacket  = (IO_Packet *)cdc_led_io.Req_PacketBuf;
  IO_Packet *resPackect = (IO_Packet *)CDC_ResponseStringBuf;

  memset(CDC_ResponseStringBuf, 0x00, 64); // Clear resPackect

  resPackect->sync      = 0xE0;
  resPackect->srcNodeId = reqPacket->dstNodeId;
  resPackect->dstNodeId = reqPacket->srcNodeId;

  resPackect->response.status  = ACK_OK;
  resPackect->response.report  = REPORT_OK;
  resPackect->response.command = reqPacket->request.command;

  switch (reqPacket->request.command) {
    case CMD_RESET:
      LED_RGB_Set(RGB_PORT_LEFT, 0, 0, 0, 0);
      LED_RGB_Set(RGB_PORT_RIGHT, 0, 0, 0, 0);
      resPackect->length = 0;
      break;
    case CMD_SET_TIMEOUT:
      resPackect->response.data[0] = reqPacket->request.data[0];
      resPackect->response.data[1] = reqPacket->request.data[1];
      resPackect->length           = 2;
      break;
    case CMD_SET_DISABLE:
      resPackect->response.data[0] = reqPacket->request.data[0];
      resPackect->length           = 1;
      break;
    case CMD_EXT_BOARD_SET_LED_RGB_DIRECT:
      LED_RGB_Set(RGB_PORT_RIGHT, 0, reqPacket->response.data[180], reqPacket->response.data[181], reqPacket->response.data[182]); // Right
      LED_RGB_Set(RGB_PORT_LEFT, 0, reqPacket->response.data[3], reqPacket->response.data[4], reqPacket->response.data[5]);        // Left
      return;
      break;
    case CMD_EXT_BOARD_INFO:
      memcpy(resPackect->response.data, "15093-06", 8);
      resPackect->response.data[8] = 0x0A;
      memcpy(resPackect->response.data + 9, "6710A", 5);
      resPackect->response.data[14] = 0xFF;
      resPackect->response.data[15] = 0xA0; // revision
      resPackect->length            = 0x10;
      break;
    case CMD_EXT_BOARD_STATUS:
      resPackect->response.data[0] = 0x00; // boardFlag
      resPackect->response.data[1] = 0x00; // uartFlag
      resPackect->response.data[2] = 0x00; // cmdFlag
      resPackect->length           = 0x03;
      break;
    case CMD_EXT_FIRM_SUM:
      resPackect->response.data[0] = 0xAA;
      resPackect->response.data[1] = 0x53;
      resPackect->length           = 0x02;
      break;
    case CMD_EXT_PROTOCOL_VERSION:
      resPackect->response.data[0] = 0x01;
      resPackect->response.data[1] = 0x01; // major
      resPackect->response.data[2] = 0x00; // minor
      resPackect->length           = 0x03;
      break;
    default:
      resPackect->response.status = ACK_INVALID;
      resPackect->length          = 0x00;
      break;
  }

  checksum = 0;

  resPackect->length += 3;

  checksum += resPackect->dstNodeId;
  checksum += resPackect->srcNodeId;
  checksum += resPackect->length;
  checksum += resPackect->response.status;
  checksum += resPackect->response.command;
  checksum += resPackect->response.report;

  for (i = 0; i < resPackect->length - 3; i++) {
    checksum += resPackect->response.data[i];
  }
  resPackect->response.data[resPackect->length - 3] = checksum;

  CDC_LED_IO_PutChar(resPackect->sync);
  for (i = 1; i < resPackect->length + 5; i++) {
    if (resPackect->buffer[i] == 0xE0 || resPackect->buffer[i] == 0xD0) {
      CDC_LED_IO_PutChar(0xD0);
      CDC_LED_IO_PutChar(resPackect->buffer[i] - 1);
    } else {
      CDC_LED_IO_PutChar(resPackect->buffer[i]);
    }
  }
}

void CDC_UART_Poll()
{
  uint8_t cur_byte;
  static uint8_t checksum         = 0;
  static uint8_t led_io_prev_byte = 0, card_io_prev_byte = 0;
  IO_Packet *led_io_packect    = (IO_Packet *)ledIO_PacketBuf;
  AIME_Request *card_io_packet = (AIME_Request *)cardIO_PacketBuf;

  while (cdc_led_io.Rx_Pending) {
    cur_byte = cdc_led_io.Rx_PendingBuf[cdc_led_io.Rx_CurPos];
    if (cur_byte == 0xE0 && led_io_prev_byte != 0xD0) {
      checksum                 = 0x20;
      cdc_led_io.Req_PacketPos = 0;
      led_io_packect->length   = 0xFF;
    } else if (led_io_prev_byte == 0xD0) {
      cur_byte++;
    } else if (cur_byte == 0xD0) {
      cdc_led_io.Rx_Pending--;
      cdc_led_io.Rx_CurPos++;
      if (cdc_led_io.Rx_Pending == 0) {
        SetEPRxValid(CDC_LED_IO_EP);
      }
      continue;
    }

    cdc_led_io.Req_PacketBuf[cdc_led_io.Req_PacketPos] = cur_byte;
    cdc_led_io.Req_PacketPos++;
    if (cdc_led_io.Req_PacketPos > 5 && cdc_led_io.Req_PacketPos - 5 == led_io_packect->length) {
      if (checksum == cur_byte) {
        LED_IO_Handler();
      } else {
        // checksum error
        cdc_led_io.Req_PacketPos = 0;
        checksum                 = 0;
      }
    }

    checksum += cur_byte;

    cdc_led_io.Rx_Pending--;
    cdc_led_io.Rx_CurPos++;
    if (cdc_led_io.Rx_Pending == 0) {
      SetEPRxValid(CDC_LED_IO_EP);
    }
    led_io_prev_byte = cur_byte;
  }
}

void CDC_Poll()
{
  CDC_UART_Poll();
  CDC_USB_Poll();
}