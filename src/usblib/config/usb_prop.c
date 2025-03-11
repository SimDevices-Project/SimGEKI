/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_prop.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : All processing related to Virtual Com Port Demo
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

#include "cdc.h"

uint8_t Request = 0;

volatile uint8_t HID_Idle_Value[2]     = {0};
volatile uint8_t HID_Protocol_Value[2] = {0};

DEVICE Device_Table =
    {
        EP_NUM,
        1};

DEVICE_PROP Device_Property =
    {
        USBD_init,
        USBD_Reset,
        USBD_Status_In,
        USBD_Status_Out,
        USBD_Data_Setup,
        USBD_NoData_Setup,
        USBD_Get_Interface_Setting,
        USBD_GetDeviceDescriptor,
        USBD_GetConfigDescriptor,
        USBD_GetStringDescriptor_MutiLang,
        0,
        DEF_USBD_UEP0_SIZE};

USER_STANDARD_REQUESTS User_Standard_Requests =
    {
        USBD_GetConfiguration,
        USBD_SetConfiguration,
        USBD_GetInterface,
        USBD_SetInterface,
        USBD_GetStatus,
        USBD_ClearFeature,
        USBD_SetEndPointFeature,
        USBD_SetDeviceFeature,
        USBD_SetDeviceAddress};

ONE_DESCRIPTOR Device_Descriptor =
    {
        (uint8_t *)USBD_DeviceDescriptor,
        USBD_SIZE_DEVICE_DESC};

ONE_DESCRIPTOR Config_Descriptor =
    {
        (uint8_t *)USBD_ConfigDescriptor,
        USBD_SIZE_CONFIG_DESC};

ONE_DESCRIPTOR String_Descriptor[USBD_NUMOF_STRING_DESC] =
    {
        {(uint8_t *)USBD_StringLangID, USBD_SIZE_STRING_LANGID},
        {(uint8_t *)USBD_StringVendor, USBD_SIZE_STRING_VENDOR},
        {(uint8_t *)USBD_StringProduct, USBD_SIZE_STRING_PRODUCT},
        {(uint8_t *)USBD_StringSerial, USBD_SIZE_STRING_SERIAL},
        {(uint8_t *)USBD_StringConfig, USBD_SIZE_STRING_CONFIG},
        {(uint8_t *)USBD_StringHIDIO, USBD_SIZE_STRING_HIDIO},
        {(uint8_t *)USBD_StringLEDIO, USBD_SIZE_STRING_LEDIO},
        {(uint8_t *)USBD_StringCardIO, USBD_SIZE_STRING_CARDIO},
        {(uint8_t *)USBD_StringCustomHID, USBD_SIZE_STRING_CUSTOM_HID},
};

MUTI_LANG_DESCRIPTOR Muti_Lang_String_Descriptor[USBD_NUMOF_STRING_DESC] =
    {
        {
            {(uint8_t *)USBD_StringLangID, USBD_SIZE_STRING_LANGID},
            {(uint8_t *)USBD_StringLangID, USBD_SIZE_STRING_LANGID},
            {(uint8_t *)USBD_StringLangID, USBD_SIZE_STRING_LANGID},
            {(uint8_t *)USBD_StringLangID, USBD_SIZE_STRING_LANGID},
        },
        {
            {(uint8_t *)USBD_StringVendor, USBD_SIZE_STRING_VENDOR},
            {NULL, 0},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringProduct, USBD_SIZE_STRING_PRODUCT},
            {(uint8_t *)USBD_StringProduct_zh_hans, USBD_SIZE_STRING_PRODUCT_zh_hans},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringSerial, USBD_SIZE_STRING_SERIAL},
            {NULL, 0},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringConfig, USBD_SIZE_STRING_CONFIG},
            {NULL, 0},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringHIDIO, USBD_SIZE_STRING_HIDIO},
            {NULL, 0},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringLEDIO, USBD_SIZE_STRING_LEDIO},
            {(uint8_t *)USBD_StringLEDIO_zh_hans, USBD_SIZE_STRING_LEDIO_zh_hans},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringCardIO, USBD_SIZE_STRING_CARDIO},
            {(uint8_t *)USBD_StringCardIO_zh_hans, USBD_SIZE_STRING_CARDIO_zh_hans},
            {NULL, 0},
            {NULL, 0},
        },
        {
            {(uint8_t *)USBD_StringCustomHID, USBD_SIZE_STRING_CUSTOM_HID},
            {(uint8_t *)USBD_StringCustomHID_zh_hans, USBD_SIZE_STRING_CUSTOM_HID_zh_hans},
            {NULL, 0},
            {NULL, 0},
        },
};

ONE_DESCRIPTOR Report_Descriptor[2] =
    {
        {(uint8_t *)USBD_HidRepDesc, USBD_SIZE_REPORT_DESC},
        {(uint8_t *)USBD_HidCustomDesc, USBD_SIZE_CUSTOM_REPORT_DESC},
};

ONE_DESCRIPTOR Hid_Descriptor[2] =
    {
        {(uint8_t *)&USBD_ConfigDescriptor[150], 0x09},
        {(uint8_t *)&USBD_ConfigDescriptor[182], 0x09},
};
/*********************************************************************
 * @fn      USBD_SetConfiguration.
 *
 * @brief     Update the device state to configured.
 *
 * @return    None.
 */
void USBD_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0) {
    bDeviceState = CONFIGURED;
  }
}

/*******************************************************************************
 * @fn         USBD_SetDeviceAddress.
 *
 * @brief      Update the device state to addressed.
 *
 * @return     None.
 */
void USBD_SetDeviceAddress(void)
{
  bDeviceState = ADDRESSED;
}

/*********************************************************************
 * @fn      USBD_SetDeviceFeature.
 *
 * @brief   SetDeviceFeature Routine.
 *
 * @return  none
 */
void USBD_SetDeviceFeature(void)
{
}

/*********************************************************************
 * @fn      USBD_ClearFeature.
 *
 * @brief   ClearFeature Routine.
 *
 * @return  none
 */
void USBD_ClearFeature(void)
{
}

/*********************************************************************
 * @fn      USBD_Status_In.
 *
 * @brief    USBD Status In Routine.
 *
 * @return   None.
 */
void USBD_Status_In(void)
{
  uint32_t Request_No = pInformation->USBbRequest;
  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
    if (Request_No == CDC_SET_LINE_CODING) {
      // UART2_USB_Init();
    }
  }
}

/*******************************************************************************
 * @fn       USBD_Status_Out
 *
 * @brief    USBD Status OUT Routine.
 *
 * @return   None.
 */
void USBD_Status_Out(void)
{
}

/*******************************************************************************
 * @fn       USBD_init.
 *
 * @brief    init routine.
 *
 * @return   None.
 */
void USBD_init(void)
{
  uint8_t i;

  pInformation->Current_Configuration = 0;
  PowerOn();
  for (i = 0; i < 8; i++) _SetENDPOINT(i, _GetENDPOINT(i) & 0x7F7F & EPREG_MASK); // all clear
  _SetISTR((uint16_t)0x00FF);                                                     // all clear
  USB_SIL_Init();
  bDeviceState = UNCONNECTED;

  USB_Port_Set(DISABLE, DISABLE);
  Delay_Ms(20);
  USB_Port_Set(ENABLE, ENABLE);
}

/*******************************************************************************
 * @fn      USBD_Reset
 *
 * @brief   USBD reset routine
 *
 * @return  none
 */
void USBD_Reset(void)
{
  pInformation->Current_Configuration = 0;
  pInformation->Current_Feature       = USBD_ConfigDescriptor[7];
  pInformation->Current_Interface     = 0;

  SetBTABLE(BTABLE_ADDRESS);

  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);
  _ClearDTOG_RX(ENDP0);
  _ClearDTOG_TX(ENDP0);

  SetEPType(ENDP1, EP_INTERRUPT);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  SetEPRxCount(ENDP1, DEF_USBD_MAX_PACK_SIZE);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  _ClearDTOG_TX(ENDP1);
  _ClearDTOG_RX(ENDP1);

  SetEPType(ENDP2, EP_BULK);
  SetEPTxStatus(ENDP2, EP_TX_NAK);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxAddr(ENDP2, ENDP2_RXADDR);
  SetEPRxCount(ENDP2, ENDP2_PACKET_SIZE);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  _ClearDTOG_RX(ENDP2);
  _ClearDTOG_TX(ENDP2);

  SetEPType(ENDP3, EP_BULK);
  SetEPTxStatus(ENDP3, EP_TX_NAK);
  SetEPTxAddr(ENDP3, ENDP3_TXADDR);
  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
  SetEPRxCount(ENDP3, ENDP3_PACKET_SIZE);
  SetEPRxStatus(ENDP3, EP_RX_VALID);
  _ClearDTOG_RX(ENDP3);
  _ClearDTOG_TX(ENDP3);

  SetEPType(ENDP4, EP_INTERRUPT);
  SetEPTxStatus(ENDP4, EP_TX_NAK);
  SetEPTxAddr(ENDP4, ENDP4_TXADDR);
  SetEPRxAddr(ENDP4, ENDP4_RXADDR);
  SetEPRxCount(ENDP4, DEF_USBD_MAX_PACK_SIZE);
  SetEPRxStatus(ENDP4, EP_RX_VALID);
  _ClearDTOG_TX(ENDP4);
  _ClearDTOG_RX(ENDP4);

  SetDeviceAddress(0);

  bDeviceState = ATTACHED;
}

/*********************************************************************
 * @fn      USBD_GetDeviceDescriptor.
 *
 * @brief   Gets the device descriptor.
 *
 * @param   Length.
 *
 * @return  The address of the device descriptor.
 */
uint8_t *USBD_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*********************************************************************
 * @fn      USBD_GetConfigDescriptor.
 *
 * @brief   get the configuration descriptor.
 *
 * @param   Length.
 *
 * @return   The address of the configuration descriptor.
 */
uint8_t *USBD_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*********************************************************************
 * @fn      USBD_GetStringDescriptor
 *
 * @brief   Gets the string descriptors according to the needed index
 *
 * @param   Length.
 *
 * @return    The address of the string descriptors.
 */
uint8_t *USBD_GetStringDescriptor(uint16_t Length)
{
  uint8_t stringIndex = pInformation->USBwValue0;

  if (stringIndex >= USBD_NUMOF_STRING_DESC) {
    return NULL;
  } else {
    return Standard_GetDescriptorData(Length, &String_Descriptor[stringIndex]);
  }
}

/*********************************************************************
 * @fn      USBD_GetStringDescriptor_MutiLang
 *
 * @brief   Gets the string descriptors according to the needed index & lang id
 *
 * @param   Length.
 *
 * @return    The address of the string descriptors.
 */
uint8_t *USBD_GetStringDescriptor_MutiLang(uint16_t Length)
{
  uint8_t stringIndex = pInformation->USBwValue0;
  uint16_t stringID   = pInformation->USBwIndex;

  if (stringIndex >= USBD_NUMOF_STRING_DESC) {
    return NULL;
  } else {
    switch (stringID) {
      case 0x0408:
      case 0x0410:
        // Chinese Simplified
        if (Muti_Lang_String_Descriptor[stringIndex].__zh_hans.Descriptor_Size == 0) {
          return Standard_GetDescriptorData(Length, &Muti_Lang_String_Descriptor[stringIndex].__en_us);
        }
        return Standard_GetDescriptorData(Length, &Muti_Lang_String_Descriptor[stringIndex].__zh_hans);
        break;
      case 0x0404:
      case 0x040c:
      case 0x0414:
        // Chinese Traditional
        if (Muti_Lang_String_Descriptor[stringIndex].__zh_hant.Descriptor_Size == 0) {
          return Standard_GetDescriptorData(Length, &Muti_Lang_String_Descriptor[stringIndex].__en_us);
        }
        return Standard_GetDescriptorData(Length, &Muti_Lang_String_Descriptor[stringIndex].__zh_hant);
        break;
      case 0x1104:
        // Japanese
        if (Muti_Lang_String_Descriptor[stringIndex].__ja_jp.Descriptor_Size == 0) {
          return Standard_GetDescriptorData(Length, &Muti_Lang_String_Descriptor[stringIndex].__en_us);
        }
        return Standard_GetDescriptorData(Length, &Muti_Lang_String_Descriptor[stringIndex].__ja_jp);
        break;
      case 0x0904:
        return Standard_GetDescriptorData(Length, &(Muti_Lang_String_Descriptor[stringIndex].__en_us));
        break;
      default:
        return Standard_GetDescriptorData(Length, &(Muti_Lang_String_Descriptor[stringIndex].__en_us));
    }
  }
}

/*********************************************************************
 * @fn      USBD_GetReportDescriptor
 *
 * @brief   Gets the report descriptors according to the needed index
 *
 * @param   Length.
 *
 * @return  The address of the device descriptor.
 */
uint8_t *USBD_GetReportDescriptor(uint16_t Length)
{
  uint8_t wIndex0 = pInformation->USBwIndex0;

  if (wIndex0 < 0x04 || wIndex0 >= USBD_NUMOF_REPORT_DESC + 0x04) {
    return NULL;
  } else {
    return Standard_GetDescriptorData(Length, &Report_Descriptor[wIndex0 - 0x04]);
  }
}

/*********************************************************************
 * @fn      USBD_GetHidDescriptor
 *
 * @brief   Gets the report descriptors according to the needed index
 *
 * @param   Length.
 *
 * @return  The address of the device descriptor.
 */
uint8_t *USBD_GetHidDescriptor(uint16_t Length)
{
  uint8_t wIndex0 = pInformation->USBwIndex0;

  if (wIndex0 < 0x04 || wIndex0 >= USBD_NUMOF_REPORT_DESC + 0x04) {
    return NULL;
  } else {
    return Standard_GetDescriptorData(Length, &Hid_Descriptor[wIndex0 - 0x04]);
  }
}

/*********************************************************************
 * @fn      USBD_Get_Interface_Setting.
 *
 * @brief   test the interface and the alternate setting according to the
 *          supported one.
 *
 * @param   Interface - interface number.
 *          AlternateSetting - Alternate Setting number.
 *
 * @return  USB_UNSUPPORT or USB_SUCCESS.
 */
RESULT USBD_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0) {
    return USB_UNSUPPORT;
  } else if (Interface > 1) {
    return USB_UNSUPPORT;
  }

  return USB_SUCCESS;
}

/*********************************************************************
 * @fn      USB_CDC_GetLineCoding.
 *
 * @brief   send the linecoding structure to the PC host.
 *
 * @param   Length
 *
 * @return  Inecoding structure base address.
 */
uint8_t *USB_CDC_GetLineCoding(uint16_t Length)
{
  if (Length == 0) {
    pInformation->Ctrl_Info.Usb_wLength = 7;
    return (NULL);
  }
  return (uint8_t *)&LineCoding[0];
}

/*********************************************************************
 * @fn      USB_CDC_SetLineCoding.
 *
 * @brief   Set the linecoding structure fields.
 *
 * @param   Length
 *
 * @return  Inecoding structure base address.
 */
uint8_t *USB_CDC_SetLineCoding(uint16_t Length)
{
  if (Length == 0) {
    pInformation->Ctrl_Info.Usb_wLength = 7;
    return (NULL);
  }
  return (uint8_t *)&LineCoding[0];
}

/*********************************************************************
 * @fn      USBD_Data_Setup
 *
 * @brief   handle the data class specific requests
 *
 * @param   Request Nb.
 *
 * @return  USB_UNSUPPORT or USB_SUCCESS.
 */
RESULT USBD_Data_Setup(uint8_t RequestNo)
{
  uint32_t Request_No;
  uint8_t *(*CopyRoutine)(uint16_t);
  uint8_t wValue1;
  Request_No  = pInformation->USBbRequest;
  CopyRoutine = NULL;
  wValue1     = pInformation->USBwValue1;
  if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {
    if (wValue1 == HID_REPORT_DESCRIPTOR) {
      CopyRoutine = USBD_GetReportDescriptor;
    } else if (wValue1 == HID_DESCRIPTOR) {
      CopyRoutine = USBD_GetHidDescriptor;
    }
    if (CopyRoutine) {
      pInformation->Ctrl_Info.CopyData = CopyRoutine;
      (*CopyRoutine)(0);
    } else {
      return USB_UNSUPPORT;
    }

  } else if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
    if (Request_No == CDC_GET_LINE_CODING) {
      CopyRoutine = &USB_CDC_GetLineCoding;
    } else if (Request_No == CDC_SET_LINE_CODING) {
      CopyRoutine = &USB_CDC_SetLineCoding;
    } else {
      return USB_UNSUPPORT;
    }
  }
  if (CopyRoutine) {
    pInformation->Ctrl_Info.CopyData    = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
  } else {
    return (USB_UNSUPPORT);
  }
  return USB_SUCCESS;
}

/*********************************************************************
 * @fn      USBD_NoData_Setup.
 *
 * @brief   handle the no data class specific requests.
 *
 * @param   Request Nb.
 *
 * @return  USB_UNSUPPORT or USB_SUCCESS.
 */
RESULT USBD_NoData_Setup(uint8_t RequestNo)
{
  uint32_t Request_No = pInformation->USBbRequest;

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
    if (Request_No == CDC_SET_LINE_CTLSTE) {

    } else if (Request_No == CDC_SEND_BREAK) {

    } else {
      return USB_UNSUPPORT;
    }
  }
  return USB_SUCCESS;
}
