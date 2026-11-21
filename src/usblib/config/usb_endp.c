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
#include "timeout.h"

static void EP1_OUT_Process(void)
{
  HIDIO_Receive_Handler();
  SetEPRxValid(ENDP1);
}

static void EP4_OUT_Process(void)
{
  HIDCONFIG_Receive_Handler();
  SetEPRxValid(ENDP4);
}

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
  // Keep OUT NAK until its queued handler has consumed the shared buffer.
  SetEPRxStatus(ENDP1, EP_RX_NAK);
  if (USB_SIL_Read(EP1_OUT, HID_Buffer_OUT) == ENDP1_PACKET_SIZE) {
    if (setTimeout(EP1_OUT_Process, 0) == 0xFF) EP1_OUT_Process();
  } else {
    SetEPRxValid(ENDP1);
  }
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
  SetEPRxStatus(ENDP4, EP_RX_NAK);
  if (USB_SIL_Read(EP4_OUT, HIDCFG_Buffer_OUT) == ENDP4_PACKET_SIZE) {
    if (setTimeout(EP4_OUT_Process, 0) == 0xFF) EP4_OUT_Process();
  } else {
    SetEPRxValid(ENDP4);
  }
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
      // CDC_Check();
    } else {
      // 发送一个ZLP 0长度包，通知包已发送完毕
      USBD_ENDPx_DataUp(CDC_LED_IO_EP, NULL, 0);
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
      // CDC_Check();
    } else {
      // 发送一个ZLP 0长度包，通知包已发送完毕
      USBD_ENDPx_DataUp(CDC_CARD_IO_EP, NULL, 0);
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
  static const struct { uint16_t address, size; } txBuffers[] = {
      [ENDP1] = {ENDP1_TXADDR, ENDP1_PACKET_SIZE},
      [ENDP2] = {ENDP2_TXADDR, ENDP2_PACKET_SIZE},
      [ENDP3] = {ENDP3_TXADDR, ENDP3_PACKET_SIZE},
      [ENDP4] = {ENDP4_TXADDR, ENDP4_PACKET_SIZE},
  };
  if (endp < ENDP1 || endp > ENDP4 || len > txBuffers[endp].size || (len && !pbuf)) {
    return USB_ERROR;
  }
  uint32_t interruptState = IRQ_Save();
  uint8_t result = USB_ERROR;
  if (bDeviceState == CONFIGURED && !(_GetISTR() & ISTR_RESET)) {
    uint16_t ep = _GetENDPOINT(endp);
    if ((ep & (EPTX_STAT | EP_CTR_TX)) == EP_TX_NAK) {
      // A hardware reset during this copy must never redirect it into the buffer table.
      UserToPMABufferCopy(pbuf, txBuffers[endp].address, len);
      if (!(_GetISTR() & ISTR_RESET)) {
        SetEPTxCount(endp, len);
        // Toggle NAK to VALID once. The host can consume the packet immediately.
        _SetENDPOINT(endp, (ep & EPREG_MASK) | EP_CTR_RX | EP_CTR_TX | EPTX_DTOG1);
        result = USB_SUCCESS;
      }
    }
  }
  IRQ_Restore(interruptState);
  return result;
}
