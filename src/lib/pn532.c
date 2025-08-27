#include "pn532.h"
#include "timeout.h"
#include "pn532_uart.h"
#include "cdc.h"
#include "comio.h"
#include <string.h>
#include <stdio.h>

uint8_t _uid[7];       // ISO14443A uid
uint8_t _uidLen;       // uid len
uint8_t _key[6];       // Mifare Classic key
uint8_t inListedTag;   // Tg number of inlisted tag.
uint8_t _felicaIDm[8]; // FeliCa IDm (NFCID2)
uint8_t _felicaPMm[8]; // FeliCa PMm (PAD)

extern CDC_Struct cdc_card_io;

enum {
  STATUS_OK                  = 0x00,
  STATUS_CARD_ERROR          = 0x01,
  STATUS_NOT_ACCEPT          = 0x02,
  STATUS_INVALID_COMMAND     = 0x03,
  STATUS_INVALID_DATA        = 0x04,
  STATUS_SUM_ERROR           = 0x05,
  STATUS_INTERNAL_ERROR      = 0x06,
  STATUS_INVALID_FIRM_DATA   = 0x07,
  STATUS_FIRM_UPDATE_SUCCESS = 0x08,
  STATUS_COMP_DUMMY_2ND      = 0x10,
  STATUS_COMP_DUMMY_3RD      = 0x20,
};

enum {
  FelicaPolling                 = 0x00,
  Felica_reqResponce            = 0x04,
  FelicaReadWithoutEncryptData  = 0x06,
  FelicaWriteWithoutEncryptData = 0x08,
  Felica_reqSysCode             = 0x0C,
  FelicaActive2                 = 0xA4,
};

struct _PN532_Status PN532_Status;

#ifdef PN532_INTERFACE_UART
#define INTR(fun) (PN532_UART.fun)
#endif

void res_init_s(uint8_t payload_len, AIME_Request *req, AIME_Response *res)
{
  res->frame_len   = 6 + payload_len;
  res->addr        = req->addr;
  res->seq_no      = req->seq_no;
  res->cmd         = req->cmd;
  res->status      = STATUS_OK;
  res->payload_len = payload_len;
}

void res_init(uint8_t payload_len)
{
  AIME_Response *res = (AIME_Response *)cdc_card_io.Res_PacketBuf;
  AIME_Request *req  = (AIME_Request *)cdc_card_io.Req_PacketBuf;
  return res_init_s(payload_len, req, res);
}

void PN532_Polling()
{
  static uint8_t bet = 0; // Between, Mode ISO14443A or FeliCa
  if (bet) {
    PN532_readPassiveTargetID(PN532_MIFARE_ISO14443A, PN532_TIMEOUT_VAL);
    bet = 0;
  } else {
    PN532_felica_Polling(0xFFFF, 0x00, PN532_TIMEOUT_VAL);
    bet = 1;
  }
}

/**************************************************************************/
/*!
    @brief  Process timeout error
    处理通信超时错误
*/
/**************************************************************************/

void PN532_Failed()
{
  AIME_Request *req  = (AIME_Request *)cdc_card_io.Req_PacketBuf;
  AIME_Response *res = (AIME_Response *)cdc_card_io.Res_PacketBuf;

  if (PN532_Status.PN532_Option_Status == PN532_SUCCESS) {
    PN532_Status.PN532_Option_Status = PN532_STANDBY;
    return;

  } else if (PN532_Status.PN532_Option_Status == PN532_STANDBY) {
    //
  }
  PN532_Status.PN532_Option_Status = PN532_ERROR;
  switch (PN532_Status.PN532_CMD_Status) {
    case PN532_SAMCONFIG:
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_GET_VERSION:
      PN532_getFirmwareVersion();
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_SET_PASSIVE_ACTIVATION_RETRIES:
      PN532_setPassiveActivationRetries();
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_SET_RFFIELD:
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_READ_PASSIVE_TARGET_ID:
      res_init(1);
      res->count = 0;
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_MIFARE_AUTHENTICATE_BLOCK:
      res_init(0);
      res->status = STATUS_CARD_ERROR;
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_MIFARE_READ_BLOCK:
      res_init(0);
      res->status = STATUS_CARD_ERROR;
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_FELICA_POLLING:
      res_init(1);
      res->count = 0;
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_FELICA_READ:
      res->RW_status[0] = 0;
      res->RW_status[1] = 0;
      res->numBlock     = req->numBlock;
      res_init(0x0D + req->numBlock * 16);
      CDC_CARD_IO_SendDataReady();
      break;
    case PN532_FELICA_WRITE:
      res_init(0x0C);
      res->RW_status[0] = 0;
      res->RW_status[1] = 0;
      res->status       = STATUS_CARD_ERROR;
      CDC_CARD_IO_SendDataReady();
      break;
    default:
      res_init(0);
      res->status = STATUS_INVALID_COMMAND;
      CDC_CARD_IO_SendDataReady();
      break;
  }
}

/**************************************************************************/
/*!
    @brief  Process UART data
*/
/**************************************************************************/

int16_t readResponse(uint8_t *buf, uint8_t len, uint8_t command)
{
  if (len < 4) {
    return PN532_NO_SPACE;
  }
  if (0 != buf[0] || 0 != buf[1] || 0xFF != buf[2]) {
    // Preamble error
    return PN532_INVALID_FRAME;
  }
  uint8_t lenth_chk = buf[3] + buf[4];
  if (lenth_chk) {
    // Length error
    return PN532_INVALID_FRAME;
  }
  buf[3] -= 2;
  if (buf[3] > len) {
    return PN532_NO_SPACE;
  }

  /** receive command byte */
  uint8_t cmd = command + 1; // response command

  if (PN532_PN532TOHOST != buf[5] || cmd != buf[6]) {
    // Command error
    return PN532_INVALID_FRAME;
  }

  uint8_t sum = PN532_PN532TOHOST + cmd;
  for (uint8_t i = 0; i < buf[3]; i++) {
    sum += buf[i + 7];
  }
  //   uint8_t chksum = sum + buf[8+buf[3]] ;
  //   /** checksum and postamble */
  //   if (chksum || 0 != buf[9+buf[3]])
  //   {
  //       //Checksum error
  //       return PN532_INVALID_FRAME;
  //   }
  // return length[0];
  return 0;
}
/**************************************************************************/
/*!
    @brief  Polling for uart response
*/
/**************************************************************************/
void PN532_Check()
{

  AIME_Request *req  = (AIME_Request *)cdc_card_io.Req_PacketBuf;
  AIME_Response *res = (AIME_Response *)cdc_card_io.Res_PacketBuf;

  // printf("uart check");
#ifdef PN532_INTERFACE_UART
  static uint8_t buffer[128];
  static uint8_t size = 0;
  size                = 0;
  INTR(getResponse)(buffer, &size);
  if (size == 0) {
    return;
  }
  if (PN532_Status.PN532_Option_Status == PN532_WAITING_FOR_ACK) {
    PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_RESPONSE;
    if ((buffer[0] == 0) && (buffer[1] == 0) && (buffer[2] == 0xff) && (buffer[3] == 0) && (buffer[4] == 0xff) && (buffer[5] == 0)) {
      memmove(buffer, buffer + 6, size);
    }
  }
  if (PN532_Status.PN532_Option_Status == PN532_WAITING_FOR_RESPONSE) {
    uint8_t status;

    switch (PN532_Status.PN532_CMD_Status) {
      case PN532_SAMCONFIG:
        if (!readResponse(buffer, size, PN532_COMMAND_SAMCONFIGURATION)) {
          // success
          PN532_Status.PN532_Option_Status = PN532_SUCCESS;
          clearTimeout(PN532_Status.PN532_Failed_task_key);
        }
        break;
      case PN532_GET_VERSION:
        PN532_Status.PN532_Option_Status = PN532_SUCCESS;
        break;
      case PN532_SET_PASSIVE_ACTIVATION_RETRIES:
        if (!readResponse(buffer, size, PN532_COMMAND_RFCONFIGURATION)) {
          // success
          PN532_Status.PN532_Option_Status = PN532_SUCCESS;
          clearTimeout(PN532_Status.PN532_Failed_task_key);
        }
        break;
      case PN532_SET_RFFIELD:
        if (!readResponse(buffer, size, PN532_COMMAND_RFCONFIGURATION)) {
          // success
          PN532_Status.PN532_Option_Status = PN532_SUCCESS;
          clearTimeout(PN532_Status.PN532_Failed_task_key);
        }
        break;
      case PN532_READ_PASSIVE_TARGET_ID:
        if (!readResponse(buffer, size, PN532_COMMAND_INLISTPASSIVETARGET)) {
          // success
          PN532_Status.PN532_Option_Status = PN532_SUCCESS;
          clearTimeout(PN532_Status.PN532_Failed_task_key);
          if (buffer[7] == 0) {
            res_init(1);
            res->count = 0;
            CDC_CARD_IO_SendDataReady();
            break;
          }
          res_init(0x07);
          res->id_len = buffer[5 + 7];
          for (uint8_t i = 0; i < buffer[5 + 7]; i++) {
            res->mifare_uid[i] = buffer[6 + i + 7];
          }
          res->count = 1;
          res->type  = 0x10;
          CDC_CARD_IO_SendDataReady();
        }
        break;
      case PN532_MIFARE_AUTHENTICATE_BLOCK:
        if (!readResponse(buffer, size, PN532_COMMAND_INDATAEXCHANGE)) {
          // Check if the response is valid and we are authenticated???
          // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
          // Mifare auth error is technically byte 7: 0x14 but anything other and 0x00 is not good
          if (buffer[7] == 0x00) {
            PN532_Status.PN532_Option_Status = PN532_SUCCESS;
            // Authentification success
            clearTimeout(PN532_Status.PN532_Failed_task_key);
            res_init(0);
            CDC_CARD_IO_SendDataReady();
          }
        }
        break;
      case PN532_MIFARE_READ_BLOCK:
        if (!readResponse(buffer, size, PN532_COMMAND_INDATAEXCHANGE)) {
          /* If isn't 0x00 we probably have an error */
          if (buffer[7] == 0x00) {
            PN532_Status.PN532_Option_Status = PN532_SUCCESS;
            clearTimeout(PN532_Status.PN532_Failed_task_key);
            res_init(0x10);
            memcpy(res->block, &buffer[8], 16);
            CDC_CARD_IO_SendDataReady();
          }
        }
        break;
      case PN532_FELICA_POLLING:
        if (!readResponse(buffer, size, PN532_COMMAND_INLISTPASSIVETARGET)) {
          // Check NbTg (pn532_packetbuffer[7])
          if (buffer[7] == 0) {
            PN532_Status.PN532_Option_Status = PN532_SUCCESS;
            clearTimeout(PN532_Status.PN532_Failed_task_key);
            res_init(1);
            res->count = 0;
            CDC_CARD_IO_SendDataReady();
            break;
          }
          if (buffer[7] != 1) {
            // Unhandled number of targets inlisted.
            break;
          }
          inListedTag = buffer[7 + 1];
          // length check
          uint8_t responseLength = buffer[7 + 2];
          if (responseLength != 18 && responseLength != 20) {
            // Wrong response length
            break;
          }
          PN532_Status.PN532_Option_Status = PN532_SUCCESS;
          clearTimeout(PN532_Status.PN532_Failed_task_key);
          for (uint8_t i = 0; i < 8; ++i) {
            res->IDm[i] = buffer[7 + 4 + i];
            res->PMm[i] = buffer[7 + 12 + i];
          }
          memcpy(PN532_Status.PN532_felica_pmm, res->PMm, 8);
          memcpy(PN532_Status.PN532_felica_idm, res->IDm, 8);
          if (responseLength == 20) {
            PN532_Status.PN532_felica_systemcode[0] = buffer[7 + 21];
            PN532_Status.PN532_felica_systemcode[1] = buffer[7 + 20];
          }
          res_init(0x13);
          res->count  = 1;
          res->type   = 0x20;
          res->id_len = 0x10;
          CDC_CARD_IO_SendDataReady();
        }
        break;
      case PN532_FELICA_READ:
        // CDC_LED_IO_PutChar(0xA1);
        status = readResponse(buffer, size, PN532_COMMAND_INDATAEXCHANGE);
        if (status == 0) {
          // CDC_LED_IO_PutChar(0xA2);
          // Check status
          if ((buffer[7] & 0x3F) != 0) {
            // CDC_LED_IO_PutChar(0xA3);
            // Status code indicates an error
            return;
          }
          // length check
          if ((status - 2) != buffer[8] - 1) {
            // CDC_LED_IO_PutChar(0xA4);
            // Wrong response length
            // return;
          }
          // length check
          if ((buffer[8] - 1) != 12 + 16 * PN532_Status.PN532_PARAMETER) {
            // CDC_LED_IO_PutChar(0xA5);
            // Read Without Encryption command failed (wrong response length)
            return;
          }

          // status flag check
          if (buffer[9 + 9] != 0 || buffer[9 + 10] != 0) {
            // CDC_LED_IO_PutChar(0xA6);
            // Read Without Encryption command failed
            return;
          }
          // CDC_LED_IO_PutChar(0xA7);
          PN532_Status.PN532_Option_Status = PN532_SUCCESS;
          clearTimeout(PN532_Status.PN532_Failed_task_key);
          uint8_t k = 9 + 12;
          for (uint8_t i = 0; i < PN532_Status.PN532_PARAMETER; i++) {
            for (uint8_t j = 0; j < 16; j++) {
              res->blockData[i][j] = buffer[k++];
            }
          }
          res->RW_status[0] = 0;
          res->RW_status[1] = 0;
          res->numBlock     = req->numBlock;
          res_init(0x0D + req->numBlock * 16);
          res->encap_len = 0x0D + req->numBlock * 16;
          CDC_CARD_IO_SendDataReady();
        }
        break;
      case PN532_FELICA_WRITE:
        // CDC_LED_IO_PutChar(0xB1);
        status = readResponse(buffer, size, PN532_COMMAND_INDATAEXCHANGE);
        if (status < 0) {
          // CDC_LED_IO_PutChar(0xB2);
          // Could not receive response
          return;
        }
        // Check status
        // if ((buffer[7] & 0x3F)!=0) {
        //   CDC_LED_IO_PutChar(0xB3);
        //   //Status code indicates an error
        //   return;
        // }
        // // length check
        // if ( (status - 2) != buffer[7+1] - 1) {
        //   CDC_LED_IO_PutChar(0xB4);
        //   //Wrong response length
        //   //return;
        // }
        // if ( (buffer[7+1] - 1) != 11 ) {
        //   CDC_LED_IO_PutChar(0xB5);
        //   //Write Without Encryption command failed (wrong response length)
        //   return;
        // }

        // // status flag check
        // if ( buffer[9+9] != 0 || buffer[9+10] != 0 ) {
        //   CDC_LED_IO_PutChar(0xB6);
        //   //Write Without Encryption command failed
        //   return;
        // }
        // CDC_LED_IO_PutChar(0xB7);
        PN532_Status.PN532_Option_Status = PN532_SUCCESS;
        clearTimeout(PN532_Status.PN532_Failed_task_key);
        res_init(0x0C);
        res->RW_status[0] = 0;
        res->RW_status[1] = 0;
        res->encap_len    = res->payload_len;
        CDC_CARD_IO_SendDataReady();
        break;
      default:
        break;
    }
  }
#endif
}

void PN532_felica_through()
{
  AIME_Request *req = (AIME_Request *)cdc_card_io.Req_PacketBuf;
  AIME_Response *res = (AIME_Response *)cdc_card_io.Res_PacketBuf;

  memcpy(req->encap_IDm, PN532_Status.PN532_felica_idm, 8);
  // memcpy(res->poll_PMm,PN532_Status.PN532_felica_pmm,8);

  uint8_t code   = req->encap_code;
  res->encap_code = code + 1;
  switch (code) {
    case FelicaPolling:
      res_init(0x14);
      // res->poll_system_code[0] = PN532_Status.PN532_felica_systemcode[0];
      // res->poll_system_code[1] = PN532_Status.PN532_felica_systemcode[1];
      res->poll_system_code[0] = 0x88;
      res->poll_system_code[1] = 0xb4;
      break;
    case Felica_reqSysCode:
      res_init(0x0D);
      res->felica_payload[0] = 0x01;
      // res->felica_payload[1] = PN532_Status.PN532_felica_systemcode[0];
      // res->felica_payload[2] = PN532_Status.PN532_felica_systemcode[1];
      res->poll_system_code[0] = 0x88;
      res->poll_system_code[1] = 0xb4;
      break;
    case FelicaActive2: {
      res_init(0x0B);
      res->felica_payload[0] = 0x00;
      break;
    }
    case FelicaReadWithoutEncryptData: {
#if PN532_DISABLE_FELICA_CHECK == 0
      uint16_t serviceCodeList = req->serviceCodeList[1] << 8 | req->serviceCodeList[0];
      uint16_t blockList[4];
      for (uint8_t i = 0; i < req->numBlock; i++) {
        blockList[i] = (uint16_t)(req->blockList[i][0] << 8 | req->blockList[i][1]);
      }
#endif
#if PN532_DISABLE_FELICA_CHECK == 0
      PN532_felica_ReadWithoutEncryption(1, &serviceCodeList, req->numBlock, blockList, res->blockData);
#endif
#if PN532_DISABLE_FELICA_CHECK == 1
      PN532_Status.PN532_PARAMETER = req->numBlock;
      if (PN532_Status.PN532_PARAMETER == 1) {
        for (uint8_t j = 0; j < 8; j++) {
          res->blockData[0][j] = PN532_Status.PN532_felica_idm[j];
        }
      } else {
        for (uint8_t j = 0; j < 8; j++) {
          res->blockData[0][j] = PN532_Status.PN532_felica_idm[j];
        }
        for (uint8_t i = 1; i < 4; i++) {
          for (uint8_t j = 0; j < 16; j++) {
            res->blockData[i][j] = 0;
          }
        }
        res->blockData[1][1] = 1;
      }
      res->RW_status[0] = 0;
      res->RW_status[1] = 0;
      res->numBlock     = req->numBlock;
      res_init(0x0D + req->numBlock * 16);
      res->encap_len = 0x0D + req->numBlock * 16;
      CDC_CARD_IO_SendDataReady();
#endif
      return;
    } break;
    case FelicaWriteWithoutEncryptData: {
#if PN532_DISABLE_FELICA_CHECK == 0
      uint16_t serviceCodeList = req->serviceCodeList[1] << 8 | req->serviceCodeList[0];
      uint16_t blockList       = (uint16_t)(req->blockList_write[0][0] << 8 | req->blockList_write[0][1]);
      PN532_felica_WriteWithoutEncryption(1, &serviceCodeList, 1, &blockList, &req->blockData);
#endif
#if PN532_DISABLE_FELICA_CHECK == 1
      PN532_Status.PN532_PARAMETER = req->numBlock;
      res_init(0x0C);
      res->RW_status[0] = 0;
      res->RW_status[1] = 0;
      res->encap_len    = res->payload_len;
      CDC_CARD_IO_SendDataReady();
#endif
      return;
    } break;
    default:
      res_init(0);
      res->status = STATUS_INVALID_COMMAND;
  }
  res->encap_len = res->payload_len;
  CDC_CARD_IO_SendDataReady();
}

/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
void PN532_Init()
{
  INTR(begin)();
  INTR(wakeup)();
  setTimeout(PN532_getFirmwareVersion, 100);
  setTimeout(PN532_setPassiveActivationRetries, 200);
  setTimeout(PN532_SAMConfig, 300);
}

/**************************************************************************/
/*!
    @brief  Checks the firmware version of the PN5xx chip

    @returns  The chip's firmware version and ID
*/
/**************************************************************************/
void PN532_getFirmwareVersion(void)
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = (PN532_COMMAND_GETFIRMWAREVERSION);

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_GET_VERSION;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);

  INTR(writeCommand)(packet_buffer, 1, 0, 0);
}

/**************************************************************************/
/*!
    @brief  Configures the SAM (Secure Access Module)
*/
/**************************************************************************/
void PN532_SAMConfig()
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  packet_buffer[1] = (0x01); // normal mode;
  packet_buffer[2] = (0x17); // timeout 50ms * 20 = 1 second
  packet_buffer[3] = (0x00); //  use IRQ pin

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_SAMCONFIG;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, 5000);

  INTR(writeCommand)(packet_buffer, 4, 0, 0);
}

/**************************************************************************/
/*!
    Sets the MxRtyPassiveActivation uint8_t of the RFConfiguration register

    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_setPassiveActivationRetries()
{
  uint8_t maxRetries = 0x10;

  uint8_t packet_buffer[64];
  packet_buffer[0] = (PN532_COMMAND_RFCONFIGURATION);
  packet_buffer[1] = (5);    // Config item 5 (MaxRetries)
  packet_buffer[2] = (0xFF); // MxRtyATR (default = 0xFF)
  packet_buffer[3] = (0x01); // MxRtyPSL (default = 0x01)
  packet_buffer[4] = (maxRetries);

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_SET_PASSIVE_ACTIVATION_RETRIES;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);

  INTR(writeCommand)(packet_buffer, 5, 0, 0);
}

/**************************************************************************/
/*!
    Sets the RFon/off uint8_t of the RFConfiguration register

    @param  autoRFCA    0x00 No check of the external field before
                        activation

                        0x02 Check the external field before
                        activation

    @param  rFOnOff     0x00 Switch the RF field off, 0x01 switch the RF
                        field on

    @returns    1 if everything executed properly, 0 for an error
*/
/**************************************************************************/

void PN532_setRFField(uint8_t autoRFCA, uint8_t rFOnOff)
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = (PN532_COMMAND_RFCONFIGURATION);
  packet_buffer[1] = (1);
  packet_buffer[2] = (0x00 | autoRFCA | rFOnOff);

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_SET_RFFIELD;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);

  INTR(writeCommand)(packet_buffer, 3, 0, 0);
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*!
    Waits for an ISO14443A target to enter the field

    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    @param  timeout       The number of tries before timing out
    @param  inlist        If set to true, the card will be inlisted

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_readPassiveTargetID(uint8_t cardbaudrate, uint16_t timeout)
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = (PN532_COMMAND_INLISTPASSIVETARGET);
  packet_buffer[1] = (1); // max 1 cards at once (we can set this to 2 later)
  packet_buffer[2] = (cardbaudrate);

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_READ_PASSIVE_TARGET_ID;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, timeout);

  INTR(writeCommand)(packet_buffer, 3, 0, 0);
}

/***** Mifare Classic Functions ******/

/**************************************************************************/
/*!
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 bytes
                          key value

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_mifareclassic_AuthenticateBlock(uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData)
{
  // Prepare the authentication command //
  uint8_t packet_buffer[64];
  packet_buffer[0] = (PN532_COMMAND_INDATAEXCHANGE); /* Data Exchange Header */
  packet_buffer[1] = 1;                              /* Max card numbers */
  packet_buffer[2] = (keyNumber ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A);
  packet_buffer[3] = blockNumber; /* Block Number (1K = 0..63, 4K = 0..255 */
  // memcpy(packet_buffer + 4, keyData, 6);
  for (uint8_t i = 0; i < 6; i++) {
    packet_buffer[4 + i] = keyData[i]; /* 4 bytes card ID */
  }
  for (uint8_t i = 0; i < uidLen; i++) {
    packet_buffer[10 + i] = uid[i]; /* 4 bytes card ID */
  }

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_MIFARE_AUTHENTICATE_BLOCK;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);

  INTR(writeCommand)(packet_buffer, 10 + uidLen, 0, 0);
}

/**************************************************************************/
/*!
    Tries to read an entire 16-bytes data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the byte array that will hold the
                          retrieved data (if any)

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_mifareclassic_ReadDataBlock(uint8_t blockNumber, uint8_t *data)
{
  uint8_t packet_buffer[64];
  /* Prepare the command */
  packet_buffer[0] = (PN532_COMMAND_INDATAEXCHANGE);
  packet_buffer[1] = (1);               /* Card number */
  packet_buffer[2] = (MIFARE_CMD_READ); /* Mifare Read command = 0x30 */
  packet_buffer[3] = (blockNumber);     /* Block Number (0..63 for 1K, 0..255 for 4K) */

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_MIFARE_READ_BLOCK;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);

  INTR(writeCommand)(packet_buffer, 4, 0, 0);
}

/***** FeliCa Functions ******/
/**************************************************************************/
/*!
    @brief  Poll FeliCa card. PN532 acting as reader/initiator,
            peer acting as card/responder.
    @param[in]  systemCode             Designation of System Code. When sending FFFFh as System Code,
                                       all FeliCa cards can return response.
    @param[in]  _requestCode            Designation of _request Data as follows:
                                         00h: No _request
                                         01h: System Code _request (to acquire System Code of the card)
                                         02h: Communication perfomance _request
    @param[out] idm                    IDm of the card (8 bytes)
    @param[out] pmm                    PMm of the card (8 bytes)
    @param[out] systemCodeResponse     System Code of the card (Optional, 2bytes)
    @return                            = 1: A FeliCa card has detected
                                       = 0: No card has detected
                                       < 0: error
*/
/**************************************************************************/
void PN532_felica_Polling(uint16_t systemCode, uint8_t _requestCode, uint16_t timeout)
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = (PN532_COMMAND_INLISTPASSIVETARGET);
  packet_buffer[1] = (1);
  packet_buffer[2] = (1);
  packet_buffer[3] = (FELICA_CMD_POLLING);
  packet_buffer[4] = ((systemCode >> 8) & 0xFF);
  packet_buffer[5] = (systemCode & 0xFF);
  packet_buffer[6] = (_requestCode);
  packet_buffer[7] = (0);

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_FELICA_POLLING;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, timeout);

  INTR(writeCommand)(packet_buffer, 8, 0, 0);
}

/**************************************************************************/
/*!
    @brief  Sends FeliCa command to the currently inlisted peer

    @param[in]  command         FeliCa command packet. (e.g. 00 FF FF 00 00  for Polling command)
    @param[in]  commandlength   Length of the FeliCa command packet. (e.g. 0x05 for above Polling command )
    @param[out] response        FeliCa response packet. (e.g. 01 NFCID2(8 bytes) PAD(8 bytes)  for Polling response)
    @param[out] responselength  Length of the FeliCa response packet. (e.g. 0x11 for above Polling command )
    @return                          = 1: Success
                                     < 0: error
*/
/**************************************************************************/
void PN532_felica_SendCommand(const uint8_t *command, uint8_t commandlength)
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = 0x40; // PN532_COMMAND_INDATAEXCHANGE;--
  packet_buffer[1] = inListedTag;
  packet_buffer[2] = commandlength + 1;
  INTR(writeCommand)(packet_buffer, 3, command, commandlength);
}

/**************************************************************************/
/*!
    @brief  Sends FeliCa Read Without Encryption command

    @param[in]  numService         Length of the serviceCodeList
    @param[in]  serviceCodeList    Service Code List (Big Endian)
    @param[in]  numBlock           Length of the blockList
    @param[in]  blockList          Block List (Big Endian, This API only accepts 2-byte block list element)
    @param[out] blockData          Block Data
    @return                        = 1: Success
                                   < 0: error
*/
/**************************************************************************/
void PN532_felica_ReadWithoutEncryption(uint8_t numService, const uint16_t *serviceCodeList, uint8_t numBlock, const uint16_t *blockList, uint8_t blockData[][16])
{
  if (numService > FELICA_READ_MAX_SERVICE_NUM) {
    PN532_Status.PN532_Option_Status   = PN532_ERROR;
    PN532_Status.PN532_CMD_Status      = PN532_FELICA_READ;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, 0);
    return;
  }
  if (numBlock > 4) {
    PN532_Status.PN532_Option_Status   = PN532_ERROR;
    PN532_Status.PN532_CMD_Status      = PN532_FELICA_READ;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, 0);
    return;
  }
  PN532_Status.PN532_PARAMETER = numBlock;
  uint8_t i, j = 0;
  uint8_t cmdLen = 1 + 8 + 1 + 2 * numService + 1 + 2 * numBlock;
  uint8_t cmd[cmdLen];
  cmd[j++] = FELICA_CMD_READ_WITHOUT_ENCRYPTION;
  for (i = 0; i < 8; ++i) {
    cmd[j++] = _felicaIDm[i];
  }
  cmd[j++] = numService;
  for (i = 0; i < numService; ++i) {
    cmd[j++] = serviceCodeList[i] & 0xFF;
    cmd[j++] = (serviceCodeList[i] >> 8) & 0xff;
  }
  cmd[j++] = numBlock;
  for (i = 0; i < numBlock; ++i) {
    cmd[j++] = (blockList[i] >> 8) & 0xFF;
    cmd[j++] = blockList[i] & 0xff;
  }

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_FELICA_READ;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);
  PN532_felica_SendCommand(cmd, cmdLen);
}

/**************************************************************************/
/*!
    @brief  Sends FeliCa Write Without Encryption command

    @param[in]  numService         Length of the serviceCodeList
    @param[in]  serviceCodeList    Service Code List (Big Endian)
    @param[in]  numBlock           Length of the blockList
    @param[in]  blockList          Block List (Big Endian, This API only accepts 2-byte block list element)
    @param[in]  blockData          Block Data (each Block has 16 bytes)
    @return                        = 1: Success
                                   < 0: error
*/
/**************************************************************************/
void PN532_felica_WriteWithoutEncryption(uint8_t numService, const uint16_t *serviceCodeList, uint8_t numBlock, const uint16_t *blockList, uint8_t blockData[][16])
{
  if (numService > FELICA_WRITE_MAX_SERVICE_NUM) {
    PN532_Status.PN532_Option_Status   = PN532_ERROR;
    PN532_Status.PN532_CMD_Status      = PN532_FELICA_WRITE;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, 0);
    return;
  }
  if (numBlock > FELICA_WRITE_MAX_BLOCK_NUM) {
    PN532_Status.PN532_Option_Status   = PN532_ERROR;
    PN532_Status.PN532_CMD_Status      = PN532_FELICA_WRITE;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, 0);
    return;
  }
  PN532_Status.PN532_PARAMETER = numBlock;
  uint8_t i, j = 0, k;
  uint8_t cmdLen = 1 + 8 + 1 + 2 * numService + 1 + 2 * numBlock + 16 * numBlock;
  uint8_t cmd[cmdLen];
  cmd[j++] = FELICA_CMD_WRITE_WITHOUT_ENCRYPTION;
  for (i = 0; i < 8; ++i) {
    cmd[j++] = _felicaIDm[i];
  }
  cmd[j++] = numService;
  for (i = 0; i < numService; ++i) {
    cmd[j++] = serviceCodeList[i] & 0xFF;
    cmd[j++] = (serviceCodeList[i] >> 8) & 0xff;
  }
  cmd[j++] = numBlock;
  for (i = 0; i < numBlock; ++i) {
    cmd[j++] = (blockList[i] >> 8) & 0xFF;
    cmd[j++] = blockList[i] & 0xff;
  }
  for (i = 0; i < numBlock; ++i) {
    for (k = 0; k < 16; k++) {
      cmd[j++] = blockData[i][k];
    }
  }

  PN532_Status.PN532_Option_Status   = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status      = PN532_FELICA_WRITE;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed, PN532_TIMEOUT_VAL);
  PN532_felica_SendCommand(cmd, cmdLen);
}