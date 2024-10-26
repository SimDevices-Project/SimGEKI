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

/**************************************************************************/
/*!
    @brief  Polling for uart response
*/
/**************************************************************************/
void PN532_Check()
{
#ifdef PN532_INTERFACE_UART
  PN532_UART_Check();
#endif
}

/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
void PN532_Init()
{
  INTR(begin)
  ();
  // INTR(wakeup)
  // ();
}

/**************************************************************************/
/*!
    @brief  Checks the firmware version of the PN5xx chip

    @returns  The chip's firmware version and ID
*/
/**************************************************************************/
void PN532_getFirmwareVersion(void)
{
  uint8_t packet_buffer[64];
  packet_buffer[0]= (PN532_COMMAND_GETFIRMWAREVERSION);
  PN532_UART_WriteCommand(packet_buffer,1,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_GET_VERSION;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/**************************************************************************/
/*!
    @brief  Configures the SAM (Secure Access Module)
*/
/**************************************************************************/
void PN532_SAMConfig()
{
  uint8_t packet_buffer[64];
  packet_buffer[0]= PN532_COMMAND_SAMCONFIGURATION;
  packet_buffer[1]= (0x01); // normal mode;
  packet_buffer[2]= (0x14); // timeout 50ms * 20 = 1 second
  packet_buffer[3]= (0x00); // dont use IRQ pin
  PN532_UART_WriteCommand(packet_buffer,4,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_SAMCONFIG;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/**************************************************************************/
/*!
    Sets the MxRtyPassiveActivation uint8_t of the RFConfiguration register

    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_setPassiveActivationRetries(uint8_t maxRetries)
{
  uint8_t packet_buffer[64];
  packet_buffer[0]= (PN532_COMMAND_RFCONFIGURATION);
  packet_buffer[1]=(5);    // Config item 5 (MaxRetries)
  packet_buffer[2]=(0xFF); // MxRtyATR (default = 0xFF)
  packet_buffer[3]=(0x01); // MxRtyPSL (default = 0x01)
  packet_buffer[4]=(maxRetries);
  PN532_UART_WriteCommand(packet_buffer,5,0,0);


  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_SET_PASSIVE_ACTIVATION_RETRIES;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/**************************************************************************/
/*!
    Sets the RFon/off uint8_t of the RFConfiguration register

    @param  autoRFCA    0x00 No check of the external field before 
                        activation 
                        
                        0x02 Check the external field before 
                        activation

    @param  rFOnOff     0x00 Switch the RF field off, 0x01 switch the RF 
                        field on

    @returns    1 if everything executed properly, 0 for an error
*/
/**************************************************************************/

void PN532_setRFField(uint8_t autoRFCA, uint8_t rFOnOff)
{
  uint8_t packet_buffer[64];
  packet_buffer[0]=(PN532_COMMAND_RFCONFIGURATION);
  packet_buffer[1]=(1);
  packet_buffer[2]=(0x00 | autoRFCA | rFOnOff);  
  PN532_UART_WriteCommand(packet_buffer,3,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_SET_RFFIELD;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*!
    Waits for an ISO14443A target to enter the field

    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    @param  timeout       The number of tries before timing out
    @param  inlist        If set to true, the card will be inlisted

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout, bool inlist)
{
  uint8_t packet_buffer[64];
  packet_buffer[0]=(PN532_COMMAND_INLISTPASSIVETARGET);
  packet_buffer[1]=(1);  // max 1 cards at once (we can set this to 2 later)
  packet_buffer[2]=(cardbaudrate);
  PN532_UART_WriteCommand(packet_buffer,3,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_READ_PASSIVE_TARGET_ID;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/***** Mifare Classic Functions ******/

/**************************************************************************/
/*!
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 bytes
                          key value

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
void PN532_mifareclassic_AuthenticateBlock (uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData)
{
    // Prepare the authentication command //
  uint8_t packet_buffer[64];
  packet_buffer[0]=(PN532_COMMAND_INDATAEXCHANGE);   /* Data Exchange Header */
  packet_buffer[1]=(1);                              /* Max card numbers */
  packet_buffer[2]=((keyNumber) ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A);
  packet_buffer[3]=(blockNumber);                    /* Block Number (1K = 0..63, 4K = 0..255 */
  memcpy (packet_buffer + 4, keyData, 6);
    for (i = 0; i < uidLen; i++) {
        pn532_packetbuffer[10 + i] = uid[i];              /* 4 bytes card ID */
    }
  PN532_UART_WriteCommand(packet_buffer,10 + uidLen,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_MIFARE_AUTHENTICATE_BLOCK;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/**************************************************************************/
/*!
    Tries to read an entire 16-bytes data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the byte array that will hold the
                          retrieved data (if any)

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t *data)
{
  uint8_t packet_buffer[64];
    /* Prepare the command */
  packet_buffer[0]=(PN532_COMMAND_INDATAEXCHANGE);
  packet_buffer[1]=(1);                      /* Card number */
  packet_buffer[2]=(MIFARE_CMD_READ);        /* Mifare Read command = 0x30 */
  packet_buffer[3]=(blockNumber);            /* Block Number (0..63 for 1K, 0..255 for 4K) */
  PN532_UART_WriteCommand(packet_buffer,4,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_MIFARE_READ_BLOCK;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/***** FeliCa Functions ******/
/**************************************************************************/
/*!
    @brief  Poll FeliCa card. PN532 acting as reader/initiator,
            peer acting as card/responder.
    @param[in]  systemCode             Designation of System Code. When sending FFFFh as System Code,
                                       all FeliCa cards can return response.
    @param[in]  requestCode            Designation of Request Data as follows:
                                         00h: No Request
                                         01h: System Code request (to acquire System Code of the card)
                                         02h: Communication perfomance request
    @param[out] idm                    IDm of the card (8 bytes)
    @param[out] pmm                    PMm of the card (8 bytes)
    @param[out] systemCodeResponse     System Code of the card (Optional, 2bytes)
    @return                            = 1: A FeliCa card has detected
                                       = 0: No card has detected
                                       < 0: error
*/
/**************************************************************************/
int8_t PN532::felica_Polling(uint16_t systemCode, uint8_t requestCode, uint8_t * idm, uint8_t * pmm, uint16_t *systemCodeResponse, uint16_t timeout)
{
  uint8_t packet_buffer[64];
  packet_buffer[0]=(PN532_COMMAND_INLISTPASSIVETARGET);
  packet_buffer[1]=(1);
  packet_buffer[2]=(1);
  packet_buffer[3]=(FELICA_CMD_POLLING);
  packet_buffer[4]=((systemCode >> 8) & 0xFF);
  packet_buffer[5]=(systemCode & 0xFF);
  packet_buffer[6]=(requestCode);
  packet_buffer[7]=(0);
  PN532_UART_WriteCommand(packet_buffer,8,0,0);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_FELICA_POLLING;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}

/**************************************************************************/
/*!
    @brief  Sends FeliCa command to the currently inlisted peer

    @param[in]  command         FeliCa command packet. (e.g. 00 FF FF 00 00  for Polling command)
    @param[in]  commandlength   Length of the FeliCa command packet. (e.g. 0x05 for above Polling command )
    @param[out] response        FeliCa response packet. (e.g. 01 NFCID2(8 bytes) PAD(8 bytes)  for Polling response)
    @param[out] responselength  Length of the FeliCa response packet. (e.g. 0x11 for above Polling command )
    @return                          = 1: Success
                                     < 0: error
*/
/**************************************************************************/
void PN532_felica_SendCommand (const uint8_t *command, uint8_t commandlength)
{
  uint8_t packet_buffer[64];
  packet_buffer[0] = 0x40; // PN532_COMMAND_INDATAEXCHANGE;--
  packet_buffer[1] = inListedTag;
  packet_buffer[2] = commandlength + 1;
  PN532_UART_WriteCommand(packet_buffer,3,command, commandlength);
}

/**************************************************************************/
/*!
    @brief  Sends FeliCa Read Without Encryption command

    @param[in]  numService         Length of the serviceCodeList
    @param[in]  serviceCodeList    Service Code List (Big Endian)
    @param[in]  numBlock           Length of the blockList
    @param[in]  blockList          Block List (Big Endian, This API only accepts 2-byte block list element)
    @param[out] blockData          Block Data
    @return                        = 1: Success
                                   < 0: error
*/
/**************************************************************************/
void PN532::felica_ReadWithoutEncryption (uint8_t numService, const uint16_t *serviceCodeList, uint8_t numBlock, const uint16_t *blockList, uint8_t blockData[][16])
{
  if (numService > FELICA_READ_MAX_SERVICE_NUM) {
    PN532_Status.PN532_Option_Status = PN532_ERROR;
    PN532_Status.PN532_CMD_Status = PN532_FELICA_READ;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 0);
    return;
  }
  if (numBlock > 4) {
    PN532_Status.PN532_Option_Status = PN532_ERROR;
    PN532_Status.PN532_CMD_Status = PN532_FELICA_READ;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 0);
    return;
  }

  uint8_t i, j=0, k;
  uint8_t cmdLen = 1 + 8 + 1 + 2*numService + 1 + 2*numBlock;
  uint8_t cmd[cmdLen];
  cmd[j++] = FELICA_CMD_READ_WITHOUT_ENCRYPTION;
  for (i=0; i<8; ++i) {
    cmd[j++] = _felicaIDm[i];
  }
  cmd[j++] = numService;
  for (i=0; i<numService; ++i) {
    cmd[j++] = serviceCodeList[i] & 0xFF;
    cmd[j++] = (serviceCodeList[i] >> 8) & 0xff;
  }
  cmd[j++] = numBlock;
  for (i=0; i<numBlock; ++i) {
    cmd[j++] = (blockList[i] >> 8) & 0xFF;
    cmd[j++] = blockList[i] & 0xff;
  }
  PN532_felica_SendCommand(cmd, cmdLen);

  PN532_Status.PN532_Option_Status = PN532_WAITING_FOR_ACK;
  PN532_Status.PN532_CMD_Status = PN532_FELICA_READ;
  PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 50);
}


/**************************************************************************/
/*!
    @brief  Sends FeliCa Write Without Encryption command

    @param[in]  numService         Length of the serviceCodeList
    @param[in]  serviceCodeList    Service Code List (Big Endian)
    @param[in]  numBlock           Length of the blockList
    @param[in]  blockList          Block List (Big Endian, This API only accepts 2-byte block list element)
    @param[in]  blockData          Block Data (each Block has 16 bytes)
    @return                        = 1: Success
                                   < 0: error
*/
/**************************************************************************/
void PN532_felica_WriteWithoutEncryption (uint8_t numService, const uint16_t *serviceCodeList, uint8_t numBlock, const uint16_t *blockList, uint8_t blockData[][16])
{
  if (numService > FELICA_WRITE_MAX_SERVICE_NUM) {
    PN532_Status.PN532_Option_Status = PN532_ERROR;
    PN532_Status.PN532_CMD_Status = PN532_FELICA_WRITE;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 0);
    return;
  }
  if (numBlock > FELICA_WRITE_MAX_BLOCK_NUM) {
    PN532_Status.PN532_Option_Status = PN532_ERROR;
    PN532_Status.PN532_CMD_Status = PN532_FELICA_WRITE;
    PN532_Status.PN532_Failed_task_key = setTimeout(PN532_Failed(), 0);
    return;
  }

  uint8_t i, j=0, k;
  uint8_t cmdLen = 1 + 8 + 1 + 2*numService + 1 + 2*numBlock + 16 * numBlock;
  uint8_t cmd[cmdLen];
  cmd[j++] = FELICA_CMD_WRITE_WITHOUT_ENCRYPTION;
  for (i=0; i<8; ++i) {
    cmd[j++] = _felicaIDm[i];
  }
  cmd[j++] = numService;
  for (i=0; i<numService; ++i) {
    cmd[j++] = serviceCodeList[i] & 0xFF;
    cmd[j++] = (serviceCodeList[i] >> 8) & 0xff;
  }
  cmd[j++] = numBlock;
  for (i=0; i<numBlock; ++i) {
    cmd[j++] = (blockList[i] >> 8) & 0xFF;
    cmd[j++] = blockList[i] & 0xff;
  }
  for (i=0; i<numBlock; ++i) {
    for(k=0; k<16; k++) {
      cmd[j++] = blockData[i][k];
    }
  }
  PN532_felica_SendCommand(cmd, cmdLen, response, &responseLength);
}