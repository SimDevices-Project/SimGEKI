#include "pn532.h"

#include "pn532_uart.h"

uint8_t _uid[7];       // ISO14443A uid
uint8_t _uidLen;       // uid len
uint8_t _key[6];       // Mifare Classic key
uint8_t inListedTag;   // Tg number of inlisted tag.
uint8_t _felicaIDm[8]; // FeliCa IDm (NFCID2)
uint8_t _felicaPMm[8]; // FeliCa PMm (PAD)

uint8_t pn532_packetbuffer[64];

#define PN532_TIMEOUT 1000

#ifdef PN532_INTERFACE_UART
#define INTR(fun) (PN532_UART.fun)
#endif

void PN532_Init()
{
  INTR(begin)
  ();
  INTR(wakeup)
  ();
}

void (*__samconfig_ack_callback)(uint8_t);

void __PN532_SAMConfig_ACK_Handler_2(uint8_t status)
{
  if (status > 0) {
    __samconfig_ack_callback(PN532_OK);
    __samconfig_ack_callback = NULL;
    return;
  } else {
    __samconfig_ack_callback(PN532_INVALID_FRAME);
    __samconfig_ack_callback = NULL;
    return;
  }
}

void __PN532_SAMConfig_ACK_Handler(uint8_t status)
{
  if (status == PN532_OK) {
    INTR(readResponse)
    (__PN532_SAMConfig_ACK_Handler_2, pn532_packetbuffer, 4, PN532_TIMEOUT);
  } else {
    __samconfig_ack_callback(status);
    __samconfig_ack_callback = NULL;
  }
}

void PN532_SAMConfig(void (*callback)(uint8_t))
{
  pn532_packetbuffer[0]    = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1]    = 0x01; // normal mode;
  pn532_packetbuffer[2]    = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3]    = 0x00; // dont use IRQ pin
  __samconfig_ack_callback = callback;
  INTR(writeCommand)
  (__PN532_SAMConfig_ACK_Handler, pn532_packetbuffer, 4, sizeof(data), NULL, 0);
}