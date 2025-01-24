#ifndef __HIDCONFIG_H_
#define __HIDCONFIG_H_

#include "usb_lib.h"
#include "usb_prop.h"

#include "keyscan.h"
#include "roller.h"

typedef enum {
  SET_ROLLER_OFFSET = 0xA0,
  UPDATE_FIRMWARE   = 0xF1,
} __packed HidconfigCommand;

typedef struct {
  uint8_t reportID;
  HidconfigCommand command;
  uint8_t payload[62];
} __packed HidconfigData;

void HIDCONFIG_Receive_Handler();

extern uint8_t HIDCFG_Buffer_OUT[64];
extern uint8_t HIDCFG_Buffer_IN[64];


#endif