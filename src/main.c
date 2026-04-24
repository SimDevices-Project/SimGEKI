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
#include "sleep.h"

#include "usb_lib.h"

#include "debug.h"

int main(void)
{
  SystemInit();
  SystemCoreClockUpdate();

  RCC_PCLK2Config(RCC_HCLK_Div2); // RCC2 时钟 2 分频

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  Delay_Init();

  Delay_Ms(50);

  LoadData();
  Set_USBConfig();
  USB_Interrupts_Config();
  USB_Init();

  Delay_Ms(10);

  Timeout_Init();
  Sleep_Init();

  LED_Init();
  HIDIO_Init();
  CDC_Init();
  PN532_Init();

  LED_Animation_PowerOn();

  setInterval(LED_Refresh, 20);
  setInterval(HIDIO_Update, 1);
  setInterval(CDC_Check, 5);
  setInterval(PN532_Check, 30);

  while (1) {
    Timer_Process();
  }
}
