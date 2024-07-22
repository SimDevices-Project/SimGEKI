#ifndef __PN532_H
#define __PN532_H

#include "bsp.h"

#define PN532_HOSTTOPN532   0xD4
#define PN532_PN532TOHOST   0xD5

#define PN532_PREAMBLE      0x00
#define PN532_STARTCODE1    0x00
#define PN532_STARTCODE2    0xFF
#define PN532_POSTAMBLE     0x00

#define PN532_OK            0
#define PN532_INVALID_ACK   0xF1
#define PN532_TIMEOUT       0xF2
#define PN532_INVALID_FRAME 0xF3
#define PN532_NO_SPACE      0xF4

#define PN532_WAITING       0xFF

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A           (0x60)
#define MIFARE_CMD_AUTH_B           (0x61)
#define MIFARE_CMD_READ             (0x30)
#define MIFARE_CMD_WRITE            (0xA0)
#define MIFARE_CMD_WRITE_ULTRALIGHT (0xA2)
#define MIFARE_CMD_TRANSFER         (0xB0)
#define MIFARE_CMD_DECREMENT        (0xC0)
#define MIFARE_CMD_INCREMENT        (0xC1)
#define MIFARE_CMD_STORE            (0xC2)

// FeliCa Commands
#define FELICA_CMD_POLLING                  (0x00)
#define FELICA_CMD_REQUEST_SERVICE          (0x02)
#define FELICA_CMD_REQUEST_RESPONSE         (0x04)
#define FELICA_CMD_READ_WITHOUT_ENCRYPTION  (0x06)
#define FELICA_CMD_WRITE_WITHOUT_ENCRYPTION (0x08)
#define FELICA_CMD_REQUEST_SYSTEM_CODE      (0x0C)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE         (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT  (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT (0x02)
#define NDEF_URIPREFIX_HTTP         (0x03)
#define NDEF_URIPREFIX_HTTPS        (0x04)
#define NDEF_URIPREFIX_TEL          (0x05)
#define NDEF_URIPREFIX_MAILTO       (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT   (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT   (0x08)
#define NDEF_URIPREFIX_FTPS         (0x09)
#define NDEF_URIPREFIX_SFTP         (0x0A)
#define NDEF_URIPREFIX_SMB          (0x0B)
#define NDEF_URIPREFIX_NFS          (0x0C)
#define NDEF_URIPREFIX_FTP          (0x0D)
#define NDEF_URIPREFIX_DAV          (0x0E)
#define NDEF_URIPREFIX_NEWS         (0x0F)
#define NDEF_URIPREFIX_TELNET       (0x10)
#define NDEF_URIPREFIX_IMAP         (0x11)
#define NDEF_URIPREFIX_RTSP         (0x12)
#define NDEF_URIPREFIX_URN          (0x13)
#define NDEF_URIPREFIX_POP          (0x14)
#define NDEF_URIPREFIX_SIP          (0x15)
#define NDEF_URIPREFIX_SIPS         (0x16)
#define NDEF_URIPREFIX_TFTP         (0x17)
#define NDEF_URIPREFIX_BTSPP        (0x18)
#define NDEF_URIPREFIX_BTL2CAP      (0x19)
#define NDEF_URIPREFIX_BTGOEP       (0x1A)
#define NDEF_URIPREFIX_TCPOBEX      (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX     (0x1C)
#define NDEF_URIPREFIX_FILE         (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID   (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG  (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT  (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW  (0x21)
#define NDEF_URIPREFIX_URN_EPC      (0x22)
#define NDEF_URIPREFIX_URN_NFC      (0x23)

// FeliCa consts
#define FELICA_READ_MAX_SERVICE_NUM     16
#define FELICA_READ_MAX_BLOCK_NUM       12 // for typical FeliCa card
#define FELICA_WRITE_MAX_SERVICE_NUM    16
#define FELICA_WRITE_MAX_BLOCK_NUM      10 // for typical FeliCa card
#define FELICA_REQ_SERVICE_MAX_NODE_NUM 32

// void     begin();
// uint32_t getFirmwareVersion(void);
// uint8_t  readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout, uint8_t inlist);
// int8_t   felica_Polling(uint16_t systemCode, uint8_t requestCode, uint8_t *idm, uint8_t *pmm, uint16_t *systemCodeResponse, uint16_t timeout);

typedef struct
{
  void (*begin)(void);
  void (*wakeup)(void);
  void (*writeCommand)(void (*callback)(uint8_t), const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
  void (*readResponse)(void (*callback)(uint8_t), uint8_t *buf, uint8_t len, uint16_t timeout);
} __packed PN532_Interface;

#endif