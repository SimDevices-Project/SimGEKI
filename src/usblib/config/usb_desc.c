/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2019/10/15
 * Description        : USB Descriptors.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "usb_lib.h"
#include "usb_desc.h"

/* USB Device Descriptors */
const uint8_t USBD_DeviceDescriptor[] = {
    USBD_SIZE_DEVICE_DESC,        // bLength
    0x01,                         // bDescriptorType
    0x10, 0x01,                   // bcdUSB
    0xEF,                         // bDeviceClass
    0x02,                         // bDeviceSubClass
    0x01,                         // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,           // bMaxPacketSize0
    VENDOR_ID_L, VENDOR_ID_H,     // 厂商ID
    PRODUCT_ID_L, PRODUCT_ID_H,   // 产品ID
    PRODUCT_BCD_L, PRODUCT_BCD_H, // 设备版本号
    0x01,                         // iManufacturer
    0x02,                         // iProduct
    0x03,                         // iSerialNumber
    0x01,                         // bNumConfigurations
};

/* USB Configration Descriptors */
const uint8_t USBD_ConfigDescriptor[USBD_SIZE_CONFIG_DESC] = {
    /* Configuration Descriptor */
    0x09,                                                     // bLength
    0x02,                                                     // bDescriptorType
    USBD_SIZE_CONFIG_DESC & 0xFF, USBD_SIZE_CONFIG_DESC >> 8, // wTotalLength
    0x03,                                                     // bNumInterfaces
    0x01,                                                     // bConfigurationValue
    0x04,                                                     // iConfiguration
    0x80,                                                     // bmAttributes: Bus Powered; Remote Wakeup
    0x32,                                                     // MaxPower: 100mA

    /* IAD Descriptor(interface 0/1)*/
    0x08, 0x0B, 0x00, 0x02, 0x02, 0x02, 0x01, 0x00,

    /* Interface 0 (CDC) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x00, // bInterfaceNumber 0
    0x00, // bAlternateSetting
    0x01, // bNumEndpoints 1
    0x02, // bInterfaceClass
    0x02, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x06, // iInterface (String Index)

    /* Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,

    /* Length/management descriptor (data class interface 1) */
    0x05, 0x24, 0x01, 0x00, 0x01,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x00, 0x01,

    /* Interrupt upload endpoint descriptor */
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x87,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize 64
    0x05,       // bInterval 5 (unit depends on device speed)

    /* Interface 1 (data interface) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x01, // bInterfaceNumber 1
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0x0A, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    /* Endpoint descriptor */
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x02,       // bEndpointAddress (OUT/H2D)
    0x02,       // bmAttributes (Bulk)
    0x40, 0x00, // wMaxPacketSize 64
    0x00,       // bInterval 0 (unit depends on device speed)

    /* Endpoint descriptor */
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x82,       // bEndpointAddress (IN/D2H)
    0x02,       // bmAttributes (Bulk)
    0x40, 0x00, // wMaxPacketSize 64
    0x00,       // bInterval 0 (unit depends on device speed)

    /* interface 2 (HID interface) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x02, // bInterfaceNumber 2
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0x03, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x05, // iInterface (String Index)

    /* interface 2 HID descriptor */
    0x09,                                                     // bLength
    0x21,                                                     // bDescriptorType
    0x11, 0x01,                                               // bcdHID
    0x00,                                                     // bCountryCode
    0x01,                                                     // bNumDescriptors
    0x22,                                                     // bDescriptorType
    USBD_SIZE_REPORT_DESC & 0xFF, USBD_SIZE_REPORT_DESC >> 8, // wDescriptorLength

    /* interface 2 endpoint descriptor*/
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x81,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize 64
    0x01,       // bInterval 1 (unit depends on device speed)

    /* interface 2 endpoint descriptor */
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x01,       // bEndpointAddress (OUT/H2D)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize 64
    0x05,       // bInterval 5 (unit depends on device speed)

};

/* USB String Descriptors */
const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID] = {
    USBD_SIZE_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04};

/* USB Device String Vendor */
const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR] = {
    USBD_SIZE_STRING_VENDOR,
    USB_STRING_DESCRIPTOR_TYPE,
    'S', 0, 'E', 0, 'G', 0, 'A', 0};

/* USB Device String Product */
const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT] = {
    USBD_SIZE_STRING_PRODUCT,
    USB_STRING_DESCRIPTOR_TYPE,
    'S', 0, 'i', 0, 'm', 0, 'G', 0, 'E', 0, 'K', 0, 'I', 0};

/* USB Device String Serial */
uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL] = {
    USBD_SIZE_STRING_SERIAL,
    USB_STRING_DESCRIPTOR_TYPE,
    '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0};

const uint8_t USBD_StringConfig[USBD_SIZE_STRING_CONFIG] = {
    USBD_SIZE_STRING_CONFIG,
    USB_STRING_DESCRIPTOR_TYPE,
    'S', 0, 'i', 0, 'm', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, 's', 0};

const uint8_t USBD_StringHIDIO[USBD_SIZE_STRING_HIDIO] = {
    USBD_SIZE_STRING_HIDIO,
    USB_STRING_DESCRIPTOR_TYPE,
    'I', 0, '/', 0, 'O', 0, ' ', 0, 'C', 0, 'O', 0, 'N', 0, 'T', 0, 'R', 0, 'O', 0, 'L', 0, ' ', 0, 'B', 0, 'D', 0, ';', 0, '1', 0, '5', 0, '2', 0, '5', 0, '7', 0, ';', 0, '0', 0, '1', 0, ';', 0, '9', 0, '0', 0, ';', 0, '1', 0, '8', 0, '3', 0, '1', 0, ';', 0, '6', 0, '6', 0, '7', 0, '9', 0, 'A', 0, ';', 0, '0', 0, '0', 0, ';', 0, 'G', 0, 'O', 0, 'U', 0, 'T', 0, '=', 0, '1', 0, '4', 0, '_', 0, 'A', 0, 'D', 0, 'I', 0, 'N', 0, '=', 0, '8', 0, ',', 0, 'E', 0, '_', 0, 'R', 0, 'O', 0, 'T', 0, 'I', 0, 'N', 0, '=', 0, '4', 0, '_', 0, 'C', 0, 'O', 0, 'I', 0, 'N', 0, 'I', 0, 'N', 0, '=', 0, '2', 0, '_', 0, 'S', 0, 'W', 0, 'I', 0, 'N', 0, '=', 0, '2', 0, ',', 0, 'E', 0, '_', 0, 'U', 0, 'Q', 0, '1', 0, '=', 0, '4', 0, '1', 0, ',', 0, '6', 0, ';', 0};

const uint8_t USBD_StringLEDIO[USBD_SIZE_STRING_LEDIO] = {
    USBD_SIZE_STRING_LEDIO,
    USB_STRING_DESCRIPTOR_TYPE,
    'L', 0, 'E', 0, 'D', 0, ' ', 0, 'B', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0, 'C', 0, 'O', 0, 'M', 0, '3', 0};

/* HID Report Descriptor */
const uint8_t USBD_HidRepDesc[USBD_SIZE_REPORT_DESC] =
    {
        0x05, 0x01,                   // Usage Page (Generic Desktop Ctrls)
        0x09, 0x04,                   // Usage (Joystick)
        0xA1, 0x01,                   // Collection (Application)
        0x85, 0x01,                   //   Report ID (1)
        0x09, 0x01,                   //   Usage (Pointer)
        0xA1, 0x00,                   //   Collection (Physical)
        0x09, 0x30,                   //     Usage (X)
        0x09, 0x31,                   //     Usage (Y)
        0x09, 0x30,                   //     Usage (X)
        0x09, 0x31,                   //     Usage (Y)
        0x09, 0x30,                   //     Usage (X)
        0x09, 0x31,                   //     Usage (Y)
        0x09, 0x30,                   //     Usage (X)
        0x09, 0x31,                   //     Usage (Y)
        0x09, 0x33,                   //     Usage (Rx)
        0x09, 0x34,                   //     Usage (Ry)
        0x09, 0x33,                   //     Usage (Rx)
        0x09, 0x34,                   //     Usage (Ry)
        0x09, 0x36,                   //     Usage (Slider)
        0x09, 0x36,                   //     Usage (Slider)
        0x15, 0x00,                   //     Logical Minimum (0)
        0x27, 0xFF, 0xFF, 0x00, 0x00, //     Logical Maximum (65535)
        0x35, 0x00,                   //     Physical Minimum (0)
        0x47, 0xFF, 0xFF, 0x00, 0x00, //     Physical Maximum (65535)
        0x95, 0x0E,                   //     Report Count (14)
        0x75, 0x10,                   //     Report Size (16)
        0x81, 0x02,                   //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,                         //   End Collection
        0x05, 0x02,                   //   Usage Page (Sim Ctrls)
        0x05, 0x09,                   //   Usage Page (Button)
        0x19, 0x01,                   //   Usage Minimum (0x01)
        0x29, 0x30,                   //   Usage Maximum (0x30)
        0x15, 0x00,                   //   Logical Minimum (0)
        0x25, 0x01,                   //   Logical Maximum (1)
        0x45, 0x01,                   //   Physical Maximum (1)
        0x75, 0x01,                   //   Report Size (1)
        0x95, 0x30,                   //   Report Count (48)
        0x81, 0x02,                   //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x09, 0x00,                   //   Usage (0x00)
        0x75, 0x08,                   //   Report Size (8)
        0x95, 0x1D,                   //   Report Count (29)
        0x81, 0x01,                   //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x06, 0xA0, 0xFF,             //   Usage Page (Vendor Defined 0xFFA0)
        0x09, 0x00,                   //   Usage (0x00)
        0x85, 0x10,                   //   Report ID (16)
        0xA1, 0x01,                   //   Collection (Application)
        0x09, 0x00,                   //     Usage (0x00)
        0x15, 0x00,                   //     Logical Minimum (0)
        0x26, 0xFF, 0x00,             //     Logical Maximum (255)
        0x75, 0x08,                   //     Report Size (8)
        0x95, 0x3F,                   //     Report Count (63)
        0x91, 0x02,                   //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0xC0,                         //   End Collection
        0xC0,                         // End Collection
};
