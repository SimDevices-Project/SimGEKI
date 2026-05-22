#include "led.h"
#include "keyscan.h"
#include "data.h"

void LEDManager_Handle()
{
  if (GlobalData->LEDManager == 0) {
    return;
  }
  switch (GlobalData->DeviceMode) {
    case 1:
    case 2:
      return;
    case 3: {
      // LA
      if (KeyScan_GetKeyDebouncedStatus(1) == 1) {
        LED_7C_Set(LED_7C_L1, LED_ON, LED_ON, LED_ON);
      } else {
        LED_7C_Set(LED_7C_L1, LED_ON, LED_OFF, LED_OFF);
      }
      // LB
      if (KeyScan_GetKeyDebouncedStatus(2) == 1) {
        LED_7C_Set(LED_7C_L2, LED_ON, LED_ON, LED_ON);
      } else {
        LED_7C_Set(LED_7C_L2, LED_OFF, LED_ON, LED_OFF);
      }
      // LC
      if (KeyScan_GetKeyDebouncedStatus(3) == 1) {
        LED_7C_Set(LED_7C_L3, LED_ON, LED_ON, LED_ON);
      } else {
        LED_7C_Set(LED_7C_L3, LED_OFF, LED_OFF, LED_ON);
      }
      // RSide
      if (KeyScan_GetKeyDebouncedStatus(4) == 0) {
        LED_RGB_SetPort(LED_RGB_PORT_RIGHT, 0xFF, 0xFF, 0xFF);
      } else {
        LED_RGB_SetPort(LED_RGB_PORT_RIGHT, 0xFF, 0x00, 0xFF);
      }
      // RA
      if (KeyScan_GetKeyDebouncedStatus(6) == 1) {
        LED_7C_Set(LED_7C_R1, LED_ON, LED_ON, LED_ON);
      } else {
        LED_7C_Set(LED_7C_R1, LED_ON, LED_OFF, LED_OFF);
      }
      // RB
      if (KeyScan_GetKeyDebouncedStatus(7) == 1) {
        LED_7C_Set(LED_7C_R2, LED_ON, LED_ON, LED_ON);
      } else {
        LED_7C_Set(LED_7C_R2, LED_OFF, LED_ON, LED_OFF);
      }
      // RC
      if (KeyScan_GetKeyDebouncedStatus(8) == 1) {
        LED_7C_Set(LED_7C_R3, LED_ON, LED_ON, LED_ON);
      } else {
        LED_7C_Set(LED_7C_R3, LED_OFF, LED_OFF, LED_ON);
      }
      // LSide
      if (KeyScan_GetKeyDebouncedStatus(9) == 0) {
        LED_RGB_SetPort(LED_RGB_PORT_LEFT, 0xFF, 0xFF, 0xFF);
      } else {
        LED_RGB_SetPort(LED_RGB_PORT_LEFT, 0xFF, 0x00, 0xFF);
      }
      break;
    }
    default:
      break;
  }
}
