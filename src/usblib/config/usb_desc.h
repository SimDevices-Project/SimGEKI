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

#define USB_IAD_DESCRIPTOR_TYPE           0x0B

#define DEF_USBD_UEP0_SIZE                16
#define DEF_USBD_MAX_PACK_SIZE            64

#define USBD_SIZE_DEVICE_DESC             18
#define USBD_SIZE_CONFIG_DESC             205

#define USBD_SIZE_REPORT_DESC             113
#define USBD_SIZE_CUSTOM_REPORT_DESC      27

#define USBD_NUMOF_REPORT_DESC            2

#define USBD_NUMOF_STRING_DESC            9

#define USBD_SIZE_STRING_LANGID           16
#define USBD_SIZE_STRING_VENDOR           10
#define USBD_SIZE_STRING_PRODUCT          38
#define USBD_SIZE_STRING_SERIAL           26
#define USBD_SIZE_STRING_CONFIG           22
#define USBD_SIZE_STRING_HIDIO            186
#define USBD_SIZE_STRING_LEDIO            30
#define USBD_SIZE_STRING_CARDIO           34
#define USBD_SIZE_STRING_CUSTOM_HID       30

#define USBD_SIZE_STRING_PRODUCT_zh_hans  30
#define USBD_SIZE_STRING_LEDIO_zh_hans    20
#define USBD_SIZE_STRING_CARDIO_zh_hans   24
#define USBD_SIZE_STRING_CUSTOM_HID_zh_hans 20

extern uint8_t USBD_DeviceDescriptor[USBD_SIZE_DEVICE_DESC];
extern const uint8_t USBD_ConfigDescriptor[USBD_SIZE_CONFIG_DESC];

extern const uint8_t USBD_HidRepDesc[USBD_SIZE_REPORT_DESC];
extern const uint8_t USBD_HidCustomDesc[USBD_SIZE_CUSTOM_REPORT_DESC];

// Not for muti-language
extern const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID];
extern const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR];
extern uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL];

// For muti-language
extern const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT];
extern const uint8_t USBD_StringProduct_zh_hans[USBD_SIZE_STRING_PRODUCT_zh_hans];

extern const uint8_t USBD_StringConfig[USBD_SIZE_STRING_CONFIG];

extern const uint8_t USBD_StringHIDIO[USBD_SIZE_STRING_HIDIO];

extern const uint8_t USBD_StringLEDIO[USBD_SIZE_STRING_LEDIO];
extern const uint8_t USBD_StringLEDIO_zh_hans[USBD_SIZE_STRING_LEDIO_zh_hans];

extern const uint8_t USBD_StringCardIO[USBD_SIZE_STRING_CARDIO];
extern const uint8_t USBD_StringCardIO_zh_hans[USBD_SIZE_STRING_CARDIO_zh_hans];

extern const uint8_t USBD_StringCustomHID[USBD_SIZE_STRING_CUSTOM_HID];
extern const uint8_t USBD_StringCustomHID_zh_hans[USBD_SIZE_STRING_CUSTOM_HID_zh_hans];

void USBD_SerialNumUpdate();
void USBD_DeviceDescriptorUpdate();

#ifdef __cplusplus
}
#endif

#endif /* __USB_DESC_H */
