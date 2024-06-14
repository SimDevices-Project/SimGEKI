#include "ch422.h"
#include "timeout.h"

/*
 *@Note
 *I2C
 *I2C1_SCL(PB6)\I2C1_SDA(PB7).
 *
 */

/* I2C Mode Definition */
#define HOST_MODE  0
#define SLAVE_MODE 1

/* I2C Communication Mode Selection */
#define I2C_MODE HOST_MODE
// #define I2C_MODE   SLAVE_MODE

/* Global define */
#define CH422_DATA_LEN     2
#define IIC_OWN_ADDRESS    0x02

#define CH422_DAT_LEN      5
#define CH422_COLORDAT_LEN 3
uint8_t Colors[CH422_DAT_LEN]    = {0xFF, 0xFF, 0xFF, 0xFF, 0x05};
uint8_t Addresses[CH422_DAT_LEN] = {0x72, 0x74, 0x70, 0x76, 0x48};

uint8_t data_len    = CH422_DAT_LEN;
uint8_t master_sate = 0xFF;

volatile uint8_t data_index = 0;

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(uint32_t bound, uint16_t address)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  I2C_InitTypeDef I2C_InitTSturcture  = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  // GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE); // Remap I2C1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  I2C_InitTSturcture.I2C_ClockSpeed          = bound;
  I2C_InitTSturcture.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitTSturcture.I2C_DutyCycle           = I2C_DutyCycle_2;
  I2C_InitTSturcture.I2C_OwnAddress1         = address;
  I2C_InitTSturcture.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &I2C_InitTSturcture);

  I2C_Cmd(I2C1, ENABLE);

#if (I2C_MODE == HOST_MODE)
  I2C_AcknowledgeConfig(I2C1, ENABLE);
#endif
}

void CH422_Init()
{
  IIC_Init(400000, IIC_OWN_ADDRESS);
}

void CH422_Check()
{
  switch (master_sate) {
    case 0: {
      if (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
        I2C_GenerateSTART(I2C1, ENABLE);
        master_sate = 1;
      }
      break;
    }
    case 1: {
      if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
        I2C_Send7bitAddress(I2C1, Addresses[data_index], I2C_Direction_Transmitter);
        master_sate = 2;
      }
      break;
    }
    case 2: {
      if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        I2C_SendData(I2C1, Colors[data_index]);
        master_sate = 3;
      }
      break;
    }
    case 3: {
      if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        data_index++;
        if (data_index >= data_len) {
          data_index  = 0;
          master_sate = 0xFF;
          if (data_len == CH422_DAT_LEN) {
            data_len = CH422_COLORDAT_LEN;
          }
        } else {
          master_sate = 0;
        }
      }
      break;
    }
    case 0xFF: {
      break;
    }
  }
}

uint8_t timerID = 0xFF;
void CH422_Refresh()
{
  if (master_sate == 0xFF) {
    master_sate = 0;
    clearInterval(timerID);
    timerID = setInterval(CH422_Check, 0);
  }
}

void CH422_Set(uint32_t dat)
{
  dat = ~dat;
  for (uint8_t i = 0; i < CH422_COLORDAT_LEN; i++) {
    Colors[i] = ((dat >> (6 * i)) & 0x3f) | 0xC0;
  }
  CH422_Refresh();
}
