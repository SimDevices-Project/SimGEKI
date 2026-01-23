#ifndef __HIDIO_H_
#define __HIDIO_H_

#include "bsp.h"
#include "keyscan.h"

// Need -fshort-enums

typedef enum {
  NORMAL     = 0x00,
  JAM        = 0x01,
  DISCONNECT = 0x02,
  BUSY       = 0x03,
} __packed CoinCondition;

typedef enum {
  SET_COMM_TIMEOUT   = 0x01,
  SET_SAMPLING_COUNT = 0x02,
  CLEAR_BOARD_STATUS = 0x03,
  SET_GENERAL_OUTPUT = 0x04,
  SET_PWM_OUTPUT     = 0x05,
} __packed HidioCommand;

typedef struct {
  CoinCondition condition;
  uint8_t count;
} __packed CoinData;

typedef struct {
  uint8_t reportID;
  uint16_t analog[8];
  uint16_t rotary[4];
  CoinData coin[2];
  uint8_t buttons[4];
  uint8_t systemStatus;
  uint8_t usbStatus;
  uint8_t _unused[29];
} __packed DataUpload;

typedef struct {
  uint8_t reportID;
  HidioCommand command;
  uint8_t payload[62];
} __packed DataReceive;

typedef struct {
  uint8_t reportID;
  uint8_t ctrlkey;
  uint8_t keymap[16];
} __packed KbdData;

const static uint8_t hid_key_map[KEY_COUNT][2] = {
    // COL1
    {0},
    {0x00, 0x01}, // LA
    {0x00, 0x20}, // LB
    {0x00, 0x10}, // LC

    {0x01, 0x40}, // RSide

    // COL2
    {0},
    {0x00, 0x02}, // RA
    {0x02, 0x01}, // RB
    {0x01, 0x80}, // RC

    {0x03, 0x80}, // LSide

    // COL3
    {0x03, 0x40}, // LMenu
    {0x01, 0x20}, // RMenu
    {0x01, 0x02}, // Test
    {0x00, 0x40}, // Service
    {0},

    // COIN
    {0}, // Coin
};

const static uint8_t kbd_key_map[KEY_COUNT][2] = {
    // COL1
    {0},
    {0x00, 0x16}, // LA
    {0x00, 0x07}, // LB
    {0x00, 0x09}, // LC

    {0x00, 0x33}, // RSide

    // COL2
    {0},
    {0x00, 0x0D}, // RA
    {0x00, 0x0E}, // RB
    {0x00, 0x0F}, // RC

    {0x00, 0x04}, // LSide

    // COL3
    {0x00, 0x14}, // LMenu
    {0x00, 0x13}, // RMenu
    {0x00, 0x3A}, // Test
    {0x00, 0x3B}, // Service
    {0},

    // COIN
    {0x00, 0x3C}, // Coin
};
void HIDIO_SGIO4_Heartbeat();
void HIDIO_Init();
void HIDIO_Receive_Handler();
void HIDIO_Update();

extern uint8_t HID_Buffer_OUT[64];
extern uint8_t HID_Buffer_IN[64];

#endif // !__HIDIO_H_