#include "hidio.h"
#include "led.h"
#include "keyscan.h"
#include "roller.h"
#include "sleep.h"
#include "string.h"
#include "data.h"

// #include "debug.h"

#include "timeout.h"

#include "usb_lib.h"
#include "usb_prop.h"

#include "hidconfig.h"

#define OUT_EP ENDP1

uint8_t HID_Buffer_OUT[64] = {0x00};
uint8_t HID_Buffer_IN[64]  = {0x00};

uint8_t HID_SGIO4_Buffer_IN[64] = {0x00};
uint8_t HID_KBD_Buffer_IN[18]   = {0x00};

static DataReceive *dataReceive = (DataReceive *)HID_Buffer_OUT;
static DataUpload *dataUpload   = (DataUpload *)HID_SGIO4_Buffer_IN;

static KbdData *kbdData = (KbdData *)HID_KBD_Buffer_IN;

static uint16_t prevKeyStatus    = 0;
static uint16_t activeKeyStatus  = 0;
static uint16_t changedKeyStatus = 0;

static uint16_t prevRollerValue   = 0;
static uint16_t activeRollerValue = 0;

uint8_t intervalID = 0xFF;

#define INTERVAL_HEARTBEAT_MS_IDLE   30
#define INTERVAL_HEARTBEAT_MS_ACTIVE 6

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
          intervalID = setInterval(HIDIO_SGIO4_Heartbeat, INTERVAL_HEARTBEAT_MS_ACTIVE);
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
  Sleep_Alive();
}

uint8_t HIDIO_SGIO4_Upload()
{
  memset(HID_Buffer_IN, 0, 64);
  memcpy(HID_Buffer_IN, HID_SGIO4_Buffer_IN, 64);
  return USBD_ENDPx_DataUp(OUT_EP, HID_Buffer_IN, 64);
}

uint8_t HIDIO_KBD_Upload()
{
  memset(HID_Buffer_IN, 0, 64);
  memcpy(HID_Buffer_IN, HID_KBD_Buffer_IN, 18);
  return USBD_ENDPx_DataUp(OUT_EP, HID_Buffer_IN, 18);
}

void HIDIO_Upload()
{
  static uint8_t timeoutID = 0xFF;
  uint8_t usbResult = 0xFF;
  switch (GlobalData->DeviceMode) {
    case 1:
      resetInterval(intervalID);
      usbResult = HIDIO_SGIO4_Upload();
      break;
    case 2:
      SP_INPUT_OnDataUpdate_Handler();
      usbResult = USB_SUCCESS;
      break;
    case 3:
      usbResult = HIDIO_KBD_Upload();
      break;
    default:
      break;
  }
  if (usbResult != USB_SUCCESS) {
    clearTimeout(timeoutID);
    timeoutID = setTimeout(HIDIO_Upload, 0); // 重试上传
  }
}

void HIDIO_SGIO4_Heartbeat()
{
  if (GlobalData->DeviceMode == 1) {
    HIDIO_SGIO4_Upload();
  }
}

void HIDIO_SGIO4_FreshData()
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
}

void HIDIO_KBD_FreshData()
{
  // Roller
  // dataUpload->analog[0] = activeRollerValue;

  // Buttons
  memset(kbdData, 0x00, sizeof(KbdData));
  kbdData->reportID = 0x31;

  for (uint8_t i = 0; i < KEY_COUNT; i++) {
    if (i == 0x04 || i == 0x09) {
      // RSide 和 LSide 需要特殊处理
      continue;
    }
    if (KeyScan_GetKeyDebouncedStatus(i)) {
      kbdData->ctrlkey |= kbd_key_map[i][0];
      kbdData->keymap[i] = kbd_key_map[i][1];
    }
  }
  // RSide 和 LSide 特殊处理
  if (!KeyScan_GetKeyDebouncedStatus(0x04)) {
    kbdData->ctrlkey |= kbd_key_map[0x04][0];
    kbdData->keymap[0x04] = kbd_key_map[0x04][1];
  }
  if (!KeyScan_GetKeyDebouncedStatus(0x09)) {
    kbdData->ctrlkey |= kbd_key_map[0x09][0];
    kbdData->keymap[0x09] = kbd_key_map[0x09][1];
  }
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
    switch (GlobalData->DeviceMode) {
      case 1:
        HIDIO_SGIO4_FreshData();
        break;
      case 2:
        break;
      case 3:
        HIDIO_KBD_FreshData();
        break;
      default:
        break;
    }
    HIDIO_Upload();
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

  HIDIO_SGIO4_FreshData();

  clearInterval(intervalID);
  intervalID = setInterval(HIDIO_SGIO4_Heartbeat, INTERVAL_HEARTBEAT_MS_IDLE);
}
