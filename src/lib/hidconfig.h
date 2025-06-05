#ifndef __HIDCONFIG_H_
#define __HIDCONFIG_H_

#include "usb_lib.h"
#include "usb_prop.h"
#include "led.h"

typedef enum {
  HIDCONFIG_REPORT_ID = 0xAA, // Custom HID Report ID
} __packed HidconfigReportID;

typedef enum {
  LEDTAG_RGB_PORT_LEFT  = 0x01,
  LEDTAG_RGB_PORT_RIGHT = 0x00,
  LEDTAG_RGB_PORT_UART  = 0x02,

  LEDTAG_RGB_7C = 0xF0, // 7C RGB LED

  LEDTAG_ALL = 0xFF, // All LEDs
} __packed HidconfigLedTag;

typedef enum {
  ROLLER_SET_OFFSET = 0xA0,
  ROLLER_GET_DATA   = 0xA1,

  LED_SET_MODE = 0xB0,

  UPDATE_FIRMWARE = 0xF1,
  CMD_NOT_SUPPORT = 0xFF,
} __packed HidconfigCommand;
typedef struct {
  HidconfigReportID reportID;
  uint8_t symbol;
  HidconfigCommand command;
  uint8_t state;
  union {
    uint8_t payload[60];
    struct
    {
      HidconfigLedTag led_tag; // LED tag, 0xF0 for 7C RGB LED, 0x00-0x02 for RGB ports, 0xFF for all LEDs
      union {
        LED_7C_Tag led_7c_tag; // 7C LED tag, 0x00-0x05 for 6 LEDs
        uint8_t led_index;     // LED index, 0x00-0x05 for 6 LEDs, 0xff for all LEDs
      };
      uint8_t mode;        // LED mode, Alawys 0x01 for now
      uint8_t brightness;  // Brightness, Always 0xFF for now
      uint8_t r;           // Red color value, 0x00-0xFF
      uint8_t g;           // Green color value, 0x00-0xFF
      uint8_t b;           // Blue color value, 0x00-0xFF
      uint8_t _dammy_[53]; // Padding to 60 bytes
    };
  };
} __packed HidconfigData;

#define STATE_OK    0x01
#define STATE_ERROR 0xFF

void HIDCONFIG_Receive_Handler();

extern uint8_t HIDCFG_Buffer_OUT[64];
extern uint8_t HIDCFG_Buffer_IN[64];

#endif