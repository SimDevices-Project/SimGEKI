#include "hidio.h"
#include "led.h"
#include "keyscan.h"
#include "roller.h"
#include "sleep.h"

// #include "debug.h"

#include "timeout.h"

#include "usb_lib.h"
#include "usb_prop.h"

#define OUT_EP ENDP1

uint8_t HID_Buffer_OUT[64] = {0x00};
uint8_t HID_Buffer_IN[64]  = {0x00};

static DataReceive *dataReceive = (DataReceive *)HID_Buffer_OUT;
static DataUpload *dataUpload   = (DataUpload *)HID_Buffer_IN;

static uint16_t prevKeyStatus    = 0;
static uint16_t activeKeyStatus  = 0;
static uint16_t changedKeyStatus = 0;

static uint16_t prevRollerValue   = 0;
static uint16_t activeRollerValue = 0;

uint8_t intervalID = 0xFF;

const uint8_t bitPosMap[] = {23, 20, 22, 19, 21, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6};

void HIDIO_Receive_Handler()
{
  switch (dataReceive->reportID) {
    // Default HIDIO
    case 0x10:
      switch (dataReceive->command) {
        case SET_COMM_TIMEOUT: {
          dataUpload->systemStatus = 0x30;

          clearInterval(intervalID);
          intervalID = setInterval(HIDIO_Upload, 5);
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
          // uint32_t ch422_dat   = 0;
          for (uint8_t i = 0; i < 3; i++) {
            LED_7C_Set(i * 2, (raw_led_dat >> bitPosMap[9 + i * 3]) & 1, (raw_led_dat >> bitPosMap[9 + i * 3 + 1]) & 1, (raw_led_dat >> bitPosMap[9 + i * 3 + 2]) & 1);
            LED_7C_Set(i * 2 + 1, (raw_led_dat >> bitPosMap[i * 3]) & 1, (raw_led_dat >> bitPosMap[i * 3 + 1]) & 1, (raw_led_dat >> bitPosMap[i * 3 + 2]) & 1);
          }
          // CH422_Set(ch422_dat);
          break;
        }
        default: {
          break;
        }
      }
      break;
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

  // 仅早于 v3.8 的主控版本需要取反
#ifdef EARLY_HARDWARE
  dataUpload->buttons[3] ^= 0x80; // Lside取反
  dataUpload->buttons[1] ^= 0x40; // Rside取反
#endif

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
    Sleep_Alive();
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

  clearInterval(intervalID);
  intervalID = setInterval(HIDIO_Upload, 30);
}
