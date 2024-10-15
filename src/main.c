/*
 * SimGEKI
 * Copyright (c) 2024 SimDevices, Handle
 */

#include "bsp.h"
#include "timeout.h"

#include "data.h"

#include "led.h"
#include "ch422.h"
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

  // USART_Printf_Init(115200);

  // RCC_Configuration();
  // TIM2_Init();
  // UART2_Init(1, DEF_UARTx_BAUDRATE, DEF_UARTx_STOPBIT, DEF_UARTx_PARITY);

  Set_USBConfig();
  USB_Init();
  USB_Interrupts_Config();

  Delay_Ms(50);

  LoadData();

  LED_Init();
  CH422_Init();
  HIDIO_Init();
  CDC_Init();
  PN532_Init();

  CH422_Set(0b100010001100010001);
  LED_RGB_Set(RGB_PORT_LEFT, 0, 0xFF, 0xFF, 0xFF);
  LED_RGB_Set(RGB_PORT_RIGHT, 0, 0xFF, 0xFF, 0xFF);

  LED_RGB_Set(RGB_PORT_UART, 0, 0xFF, 0xFF, 0xFF);

  // setRgbColorPort(1, 0xff, 0, 0);
  // LED_RGB_Refresh();

  // setRgbColor(2, 0, 0x00, 0x00, 0xFF);

  setInterval(CH422_Refresh, 20);
  setInterval(LED_RGB_Refresh, 20);
  setInterval(HIDIO_Update, 1);
  setInterval(CDC_Check, 3);

  setInterval(HIDIO_Upload, 1500);
  while (1) {
    Timer_Process();

    // Delay_Us(100); // Do Something

    // LED_RGB_Refresh();
    // CH422_Check();

    // UART2_DataRx_Deal();
    // UART2_DataTx_Deal();
  }
}
