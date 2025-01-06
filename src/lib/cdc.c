#include "cdc.h"
#include "comio.h"

#include "usb_pwr.h"
#include "usb_lib.h"
#include "usb_prop.h"

#include "led.h"

#include "string.h"

#include "pn532_uart.h"

#include <string.h>

// 初始化波特率为115200，1停止位，无校验，8数据位。
uint8_t LineCoding[LINECODING_SIZE] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08};

uint8_t ledIO_PutCharBuf[CDC_PUTCHARBUF_LEN];
uint8_t cardIO_PutCharBuf[CDC_PUTCHARBUF_LEN];

uint8_t ledIO_Rx_PendingBuf[CDC_PENDINGBUF_LEN];
uint8_t cardIO_Rx_PendingBuf[CDC_PENDINGBUF_LEN];

uint8_t usb_DataUpBuf[CDC_USB_BUFF_SIZE];

uint8_t ledIO_PacketBuf[256];
uint8_t cardIO_PacketBuf[64];

uint8_t ledIO_ResponseStringBuf[64];
uint8_t cardIO_ResponseStringBuf[128];

CDC_Struct cdc_led_io;
CDC_Struct cdc_card_io;

extern uint8_t key_A[6];
extern uint8_t key_B[6];

extern AIME_Request _req;
extern AIME_Response res;

uint8_t cardIO_SendDataReady_Flag = 0;

xdata void CDC_Init()
{
  cdc_led_io.PutCharBuff       = ledIO_PutCharBuf;
  cdc_led_io.PutCharBuff_Last  = 0;
  cdc_led_io.PutCharBuff_First = 0;
  cdc_led_io.Tx_Busy           = 0;
  cdc_led_io.Tx_Full           = 0;
  cdc_led_io.Rx_Pending        = 0;
  cdc_led_io.Rx_PendingBuf     = ledIO_Rx_PendingBuf;
  cdc_led_io.Rx_CurPos         = 0;
  cdc_led_io.Req_PacketPos     = 0;
  cdc_led_io.Req_PacketBuf     = ledIO_PacketBuf;

  cdc_card_io.PutCharBuff       = cardIO_PutCharBuf;
  cdc_card_io.PutCharBuff_Last  = 0;
  cdc_card_io.PutCharBuff_First = 0;
  cdc_card_io.Tx_Busy           = 0;
  cdc_card_io.Tx_Full           = 0;
  cdc_card_io.Rx_Pending        = 0;
  cdc_card_io.Rx_PendingBuf     = cardIO_Rx_PendingBuf;
  cdc_card_io.Rx_CurPos         = 0;
  cdc_card_io.Req_PacketPos     = 0;
  cdc_card_io.Req_PacketBuf     = cardIO_PacketBuf;
}

void CDC_LED_IO_Upload(uint8_t length)
{
  USBD_ENDPx_DataUp(CDC_LED_IO_EP, usb_DataUpBuf, length);
}

void CDC_CARD_IO_Upload(uint8_t length)
{
  USBD_ENDPx_DataUp(CDC_CARD_IO_EP, usb_DataUpBuf, length);
}

void CDC_LED_IO_USB_Poll()
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
          memcpy(usb_DataUpBuf, &cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_First], usb_tx_len);

          // length (the first byte in the buffer, the last byte to send), if any
          if (cdc_led_io.PutCharBuff_Last > 0)
            memcpy(&usb_DataUpBuf[usb_tx_len], cdc_led_io.PutCharBuff, cdc_led_io.PutCharBuff_Last);

          // Send the entire buffer
          CDC_LED_IO_Upload(CDC_PUTCHARBUF_LEN);

          cdc_led_io.Tx_Full = 0;

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
          memcpy(usb_DataUpBuf, &cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_First], usb_tx_len);

          // length (the first byte in the buffer, the last byte to send), if any
          if (cdc_led_io.PutCharBuff_Last > 0) {
            memcpy(&usb_DataUpBuf[usb_tx_len], cdc_led_io.PutCharBuff, cdc_led_io.PutCharBuff_Last);
            usb_tx_len += cdc_led_io.PutCharBuff_Last;
          }

        } else {
          usb_tx_len = cdc_led_io.PutCharBuff_Last - cdc_led_io.PutCharBuff_First;
          memcpy(usb_DataUpBuf, &cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_First], usb_tx_len);
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

void CDC_CARD_IO_USB_Poll()
{
  uint8_t usb_tx_len;
  if (bDeviceState == CONFIGURED) {
    if (!cdc_card_io.Tx_Busy) {
      if (cdc_card_io.PutCharBuff_First == cdc_card_io.PutCharBuff_Last) {
        if (cdc_card_io.Tx_Full) {
          // Buffer is full

          cdc_card_io.Tx_Busy = 1;

          // length (the first byte to send, the end of the buffer)
          usb_tx_len = CDC_PUTCHARBUF_LEN - cdc_card_io.PutCharBuff_First;
          memcpy(usb_DataUpBuf, &cdc_card_io.PutCharBuff[cdc_card_io.PutCharBuff_First], usb_tx_len);

          // length (the first byte in the buffer, the last byte to send), if any
          if (cdc_card_io.PutCharBuff_Last > 0)
            memcpy(&usb_DataUpBuf[usb_tx_len], cdc_card_io.PutCharBuff, cdc_card_io.PutCharBuff_Last);

          // Send the entire buffer
          CDC_CARD_IO_Upload(CDC_PUTCHARBUF_LEN);

          cdc_card_io.Tx_Full = 0;

          // A 64-byte packet is going to be sent, according to USB specification, USB uses a less-than-max-length packet to demarcate an end-of-transfer
          // As a result, we need to send a zero-length-packet.
          // return;
        }

        // Otherwise buffer is empty, nothing to send
        // return;
      } else {
        cdc_card_io.Tx_Busy = 1;

        // CDC1_PutChar() is the only way to insert into CDC1_PutCharBuf, it detects buffer overflow and notify the CDC_USB_Poll().
        // So in this condition the buffer can not be full, so we don't have to send a zero-length-packet after this.

        if (cdc_card_io.PutCharBuff_First > cdc_card_io.PutCharBuff_Last) {
          // Rollback
          // length (the first byte to send, the end of the buffer)
          usb_tx_len = CDC_PUTCHARBUF_LEN - cdc_card_io.PutCharBuff_First;
          memcpy(usb_DataUpBuf, &cdc_card_io.PutCharBuff[cdc_card_io.PutCharBuff_First], usb_tx_len);

          // length (the first byte in the buffer, the last byte to send), if any
          if (cdc_card_io.PutCharBuff_Last > 0) {
            memcpy(&usb_DataUpBuf[usb_tx_len], cdc_card_io.PutCharBuff, cdc_card_io.PutCharBuff_Last);
            usb_tx_len += cdc_card_io.PutCharBuff_Last;
          }

        } else {
          usb_tx_len = cdc_card_io.PutCharBuff_Last - cdc_card_io.PutCharBuff_First;
          memcpy(usb_DataUpBuf, &cdc_card_io.PutCharBuff[cdc_card_io.PutCharBuff_First], usb_tx_len);
        }

        cdc_card_io.PutCharBuff_First += usb_tx_len;
        if (cdc_card_io.PutCharBuff_First >= CDC_PUTCHARBUF_LEN)
          cdc_card_io.PutCharBuff_First -= CDC_PUTCHARBUF_LEN;

        // ACK next IN transfer
        CDC_CARD_IO_Upload(usb_tx_len);
      }
    }
  }
}

void CDC_LED_IO_PutChar(uint8_t tdata)
{
  // Add new data to LED IO PutCharBuf
  cdc_led_io.PutCharBuff[cdc_led_io.PutCharBuff_Last++] = tdata;
  if (cdc_led_io.PutCharBuff_Last >= CDC_PUTCHARBUF_LEN) {
    // Rotate the tail to the beginning of the buffer
    cdc_led_io.PutCharBuff_Last = 0;
  }

  if (cdc_led_io.PutCharBuff_Last == cdc_led_io.PutCharBuff_First) {
    // Buffer is full
    cdc_led_io.Tx_Full = 1;

    while (cdc_led_io.Tx_Full) // Wait until the buffer has vacancy
      CDC_LED_IO_USB_Poll();
  }
}

void CDC_CARD_IO_PutChar(uint8_t tdata)
{
  // Add new data to Card IO PutCharBuf
  cdc_card_io.PutCharBuff[cdc_card_io.PutCharBuff_Last++] = tdata;
  if (cdc_card_io.PutCharBuff_Last >= CDC_PUTCHARBUF_LEN) {
    // Rotate the tail to the beginning of the buffer
    cdc_card_io.PutCharBuff_Last = 0;
  }

  if (cdc_card_io.PutCharBuff_Last == cdc_card_io.PutCharBuff_First) {
    // Buffer is full
    cdc_card_io.Tx_Full = 1;

    while (cdc_card_io.Tx_Full) // Wait until the buffer has vacancy
      CDC_CARD_IO_USB_Poll();
  }
}

void CDC_LED_IO_Handler()
{
  uint8_t checksum, i; // Response flag, also use for checksum & i
  IO_Packet *reqPacket  = (IO_Packet *)cdc_led_io.Req_PacketBuf;
  IO_Packet *resPackect = (IO_Packet *)ledIO_ResponseStringBuf;

  memset(ledIO_ResponseStringBuf, 0x00, 64); // Clear resPackect

  resPackect->sync      = 0xE0;
  resPackect->srcNodeId = reqPacket->dstNodeId;
  resPackect->dstNodeId = reqPacket->srcNodeId;

  resPackect->response.status  = ACK_OK;
  resPackect->response.report  = REPORT_OK;
  resPackect->response.command = reqPacket->request.command;


  switch (reqPacket->request.command) {
    case CMD_RESET:
      LED_RGB_Set(LED_RGB_PORT_LEFT, 0, 0, 0, 0);
      LED_RGB_Set(LED_RGB_PORT_RIGHT, 0, 0, 0, 0);
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
      LED_RGB_Set(LED_RGB_PORT_RIGHT, 0, reqPacket->request.data[3], reqPacket->request.data[4], reqPacket->request.data[5]);      // Right
      LED_RGB_Set(LED_RGB_PORT_LEFT, 0, reqPacket->request.data[180], reqPacket->request.data[181], reqPacket->request.data[182]); // Left
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

#define CARD_READER_RATE_HI  1
#define CARD_READER_RATE_LOW 0
#define CARD_READER_RATE     CARD_READER_RATE_HI

const uint8_t CARD_READER_FW_VERSION_HIRATE[2]  = "\x94";
const uint8_t CARD_READER_FW_VERSION_LOWRATE[24] = "TN32MSEC003S F/W Ver1.2";

const uint8_t CARD_READER_VERSION_HIRATE[10]  = "837-15396";
const uint8_t CARD_READER_VERSION_LOWRATE[24] = "TN32MSEC003S H/W Ver3.0";

const uint8_t CARD_READER_EXTRA_INFO_HIRATE[13]  = "000-00000\xFF\x11\x40";
const uint8_t CARD_READER_EXTRA_INFO_LOWRATE[10] = "15084\xFF\x10\x00\x12";

// 通知 Card IO 数据已准备好
void CDC_CARD_IO_SendDataReady()
{
  cardIO_SendDataReady_Flag = 1;
}

// 发送准备好的 Card IO 数据
void CDC_CARD_IO_SendData()
{
  uint8_t checksum, i;

  cardIO_SendDataReady_Flag = 0;

  res.frame_len = 6 + res.payload_len;
  checksum       = 0;
  for (i = 0; i < res.frame_len; i++) {
    checksum += res.buffer[i];
  }
  res.buffer[res.frame_len] = checksum;

  CDC_CARD_IO_PutChar(0xE0);
  for (i = 0; i <= res.frame_len; i++) {
    if (res.buffer[i] == 0xE0 || res.buffer[i] == 0xD0) {
      CDC_CARD_IO_PutChar(0xD0);
      CDC_CARD_IO_PutChar(res.buffer[i] - 1);
    } else {
      CDC_CARD_IO_PutChar(res.buffer[i]);
    }
  }
}

// Card IO 处理函数
void CDC_CARD_IO_Handler()
{
  
  static uint8_t AimeKey[6], BanaKey[6];
  uint16_t SystemCode;
  memset(cardIO_ResponseStringBuf, 0x00, 64); // Clear resPackect

  res.addr   = _req.addr;
  res.seq_no = _req.seq_no;
  res.cmd    = _req.cmd;
  res.frame_len = 6;
  res.status = 0;
  res.payload_len = 0;
  //CDC_LED_IO_PutChar(_req.cmd);
  switch (_req.cmd) {
    case CMD_TO_NORMAL_MODE:
      // _writeCommand(CDC2_RequestPacketBuf, 0, CDC_ResponseStringBuf, 0);
      // if (getFirmwareVersion())
      // {
      //   res.status = 0x03;
      //   res.payload_len = 0;
      // }
      // else
        res.frame_len = 6;
        res.status = 0;
        res.seq_no = 0;
        res.payload_len = 0;
        memcpy(cardIO_ResponseStringBuf,res.buffer,128);
        CDC_CARD_IO_SendDataReady();
      break;
    case CMD_GET_FW_VERSION:
// #if CARD_READER_RATE == CARD_READER_RATE_HI
//       memcpy(res.version, CARD_READER_FW_VERSION_HIRATE, sizeof(CARD_READER_FW_VERSION_HIRATE));
//       res.payload_len = sizeof(CARD_READER_FW_VERSION_HIRATE);
// #elif CARD_READER_RATE == CARD_READER_RATE_LOW
      memcpy(res.version, CARD_READER_FW_VERSION_LOWRATE, sizeof(CARD_READER_FW_VERSION_LOWRATE) - 1);
      res.payload_len = sizeof(CARD_READER_FW_VERSION_LOWRATE) - 1;
      res.frame_len = 6 + res.payload_len;
// #endif
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_GET_HW_VERSION:
// #if CARD_READER_RATE == CARD_READER_RATE_HI
//       memcpy(res.version, CARD_READER_VERSION_HIRATE, 9);
//       res.payload_len = 9;
// #elif CARD_READER_RATE == CARD_READER_RATE_LOW
      memcpy(res.version, CARD_READER_VERSION_LOWRATE, sizeof(CARD_READER_VERSION_LOWRATE) - 1 );
      res.payload_len = sizeof(CARD_READER_VERSION_LOWRATE) - 1;
      res.frame_len = 6 + res.payload_len;
// #endif
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_EXT_BOARD_INFO:
// #if CARD_READER_RATE == CARD_READER_RATE_HI
//       memcpy(res.info_payload, CARD_READER_EXTRA_INFO_HIRATE, 12);
//       res.payload_len = 12;
// #elif CARD_READER_RATE == CARD_READER_RATE_LOW
      memcpy(res.info_payload, CARD_READER_EXTRA_INFO_LOWRATE, sizeof(CARD_READER_EXTRA_INFO_LOWRATE) - 1);
      res.payload_len = sizeof(CARD_READER_EXTRA_INFO_LOWRATE) - 1;
      res.frame_len = 6 + res.payload_len;
// #endif
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_MIFARE_KEY_SET_B:
      memcpy(key_B, _req.key, 6);
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_MIFARE_KEY_SET_A:
      memcpy(key_A, _req.key, 6);
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_EXT_BOARD_SET_LED_RGB:
      LED_RGB_Set(LED_RGB_PORT_UART, 0, _req.color_payload[0], _req.color_payload[1], _req.color_payload[2]);
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_EXT_TO_NORMAL_MODE:
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_START_POLLING:
      PN532_setRFField(0,1);
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_STOP_POLLING:
      PN532_setRFField(0,0);
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    case CMD_SEND_HEX_DATA:
      res.status = 0x20;
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
    //下列状态是非阻塞式处理，这里仅对PN532的发送进行处理
    case CMD_CARD_DETECT:
        PN532_Polling();
        break;
    case CMD_MIFARE_READ:
      CDC_LED_IO_PutChar(0Xe1);
      PN532_mifareclassic_ReadDataBlock(_req.block_no, res.block);
      break;
    case CMD_FELICA_THROUGH:
      PN532_felica_through();
      break;
    case CMD_MIFARE_AUTHORIZE_B:
      PN532_mifareclassic_AuthenticateBlock(_req.uid, 4, _req.block_no, 1,key_B);
      break;
    case CMD_MIFARE_AUTHORIZE_A:
      PN532_mifareclassic_AuthenticateBlock(_req.uid, 4, _req.block_no, 0,key_A);
      break;
    default:
      memcpy(cardIO_ResponseStringBuf,res.buffer,128);
      CDC_CARD_IO_SendDataReady();
      break;
  }
}

void CDC_LED_IO_UART_Poll()
{
  uint8_t cur_byte;
  static uint8_t checksum  = 0;
  static uint8_t prev_byte = 0;
  IO_Packet *packect       = (IO_Packet *)cdc_led_io.Req_PacketBuf;

  while (cdc_led_io.Rx_Pending) {
    cur_byte = cdc_led_io.Rx_PendingBuf[cdc_led_io.Rx_CurPos];
    if (cur_byte == 0xE0 && prev_byte != 0xD0) {
      checksum                 = 0x20;
      cdc_led_io.Req_PacketPos = 0;
      packect->length          = 0xFF;
    } else if (prev_byte == 0xD0) {
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
    if (cdc_led_io.Req_PacketPos > 5 && cdc_led_io.Req_PacketPos - 5 == packect->length) {
      // CDC_LED_IO_PutChar(led_io_packect->length);
      // CDC_LED_IO_PutChar(checksum);
      // CDC_LED_IO_PutChar(led_io_prev_byte);
      // CDC_LED_IO_PutChar(cur_byte);
      // CDC_LED_IO_PutChar(cdc_led_io.Rx_CurPos);
      if (checksum == cur_byte) {
        // CDC_LED_IO_PutChar(0xAA);
        CDC_LED_IO_Handler();
      } else {
        // checksum error
      }

      cdc_led_io.Req_PacketPos = 0;
      checksum                 = 0;
      prev_byte                = 0;
      cdc_led_io.Rx_Pending--;
      cdc_led_io.Rx_CurPos++;
      if (cdc_led_io.Rx_Pending == 0) {
        SetEPRxValid(CDC_LED_IO_EP);
      }
      continue;
    }

    checksum += cur_byte;

    cdc_led_io.Rx_Pending--;
    cdc_led_io.Rx_CurPos++;
    if (cdc_led_io.Rx_Pending == 0) {
      SetEPRxValid(CDC_LED_IO_EP);
    }
    prev_byte = cur_byte;
  }
}

// Card IO CDC 数据读取
void CDC_CARD_IO_UART_Poll()
{
#if PN532_UART_DIRECT != 0
  uint8_t dat;
  while (cdc_card_io.Rx_Pending) {
    dat = cdc_card_io.Rx_PendingBuf[cdc_card_io.Rx_CurPos];
    USART_SendData(USART1, dat);
    // CDC_CARD_IO_PutChar(dat);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    cdc_card_io.Rx_CurPos++;
    cdc_card_io.Rx_Pending--;
  }
  SetEPRxValid(CDC_CARD_IO_EP);
  return;
#else
  uint8_t cur_byte;
  static uint8_t checksum  = 0;
  static uint8_t prev_byte = 0;
  uint8_t len = 0;
  cdc_card_io.Rx_CurPos = 0;
  uint8_t ret = cdc_card_io.Rx_Pending;
  for(uint8_t i = 0;i<ret;i++) {
    cur_byte = cdc_card_io.Rx_PendingBuf[i];
    if (cur_byte == 0xE0) {
      _req.frame_len        = 0xFF;
      continue;
    } 
    if (_req.frame_len == 0xFF) {
      _req.frame_len = cur_byte;
      checksum = cur_byte;
      len = 0;
      continue;
    }
    if (cur_byte == 0xD0) {
      prev_byte = 0xD0;
      
      // if (cdc_card_io.Rx_Pending == 0) {
      //   SetEPRxValid(CDC_CARD_IO_EP);
      // }
      continue;
    }
    if (prev_byte == 0xD0) {
      cur_byte++;
      prev_byte = 0;
    }
    len += 1;
    _req.buffer[len] = cur_byte;
    if (len == _req.frame_len) {
      if (checksum == cur_byte) {
        CDC_CARD_IO_Handler();
      } else {
        //cdc_card_io.Req_PacketBuf[3] = 0x05; //STATUS_SUM_ERROR
        CDC_CARD_IO_Handler();
      }

      // cdc_card_io.Req_PacketPos = 0;
      // checksum                  = 0;
      // prev_byte                 = 0;
      // if (cdc_card_io.Rx_Pending == 0) {
      //   SetEPRxValid(CDC_CARD_IO_EP);
      // }
      // return;
      break;
    }

    checksum += cur_byte;
    cdc_card_io.Rx_Pending --;
  }
  SetEPRxValid(CDC_CARD_IO_EP);
#endif
}

void CDC_UART_Poll()
{
  CDC_LED_IO_UART_Poll();
  CDC_CARD_IO_UART_Poll();
}

void CDC_USB_Poll()
{
  CDC_LED_IO_USB_Poll();
  CDC_CARD_IO_USB_Poll();
}

// CDC 数据轮询
void CDC_Poll()
{
  CDC_UART_Poll();
  CDC_USB_Poll();
}

// CDC 状态机检查
void CDC_States()
{
  if (cardIO_SendDataReady_Flag) {
    CDC_CARD_IO_SendData();
  }
}

// CDC 定时检查，需注册全局定时器
void CDC_Check()
{
  CDC_Poll();
  CDC_States();
}