/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_endp.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Endpoint routines
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_prop.h"

#include "cdc.h"

#include "hidio.h"
#include "hidconfig.h"

/*********************************************************************
 * @fn      EP1_IN_Callback
 *
 * @brief  Endpoint 1 IN.
 *
 * @return  none
 */
void EP1_IN_Callback(void)
{
}

/*********************************************************************
 * @fn      EP1_OUT_Callback
 *
 * @brief  Endpoint 1 OUT.
 *
 * @return  none
 */
void EP1_OUT_Callback(void)
{
  if (USB_SIL_Read(EP1_OUT, HID_Buffer_OUT) == ENDP1_PACKET_SIZE) {
    HIDIO_Receive_Handler();
  }
  SetEPRxValid(ENDP1);
}

/*********************************************************************
 * @fn      EP4_IN_Callback
 *
 * @brief  Endpoint 4 IN.
 *
 * @return  none
 */
void EP4_IN_Callback(void)
{
}

/*********************************************************************
 * @fn      EP4_OUT_Callback
 *
 * @brief  Endpoint 4 OUT.
 *
 * @return  none
 */
void EP4_OUT_Callback(void)
{
  if (USB_SIL_Read(EP4_OUT, HIDCFG_Buffer_OUT) == ENDP4_PACKET_SIZE) {
    HIDCONFIG_Receive_Handler();
  }
  SetEPRxValid(ENDP4);
}
/*********************************************************************
 * @fn      EP2_OUT_Callback
 *
 * @brief  Endpoint 2 OUT.
 *
 * @return  none
 */
void EP2_OUT_Callback(void)
{
  cdc_led_io.Rx_Pending = (uint8_t)USB_SIL_Read(EP2_OUT, cdc_led_io.Rx_PendingBuf);
  cdc_led_io.Rx_CurPos  = 0;

  SetEPRxStatus(CDC_LED_IO_EP, EP_RX_NAK);
}

/*********************************************************************
 * @fn      EP2_IN_Callback
 *
 * @brief  Endpoint 2 IN.
 *
 * @return  none
 */
void EP2_IN_Callback(void)
{
  if (_GetEPTxCount(CDC_LED_IO_EP) == ENDP2_PACKET_SIZE) {
    // 当还有数据未发送时，继续发送包
    if (cdc_led_io.Tx_Full || cdc_led_io.PutCharBuff_First != cdc_led_io.PutCharBuff_Last) {
      SetEPTxStatus(CDC_LED_IO_EP, EP_TX_NAK);
      CDC_Check();
    } else {
      // 发送一个ZLP 0长度包，通知包已发送完毕
      USB_SIL_Write(0x80 | CDC_LED_IO_EP, 0, 0);
    }
  } else {
    // 标记NAK状态，停止传输
    SetEPTxStatus(CDC_LED_IO_EP, EP_TX_NAK);
  }
}

/*********************************************************************
 * @fn      EP3_OUT_Callback
 *
 * @brief  Endpoint 3 OUT.
 *
 * @return  none
 */
void EP3_OUT_Callback(void)
{
  cdc_card_io.Rx_Pending = (uint8_t)USB_SIL_Read(EP3_OUT, cdc_card_io.Rx_PendingBuf);
  cdc_card_io.Rx_CurPos  = 0;

  SetEPRxStatus(CDC_CARD_IO_EP, EP_RX_NAK);
}

/*********************************************************************
 * @fn      EP3_IN_Callback
 *
 * @brief  Endpoint 3 IN.
 *
 * @return  none
 */
void EP3_IN_Callback(void)
{
  if (_GetEPTxCount(CDC_CARD_IO_EP) == ENDP3_PACKET_SIZE) {
    // 当还有数据未发送时，继续发送包
    if (cdc_card_io.Tx_Full || cdc_card_io.PutCharBuff_First != cdc_card_io.PutCharBuff_Last) {
      SetEPTxStatus(CDC_CARD_IO_EP, EP_TX_NAK);
      CDC_Check();
    } else {
      // 发送一个ZLP 0长度包，通知包已发送完毕
      USB_SIL_Write(0x80 | CDC_CARD_IO_EP, 0, 0);
    }
  } else {
    // 标记NAK状态，停止传输
    SetEPTxStatus(CDC_CARD_IO_EP, EP_TX_NAK);
  }
}

/*********************************************************************
 * @fn      USBD_ENDPx_DataUp
 *
 * @brief  USBD ENDPx DataUp Function
 *
 * @param   endp - endpoint num.
 *          *pbuf - A pointer points to data.
 *          len - data length to transmit.
 *
 * @return  data up status.
 */
uint8_t USBD_ENDPx_DataUp(uint8_t endp, uint8_t *pbuf, uint16_t len)
{
  switch (endp) {
    case ENDP1: {
      if (GetEPTxStatus(ENDP1) == EP_TX_VALID) {
        return USB_ERROR;
      }
      USB_SIL_Write(EP1_IN, pbuf, len);
      SetEPTxStatus(ENDP1, EP_TX_VALID);
      break;
    }
    case ENDP2: {
      if (GetEPTxStatus(ENDP2) == EP_TX_VALID) {
        return USB_ERROR;
      }
      USB_SIL_Write(EP2_IN, pbuf, len);
      SetEPTxStatus(ENDP2, EP_TX_VALID);
      break;
    }
    case ENDP3: {
      if (GetEPTxStatus(ENDP3) == EP_TX_VALID) {
        return USB_ERROR;
      }
      USB_SIL_Write(EP3_IN, pbuf, len);
      SetEPTxStatus(ENDP3, EP_TX_VALID);
      break;
    }
    case ENDP4: {
      if (GetEPTxStatus(ENDP4) == EP_TX_VALID) {
        return USB_ERROR;
      }
      USB_SIL_Write(EP4_IN, pbuf, len);
      SetEPTxStatus(ENDP4, EP_TX_VALID);
      break;
    }
    default:
      return USB_ERROR;
  }
  return USB_SUCCESS;
}
