/*
 * SimGEKI
 * Copyright (c) 2024 SimDevices, Handle
 */

#include "bsp.h"
#include "timeout.h"

#include "data.h"

#include "led.h"
#include "hidio.h"
#include "cdc.h"
#include "pn532.h"

#include "usb_lib.h"

#include "debug.h"

int main(void)
{
  SystemInit();
  SystemCoreClockUpdate();

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  Delay_Init();
  Timeout_Init();

  Set_USBConfig();
  USB_Init();
  USB_Interrupts_Config();

  Delay_Ms(50);

  LoadData();

  LED_Init();
  HIDIO_Init();
  CDC_Init();
  PN532_Init();

  LED_7C_Set(LED_7C_L1, LED_ON, LED_OFF, LED_OFF);
  LED_7C_Set(LED_7C_L2, LED_OFF, LED_ON, LED_OFF);
  LED_7C_Set(LED_7C_L3, LED_OFF, LED_OFF, LED_ON);
  LED_7C_Set(LED_7C_R1, LED_ON, LED_OFF, LED_OFF);
  LED_7C_Set(LED_7C_R2, LED_OFF, LED_ON, LED_OFF);
  LED_7C_Set(LED_7C_R3, LED_OFF, LED_OFF, LED_ON);

  LED_RGB_Set(LED_RGB_PORT_LEFT, 0, 0xFF, 0x00, 0xFF);
  LED_RGB_Set(LED_RGB_PORT_RIGHT, 0, 0xFF, 0x00, 0xFF);

  LED_RGB_Set(LED_RGB_PORT_UART, 0, 0xFF, 0xFF, 0xFF);

  setInterval(LED_Refresh, 20);
  setInterval(HIDIO_Update, 1);
  setInterval(CDC_Check, 5);
  setInterval(PN532_Check, 30);

  while (1) {
    Timer_Process();
  }
}
