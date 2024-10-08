#include "hidio.h"
#include "ch422.h"
#include "keyscan.h"
#include "roller.h"

// #include "debug.h"

#include "usb_lib.h"
#include "usb_prop.h"

#define OUT_EP ENDP1

uint8_t HID_Buffer_OUT[64] = {0x00};
uint8_t HID_Buffer_IN[64]  = {0x00};

DataReceive *dataReceive = (DataReceive *)HID_Buffer_OUT;
DataUpload *dataUpload   = (DataUpload *)HID_Buffer_IN;

static uint16_t prevKeyStatus    = 0;
static uint16_t activeKeyStatus  = 0;
static uint16_t changedKeyStatus = 0;

static uint16_t prevRollerValue   = 0;
static uint16_t activeRollerValue = 0;

const uint8_t bitPosMap[] = {23, 20, 22, 19, 21, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6};

void HIDIO_Receive_Handler()
{
  switch (dataReceive->reportID) {
    // Default HIDIO
    case 0x10:
      switch (dataReceive->command) {
        case SET_COMM_TIMEOUT: {
          dataUpload->systemStatus = 0x30;
          break;
        }
        case SET_SAMPLING_COUNT: {
          dataUpload->systemStatus = 0x30;
          break;
        }
        case CLEAR_BOARD_STATUS: {
          dataUpload->systemStatus = 0x00;

          dataUpload->coin[0].count     = 0;
          dataUpload->coin[0].condition = NORMAL;
          dataUpload->coin[1].count     = 0;
          dataUpload->coin[1].condition = NORMAL;
          break;
        }
        case SET_GENERAL_OUTPUT: {
          uint32_t raw_led_dat = (uint32_t)(dataReceive->payload[0]) << 16 | (uint32_t)(dataReceive->payload[1]) << 8 | dataReceive->payload[2];
          uint32_t ch422_dat   = 0;
          for (uint8_t i = 0; i < 3; i++) {
            ch422_dat <<= 1;
            ch422_dat |= (raw_led_dat >> bitPosMap[9 + i * 3]) & 1;
            ch422_dat <<= 1;
            ch422_dat |= (raw_led_dat >> bitPosMap[9 + i * 3 + 1]) & 1;
            ch422_dat <<= 1;
            ch422_dat |= (raw_led_dat >> bitPosMap[9 + i * 3 + 2]) & 1;

            ch422_dat <<= 1;
            ch422_dat |= (raw_led_dat >> bitPosMap[i * 3]) & 1; // R
            ch422_dat <<= 1;
            ch422_dat |= (raw_led_dat >> bitPosMap[i * 3 + 1]) & 1; // G
            ch422_dat <<= 1;
            ch422_dat |= (raw_led_dat >> bitPosMap[i * 3 + 2]) & 1; // B
          }
          CH422_Set(ch422_dat);
          break;
        }
        default: {
          break;
        }
      }
      break;
    // Custom HIDIO
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

void HIDIO_Upload()
{
  USBD_ENDPx_DataUp(OUT_EP, HID_Buffer_IN, 64);
}

void HIDIO_FreshData()
{
  // Roller
  dataUpload->analog[0] = activeRollerValue;

  // Buttons
  dataUpload->buttons[0] = 0x00;
  dataUpload->buttons[1] = 0x00;
  dataUpload->buttons[2] = 0x00;
  dataUpload->buttons[3] = 0x00;

  for (uint8_t i = 0; i < KEY_COUNT - 1; i++) {
    if (KeyScan_GetKeyDebouncedStatus(i)) {
      dataUpload->buttons[hid_key_map[i][0]] |= hid_key_map[i][1];
    }
  }
  dataUpload->buttons[3] ^= 0x80; // Lside取反
  dataUpload->buttons[1] ^= 0x40; // Rside取反

  // Coin
  if ((changedKeyStatus & (1 << (KEY_COUNT - 1))) && KeyScan_GetKeyDebouncedStatus(KEY_COUNT - 1)) {
    dataUpload->coin[0].count++;
    dataUpload->coin[1].count++;
  }

  HIDIO_Upload();
}

void HIDIO_Update()
{
  uint8_t freshRequired = 0;
  KeyScan_Scan();
  activeKeyStatus = KeyScan_GetAllKeyDebouncedStatus();
  if (activeKeyStatus != prevKeyStatus) {
    freshRequired = 1;
  }
  changedKeyStatus = activeKeyStatus ^ prevKeyStatus;
  prevKeyStatus    = activeKeyStatus;

  Roller_Update();
  activeRollerValue = ~Roller_GetValue();
  if (activeRollerValue != prevRollerValue) {
    freshRequired = 1;
  }
  prevRollerValue = activeRollerValue;

  if (freshRequired) {
    HIDIO_FreshData();
  }
}

xdata void HIDIO_Init()
{
  KeyScan_Init();
  Roller_Init();
  dataUpload->reportID = 0x01;

  dataUpload->analog[0] = 0x8000;
  // dataForUpload->rotary[0]    = 0x8100;
  dataUpload->buttons[1] = 0x40;
  dataUpload->buttons[3] = 0x80;
  // dataForUpload->systemStatus = 0x02;
  dataUpload->systemStatus = 0x30;

  HIDIO_FreshData();
}
