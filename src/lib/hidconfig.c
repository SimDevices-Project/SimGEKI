#include "hidconfig.h"
#include "roller.h"

#include "string.h"

uint8_t HIDCFG_Buffer_OUT[64] = {0x00};
uint8_t HIDCFG_Buffer_IN[64]  = {0x00};

static HidconfigData *dataReceive = (HidconfigData *)HIDCFG_Buffer_OUT;
static HidconfigData *dataUpload  = (HidconfigData *)HIDCFG_Buffer_IN;

#define HID_CFG_EP ENDP4

void HIDCONFIG_Upload();
void HIDCONFIG_Receive_Handler();

/**
 * Request
 * 1 byte: Report ID
 * 1 byte: Symbol
 * 1 byte: Command
 * 1 byte: Unused
 * 60 bytes: Payload
 *
 * Response
 * 1 byte: Report ID
 * 1 byte: Symbol, same as request
 * 1 byte: Command, same as request
 * 1 byte: State
 * 60 bytes: Payload
 */

void HIDCONFIG_Receive_Handler()
{
  switch (dataReceive->reportID) {
    // Custom HIDConfigs
    case 0xAA: {
      memset(HIDCFG_Buffer_IN, 0, 64); // Clear buffer

      dataUpload->reportID = 0xAA;
      dataUpload->symbol   = dataReceive->symbol;

      switch (dataReceive->command) {
        case GET_ROLLER_DATA: {
          uint16_t rollerValue    = Roller_GetValue();
          uint16_t rollerRawValue = Roller_GetRawValue();
          // Upload roller data
          dataUpload->command    = GET_ROLLER_DATA;
          dataUpload->state      = STATE_OK;
          dataUpload->payload[0] = (uint8_t)(rollerValue >> 8);
          dataUpload->payload[1] = (uint8_t)(rollerValue & 0xFF);
          dataUpload->payload[2] = (uint8_t)(rollerRawValue >> 8);
          dataUpload->payload[3] = (uint8_t)(rollerRawValue & 0xFF);
          break;
        }
        case SET_ROLLER_OFFSET: {
          // Calc roller offset to 0x8000
          Roller_ResetOffset();

          dataUpload->command = SET_ROLLER_OFFSET;
          dataUpload->state   = STATE_OK;
          break;
        }
        default: {
          dataUpload->command = CMD_NOT_SUPPORT;
          dataUpload->state   = STATE_ERROR;
          break;
        }
      }
      HIDCONFIG_Upload();
      break;
    }
    default: {
      break;
    }
  }
}

void HIDCONFIG_Upload()
{
  USBD_ENDPx_DataUp(HID_CFG_EP, HIDCFG_Buffer_IN, 64);
}