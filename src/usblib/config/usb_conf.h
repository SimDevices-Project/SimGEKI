/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_conf.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : This file contains all the functions prototypes for the  
 *                      USB configration firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/ 
#ifndef __USB_CONF_H
#define __USB_CONF_H


#define EP_NUM                          (15)

/* Buffer Description Table */
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)


/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x50)
#define ENDP0_TXADDR        (0x60)

/* EP1  */
#define ENDP1_PACKET_SIZE   (64)
#define ENDP1_TXADDR        (0x70)               
#define ENDP1_RXADDR        (ENDP1_TXADDR + ENDP1_PACKET_SIZE)

/* EP2  */
#define ENDP2_PACKET_SIZE   (32)
#define ENDP2_TXADDR        (ENDP1_RXADDR + ENDP2_PACKET_SIZE)
#define ENDP2_RXADDR        (ENDP2_TXADDR + ENDP2_PACKET_SIZE)

/* EP3  */
#define ENDP3_PACKET_SIZE   (32)
#define ENDP3_TXADDR        (ENDP2_RXADDR + ENDP3_PACKET_SIZE)
#define ENDP3_RXADDR        (ENDP3_TXADDR + ENDP3_PACKET_SIZE)

/* EP4  */
#define ENDP4_PACKET_SIZE   (64)
#define ENDP4_TXADDR        (ENDP3_RXADDR + ENDP4_PACKET_SIZE)
#define ENDP4_RXADDR        (ENDP4_TXADDR + ENDP4_PACKET_SIZE)


/* ISTR events */
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM  | CNTR_SOFM \
                 | CNTR_ESOFM | CNTR_RESETM )

/* #define CTR_CALLBACK */
/* #define DOVR_CALLBACK */
/* #define ERR_CALLBACK */
/* #define WKUP_CALLBACK */
/* #define SUSP_CALLBACK */
/* #define RESET_CALLBAC K*/
/* #define SOF_CALLBACK */
/* #define ESOF_CALLBACK */


/* CTR service routines */
/* associated to defined endpoints */
// #define  EP1_IN_Callback   NOP_Process
// #define  EP2_IN_Callback   NOP_Process
// #define  EP3_IN_Callback   NOP_Process
// #define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

// #define  EP1_OUT_Callback   NOP_Process
// #define  EP2_OUT_Callback   NOP_Process
// #define  EP3_OUT_Callback   NOP_Process
// #define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /* __USB_CONF_H */






