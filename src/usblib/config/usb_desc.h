/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : This file contains all the functions prototypes for the
 *                      USB description firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __USB_DESC_H
#define __USB_DESC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v20x.h"

#define VENDOR_ID_H                       0x0C
#define VENDOR_ID_L                       0xA3
#define PRODUCT_ID_H                      0x00
#define PRODUCT_ID_L                      0x21

#define PRODUCT_BCD_H                     0x02
#define PRODUCT_BCD_L                     0x00

#define USB_DEVICE_DESCRIPTOR_TYPE        0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE 0x02
#define USB_STRING_DESCRIPTOR_TYPE        0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE     0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE      0x05

#define DEF_USBD_UEP0_SIZE                64
#define DEF_USBD_MAX_PACK_SIZE            64

#define USBD_SIZE_DEVICE_DESC             18
#define USBD_SIZE_CONFIG_DESC             107
#define USBD_SIZE_REPORT_DESC             113
#define USBD_SIZE_STRING_LANGID           4
#define USBD_SIZE_STRING_VENDOR           10
#define USBD_SIZE_STRING_PRODUCT          16
#define USBD_SIZE_STRING_SERIAL           22
#define USBD_SIZE_STRING_CONFIG           22
#define USBD_SIZE_STRING_HIDIO            188

extern const uint8_t USBD_DeviceDescriptor[USBD_SIZE_DEVICE_DESC];
extern const uint8_t USBD_ConfigDescriptor[USBD_SIZE_CONFIG_DESC];
extern const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID];
extern const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR];
extern const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT];
extern const uint8_t USBD_HidRepDesc[USBD_SIZE_REPORT_DESC];
extern uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL];
extern const uint8_t USBD_StringConfig[USBD_SIZE_STRING_CONFIG];
extern const uint8_t USBD_StringHIDIO[USBD_SIZE_STRING_HIDIO];

#ifdef __cplusplus
}
#endif

#endif /* __USB_DESC_H */
