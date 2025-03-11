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
#include "usb_conf.h"

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
    0x06,                                                     // bNumInterfaces
    0x01,                                                     // bConfigurationValue
    0x04,                                                     // iConfiguration
    0x80,                                                     // bmAttributes: Bus Powered; Remote Wakeup
    0xfa,                                                     // MaxPower: 500mA

    /* IAD Descriptor(interface 0/1)*/
    8,                       // Length of the descriptor 描述符长度
    USB_IAD_DESCRIPTOR_TYPE, // Type: Interface Association Descriptor (IAD) 描述符类型：接口关联描述符
    0x00,                    // First interface: 2 in this case, see following 第一个要关联的接口ID
    0x02,                    // Total number of grouped interfaces 总共要关联的接口数量
    0x02,                    // bFunctionClass
    0x02,                    // bFunctionSubClass
    0x00,                    // bFunctionProtocol
    0x06,                    // Index of string descriptor describing this function 字符串描述符索引

    /* Interface 0 (CDC) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x00, // bInterfaceNumber 0
    0x00, // bAlternateSetting
    0x01, // bNumEndpoints 1
    0x02, // bInterfaceClass
    0x02, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    /* Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,

    /* Length/management descriptor (data class interface 1) */
    0x05, 0x24, 0x01, 0x00, 0x01,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x00, 0x01,

    /* Interrupt upload endpoint descriptor */
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x86,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize 64
    0x40,       // bInterval 5 (unit depends on device speed)

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
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x02,                    // bEndpointAddress (OUT/H2D)
    0x02,                    // bmAttributes (Bulk)
    ENDP2_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x00,                    // bInterval 0 (unit depends on device speed)

    /* Endpoint descriptor */
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x82,                    // bEndpointAddress (IN/D2H)
    0x02,                    // bmAttributes (Bulk)
    ENDP2_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x00,                    // bInterval 0 (unit depends on device speed)

    /* IAD Descriptor(interface 2/3)*/
    8,                       // Length of the descriptor 描述符长度
    USB_IAD_DESCRIPTOR_TYPE, // Type: Interface Association Descriptor (IAD) 描述符类型：接口关联描述符
    0x02,                    // First interface: 2 in this case, see following 第一个要关联的接口ID
    0x02,                    // Total number of grouped interfaces 总共要关联的接口数量
    0x02,                    // bFunctionClass
    0x02,                    // bFunctionSubClass
    0x00,                    // bFunctionProtocol
    0x07,                    // Index of string descriptor describing this function 字符串描述符索引

    /* Interface 2 (CDC) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x02, // bInterfaceNumber 2
    0x00, // bAlternateSetting
    0x01, // bNumEndpoints 1
    0x02, // bInterfaceClass
    0x02, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    /* Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,

    /* Length/management descriptor (data class interface 3) */
    0x05, 0x24, 0x01, 0x00, 0x03,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x02, 0x03,

    /* Interrupt upload endpoint descriptor */
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x87,       // bEndpointAddress (IN/D2H)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize 64
    0x40,       // bInterval 5 (unit depends on device speed)

    /* Interface 3 (data interface) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x03, // bInterfaceNumber 3
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0x0A, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    /* Endpoint descriptor */
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x03,                    // bEndpointAddress (OUT/H2D)
    0x02,                    // bmAttributes (Bulk)
    ENDP3_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x00,                    // bInterval 0 (unit depends on device speed)

    /* Endpoint descriptor */
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x83,                    // bEndpointAddress (IN/D2H)
    0x02,                    // bmAttributes (Bulk)
    ENDP3_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x00,                    // bInterval 0 (unit depends on device speed)

    /* interface 4 (HID interface) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x04, // bInterfaceNumber 4
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0x03, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x05, // iInterface (String Index)

    /* interface 4 HID descriptor */
    0x09,                                                     // bLength
    0x21,                                                     // bDescriptorType
    0x11, 0x01,                                               // bcdHID
    0x00,                                                     // bCountryCode
    0x01,                                                     // bNumDescriptors
    0x22,                                                     // bDescriptorType
    USBD_SIZE_REPORT_DESC & 0xFF, USBD_SIZE_REPORT_DESC >> 8, // wDescriptorLength

    /* interface 4 endpoint descriptor*/
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x81,                    // bEndpointAddress (IN/D2H)
    0x03,                    // bmAttributes (Interrupt)
    ENDP1_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x01,                    // bInterval 1 (unit depends on device speed)

    /* interface 4 endpoint descriptor */
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x01,                    // bEndpointAddress (OUT/H2D)
    0x03,                    // bmAttributes (Interrupt)
    ENDP1_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x05,                    // bInterval 5 (unit depends on device speed)

    /* interface 5 (HID interface) descriptor */
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x05, // bInterfaceNumber 5
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0x03, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x08, // iInterface (String Index)

    /* interface 5 HID descriptor */
    0x09,                                                                   // bLength
    0x21,                                                                   // bDescriptorType
    0x11, 0x01,                                                             // bcdHID
    0x00,                                                                   // bCountryCode
    0x01,                                                                   // bNumDescriptors
    0x22,                                                                   // bDescriptorType
    USBD_SIZE_CUSTOM_REPORT_DESC & 0xFF, USBD_SIZE_CUSTOM_REPORT_DESC >> 8, // wDescriptorLength

    /* interface 5 endpoint descriptor ep4_IN*/
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x84,                    // bEndpointAddress (IN/D2H)
    0x03,                    // bmAttributes (Interrupt)
    ENDP4_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x05,                    // bInterval 5 (unit depends on device speed)

    /* interface 5 endpoint descriptor ep4_OUT*/
    0x07,                    // bLength
    0x05,                    // bDescriptorType (Endpoint)
    0x04,                    // bEndpointAddress (OUT/H2D)
    0x03,                    // bmAttributes (Interrupt)
    ENDP4_PACKET_SIZE, 0x00, // wMaxPacketSize 64
    0x05,                    // bInterval 5 (unit depends on device speed)

};

/* USB String Descriptors */
const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID] = {
    USBD_SIZE_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x04, 0x08, // LangID = 0x0804: Chinese (Simplified, China Mainland)
    0x04, 0x10, // LangID = 0x1004: Chinese (Simplified, Singapore)
    0x04, 0x0C, // LangID = 0x0C04: Chinese (Traditional, HK)
    0x04, 0x04, // LangID = 0x0404: Chinese (Traditional, Taiwan)
    0x04, 0x14, // LangID = 0x1404: Chinese (Traditional, Macau)
    0x11, 0x04, // LangID = 0x0411: Japanese (Japan)
    0x09, 0x04, // LangID = 0x0409: English (United States)
};

/* USB Device String Vendor */
const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR] = {
    USBD_SIZE_STRING_VENDOR,
    USB_STRING_DESCRIPTOR_TYPE,
    'S', 0, 'E', 0, 'G', 0, 'A', 0};

/* USB Device String Product */
const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT] = {
    USBD_SIZE_STRING_PRODUCT,
    USB_STRING_DESCRIPTOR_TYPE,
    0x53, 0x00, 0x69, 0x00, 0x6d, 0x00, 0x47, 0x00, 0x45, 0x00, 0x4b, 0x00, 0x49, 0x00, 0x20, 0x00, 0x43, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x74, 0x00, 0x72, 0x00, 0x6f, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x65, 0x00, 0x72, 0x00};

const uint8_t USBD_StringProduct_zh_hans[USBD_SIZE_STRING_PRODUCT_zh_hans] = {
    USBD_SIZE_STRING_PRODUCT_zh_hans,
    USB_STRING_DESCRIPTOR_TYPE,
    0x53, 0x00, 0x69, 0x00, 0x6d, 0x00, 0x47, 0x00, 0x45, 0x00, 0x4b, 0x00, 0x49, 0x00, 0x57, 0x88, 0x3a, 0x67, 0xce, 0x98, 0x3c, 0x68, 0xa7, 0x63, 0x36, 0x52, 0x68, 0x56};

/* USB Device String Serial */
uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL] = {
    USBD_SIZE_STRING_SERIAL,
    USB_STRING_DESCRIPTOR_TYPE,
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0};

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

const uint8_t USBD_StringLEDIO_zh_hans[USBD_SIZE_STRING_LEDIO_zh_hans] = {
    USBD_SIZE_STRING_LEDIO_zh_hans,
    USB_STRING_DESCRIPTOR_TYPE,
    0xbe, 0x8b, 0x07, 0x59, 0x6f, 0x70, 0x49, 0x51, 0x20, 0x00, 0x43, 0x00, 0x4f, 0x00, 0x4d, 0x00, 0x33, 0x00};

const uint8_t USBD_StringCardIO[USBD_SIZE_STRING_CARDIO] = {
    USBD_SIZE_STRING_CARDIO,
    USB_STRING_DESCRIPTOR_TYPE,
    'C', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0, 'R', 0, 'e', 0, 'a', 0, 'd', 0, 'e', 0, 'r', 0, ' ', 0, 'C', 0, 'O', 0, 'M', 0, '1', 0};

const uint8_t USBD_StringCardIO_zh_hans[USBD_SIZE_STRING_CARDIO_zh_hans] = {
    USBD_SIZE_STRING_CARDIO_zh_hans,
    USB_STRING_DESCRIPTOR_TYPE,
    0x4e, 0x00, 0x46, 0x00, 0x43, 0x00, 0xfb, 0x8b, 0x61, 0x53, 0x68, 0x56, 0x20, 0x00, 0x43, 0x00, 0x4f, 0x00, 0x4d, 0x00, 0x31, 0x00};

const uint8_t USBD_StringCustomHID[USBD_SIZE_STRING_CUSTOM_HID] = {
    USBD_SIZE_STRING_CUSTOM_HID,
    USB_STRING_DESCRIPTOR_TYPE,
    'S', 0, 'i', 0, 'm', 0, 'G', 0, 'E', 0, 'K', 0, 'I', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0};

const uint8_t USBD_StringCustomHID_zh_hans[USBD_SIZE_STRING_CUSTOM_HID_zh_hans] = {
    USBD_SIZE_STRING_CUSTOM_HID_zh_hans,
    USB_STRING_DESCRIPTOR_TYPE,
    0x53, 0x00, 0x69, 0x00, 0x6d, 0x00, 0x47, 0x00, 0x45, 0x00, 0x4b, 0x00, 0x49, 0x00, 0xbe, 0x8b, 0x6e, 0x7f};

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

const uint8_t USBD_HidCustomDesc[USBD_SIZE_CUSTOM_REPORT_DESC] = {
    0x06, 0x00, 0xFF, // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,       // Usage (Vendor Usage 1)
    0xA1, 0x01,       // Collection (Application)
    0x85, 0xAA,       // Report ID (170 0xAA)
    0x15, 0x00,       // Logical Minimum (0)
    0x26, 0xFF, 0x00, // Logical Maximum (255)
    0x75, 0x08,       // Report Size (8)
    0x95, 0x3F,       // Report Count (63)
    0x09, 0x02,       // Usage (Vendor Usage 2)
    0x81, 0x02,       // Input (Data,Var,Abs)
    0x09, 0x03,       // Usage (Vendor Usage 3)
    0x91, 0x02,       // Output (Data,Var,Abs)
    0xC0              // End Collection
};
void _IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
  for (uint8_t idx = 0; idx < len; idx++) {
    if (((value >> 28)) < 0xA) {
      pbuf[2 * idx] = (value >> 28) + '0';
    } else {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }
    value             = value << 4;
    pbuf[2 * idx + 1] = 0;
  }
}

void USBD_SerialNumUpdate()
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  Device_Serial0 = *(uint32_t *)0x1FFFF7E8;
  Device_Serial1 = *(uint32_t *)0x1FFFF7EC;
  Device_Serial2 = *(uint32_t *)0x1FFFF7F0;

  if (Device_Serial0 != 0) {
    _IntToUnicode(Device_Serial0, &USBD_StringSerial[2], 4);
    _IntToUnicode(Device_Serial1, &USBD_StringSerial[10], 4);
    _IntToUnicode(Device_Serial2, &USBD_StringSerial[18], 4);
  }
}