#include "hidconfig.h"

uint8_t HIDCFG_Buffer_OUT[64] = {0x00};
uint8_t HIDCFG_Buffer_IN[64]  = {0x00};

static HidconfigData *dataReceive = (HidconfigData *)HIDCFG_Buffer_OUT;
static HidconfigData *dataUpload  = (HidconfigData *)HIDCFG_Buffer_IN;

void HIDCONFIG_Receive_Handler()
{
  switch (dataReceive->reportID) {
    // Custom HIDConfigs
    case 0xAA:
      switch (dataReceive->command) {
        case SET_ROLLER_OFFSET: {
          // Calc roller offset to 0x8000
          Roller_ResetOffset();
          break;
        }
        default: {
          break;
        }
      }
    default: {
      break;
    }
  }
}