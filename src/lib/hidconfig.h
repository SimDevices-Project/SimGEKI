#ifndef __HIDCONFIG_H_
#define __HIDCONFIG_H_

#include "usb_lib.h"
#include "usb_prop.h"

typedef enum {
  SET_ROLLER_OFFSET = 0xA0,
  GET_ROLLER_DATA   = 0xA1,
  UPDATE_FIRMWARE   = 0xF1,
  CMD_NOT_SUPPORT   = 0xFF,
} __packed HidconfigCommand;
typedef struct {
  uint8_t reportID;
  uint8_t symbol;
  HidconfigCommand command;
  uint8_t state;
  uint8_t payload[60];
} __packed HidconfigData;

#define STATE_OK    0x01
#define STATE_ERROR 0xFF

void HIDCONFIG_Receive_Handler();

extern uint8_t HIDCFG_Buffer_OUT[64];
extern uint8_t HIDCFG_Buffer_IN[64];

#endif