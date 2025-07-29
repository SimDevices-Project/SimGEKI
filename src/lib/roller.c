#include "bsp.h"
#include "roller.h"
#include "data.h"

#include "led.h"

/* Global define */

// Actully 5 bytes, extra 1 byte for data safety
#define DATA_LEN               6
#define VALUE_OFFSET_MASK      0xFFFF

#define ROLLER_MODE_TEST_LIMIT 8

#define _offset                GlobalData->RollerOffset
#define VALUE_DEFAULT          0x8000

typedef enum {
  ROLLER_MODE_TEST = 0,
  ROLLER_MODE_SPI,
  ROLLER_MODE_ADC,
} RollerMode;

RollerMode rollerMode       = ROLLER_MODE_TEST;
uint8_t rollerModeTestCount = ROLLER_MODE_TEST_LIMIT;

int16_t Calibrattion_Val = 0;

uint16_t ADCValue[72] = {0}; // ADC采样值，前8次采样会被丢弃，只保留64次有效采样值

const uint8_t TxData[DATA_LEN] = {0x05, 0x00};
struct rxdata_t {
  uint8_t zero;
  uint8_t angle_h;
  uint8_t angle_l;
  uint8_t status;
  uint8_t crc;
  uint8_t dummy;
} __packed EncoderData;

volatile uint16_t EncoderValue = 0;

const uint8_t tableCRC[256] =
    {
        0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53, 0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
        0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E, 0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
        0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4, 0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
        0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19, 0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
        0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40, 0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
        0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D, 0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
        0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7, 0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
        0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A, 0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
        0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75, 0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
        0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8, 0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
        0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2, 0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
        0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F, 0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
        0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66, 0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
        0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB, 0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
        0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1, 0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
        0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C, 0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4};

/************************************************************************************************************
 * Name    : calcCRC8
 * Func    : Table CRC is the pointer to the look-up table (LUT)
 * input   :
 * output  :
 * Author  :
 * history :
 *************************************************************************************************************/
//*Table CRC is the pointer to the look-up table (LUT)
uint8_t calcCRC8(uint8_t *message, uint8_t Bytelength, const uint8_t *TableCRC)
{
  uint8_t crc;
  uint8_t index;

  crc = 0xFF;
  for (index = 0; index < Bytelength; index++) {
    crc = TableCRC[crc ^ *(message + index)];
  }
  return (~crc);
}

/*********************************************************************
 * @fn      SPI_FullDuplex_Init
 *
 * @brief   Configuring the SPI for full-duplex communication.
 *
 * @return  none
 */
void SPI_FullDuplex_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  SPI_InitTypeDef SPI_InitStructure   = {0};

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  // NSS
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // SCK
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // MISO
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // MOSI
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SPI_SSOutputCmd(SPI2, ENABLE);

  SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS               = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial     = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

  SPI_Cmd(SPI2, ENABLE);
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
  DMA_InitTypeDef DMA_InitStructure = {0};

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA_CHx);

  DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
  DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize         = bufsize;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_Tx_Init_ADC
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init_ADC(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
  DMA_InitTypeDef DMA_InitStructure = {0};

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA_CHx);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
  DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize         = bufsize;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
  DMA_InitTypeDef DMA_InitStructure = {0};

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA_CHx);

  DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
  DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize         = bufsize;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      Get_ADC_ConversionVal
 *
 * @brief   Get Conversion Value.
 *
 * @param   val - Sampling value
 *
 * @return  val+Calibrattion_Val - Conversion Value.
 */
uint16_t Get_ADC_ConversionVal(int16_t val)
{
  if ((val + Calibrattion_Val) < 0 || val == 0)
    return 0;
  if ((Calibrattion_Val + val) > 4095 || val == 4095)
    return 4095;
  return (val + Calibrattion_Val);
}

xdata void Roller_Init()
{
  SPI_FullDuplex_Init();
  DMA_Tx_Init(DMA1_Channel5, (u32) & (SPI2->DATAR), (u32)TxData, DATA_LEN);
  DMA_Rx_Init(DMA1_Channel4, (u32) & (SPI2->DATAR), (u32)&EncoderData, DATA_LEN);

  GPIO_SetBits(GPIOB, GPIO_Pin_12);

  DMA_Cmd(DMA1_Channel5, ENABLE);
  DMA_Cmd(DMA1_Channel4, ENABLE);
}

xdata void Roller_ADC_Init()
{
  ADC_InitTypeDef ADC_InitStructure   = {0};
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  // 使能GPIOA和ADC1外设时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);

  // 将PA5配置为模拟输入，供ADC采集使用
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // ADC1初始化配置：独立模式、单通道连续转换，软件触发转换，数据右对齐
  ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode       = DISABLE;                   // 单通道模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    // 连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; // 软件触发
  ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel       = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  // 配置ADC1规则组通道5（对应PA5）
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);

  // 使能ADC1
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);

  ADC_BufferCmd(ADC1, DISABLE); // 禁用缓冲区
  ADC_ResetCalibration(ADC1);   // 复位校准
  while (ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1); // 启动校准
  while (ADC_GetCalibrationStatus(ADC1));
  Calibrattion_Val = Get_CalibrationValue(ADC1); // 获取校准值

  DMA_Tx_Init_ADC(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)&ADCValue, 64);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  DMA_Cmd(DMA1_Channel1, ENABLE);
}

// 重新读取编码器值
void Roller_Update()
{
  switch (rollerMode) {
    case ROLLER_MODE_TEST: { // 测试是否有SPI合法输入数据
      if (EncoderData.angle_h == 0 && EncoderData.angle_l == 0 && EncoderData.status == 0 && EncoderData.crc == 0) {
        // 输入不合法
        rollerModeTestCount++;
      } else {
        // 输入合法
        rollerModeTestCount--;
      }
      if (rollerModeTestCount == 0) {
        // 如果合法次数超过阈值，说明接入了SPI编码器，则切换到SPI模式
        rollerMode          = ROLLER_MODE_SPI;
        rollerModeTestCount = 0;
      } else if (rollerModeTestCount > 2 * ROLLER_MODE_TEST_LIMIT) {
        // 如果不合法次数超过阈值，说明接入了ADC编码器，则切换到ADC模式
        rollerMode          = ROLLER_MODE_ADC;
        rollerModeTestCount = 0;
        // 关闭SPI和对应DMA通道
        SPI_Cmd(SPI2, DISABLE);
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        Roller_ADC_Init();
      }
    }
    case ROLLER_MODE_SPI: {
      if ((!DMA_GetFlagStatus(DMA1_FLAG_TC5)) && (!DMA_GetFlagStatus(DMA1_FLAG_TC4))) {
        return;
      }

      DMA_ClearFlag(DMA1_FLAG_TC5); // 清除传输完成标志
      DMA_ClearFlag(DMA1_FLAG_TC4); // 清除传输完成标志

      SPI_Cmd(SPI2, DISABLE);

      if (EncoderData.crc == calcCRC8((uint8_t *)&(EncoderData.angle_h), 3, tableCRC)) {
        EncoderValue = ((uint16_t)EncoderData.angle_h << 8 | (uint16_t)EncoderData.angle_l);
      }

      DMA_Cmd(DMA1_Channel5, DISABLE);
      DMA_SetCurrDataCounter(DMA1_Channel5, DATA_LEN);
      DMA_Cmd(DMA1_Channel5, ENABLE);

      DMA_Cmd(DMA1_Channel4, DISABLE);
      DMA_SetCurrDataCounter(DMA1_Channel4, DATA_LEN);
      DMA_Cmd(DMA1_Channel4, ENABLE);

      SPI_Cmd(SPI2, ENABLE);
      break;
    }
    case ROLLER_MODE_ADC: {
      if ((!DMA_GetFlagStatus(DMA1_FLAG_TC1))) {
        return;
      }
      DMA_ClearFlag(DMA1_FLAG_TC1); // 清除传输完成标志

      uint8_t i;
      uint32_t sum = 0;
      for (i = 8; i < 72; i++) {
        sum += ADCValue[i];
      }

      // 读取ADC值
      EncoderValue = Get_ADC_ConversionVal(sum >> 6);
      EncoderValue = ~EncoderValue; // 取反以符合编码器方向
      EncoderValue <<= 4;

      // 重新开始ADC转换
      DMA_Cmd(DMA1_Channel1, DISABLE);
      DMA_SetCurrDataCounter(DMA1_Channel1, 72);
      DMA_Cmd(DMA1_Channel1, ENABLE);
      break;
    }
    default:
      break;
  }
}

// 获取原始编码器值
uint16_t Roller_GetRawValue()
{
  return EncoderValue;
}

#define DEBOUNCE_LENGTH  8                      // 滑动窗口大小
#define DEBOUNCE_LIMIT_A 64                     // 去抖阈值
#define DEBOUNCE_LIMIT_B 16                     // 滤波后的去抖阈值
uint16_t debounceBuffer[DEBOUNCE_LENGTH] = {0}; // 滑动窗口
uint32_t debounceSumValue                = 0;   // 滑动窗口和
uint16_t debounceAvgValue                = 0;   // 滑动窗口平均值
uint16_t outputValue                     = 0;   // 滤波后的去抖值
uint8_t debounceIndex                    = 0;   // 滑动窗口索引

// 获取经过OFFSET处理并去抖后的编码器值
uint16_t Roller_GetValue()
{
  const uint8_t multiple = 3; // 缩放倍数

  uint16_t rawVal     = Roller_GetRawValue();
  uint8_t refreshFlag = 0;

  // 去抖
  if (rawVal > debounceAvgValue) {
    if (rawVal - debounceAvgValue > DEBOUNCE_LIMIT_A) {
      refreshFlag = 1;
    }
  } else {
    if (debounceAvgValue - rawVal > DEBOUNCE_LIMIT_A) {
      refreshFlag = 1;
    }
  }
  if (refreshFlag == 1) {
    for (uint8_t i = 0; i < DEBOUNCE_LENGTH; i++) {
      debounceBuffer[i] = rawVal;
    }
    debounceAvgValue = rawVal;
    debounceSumValue = rawVal * DEBOUNCE_LENGTH;
  } else {
    debounceSumValue -= debounceBuffer[debounceIndex];
    debounceBuffer[debounceIndex] = rawVal;
    debounceSumValue += rawVal;
    debounceIndex    = (debounceIndex + 1) % DEBOUNCE_LENGTH;
    debounceAvgValue = debounceSumValue / DEBOUNCE_LENGTH;
  }

  if (outputValue > debounceAvgValue && outputValue - debounceAvgValue > DEBOUNCE_LIMIT_B) {
    outputValue = debounceAvgValue;
  } else if (debounceAvgValue >= outputValue && debounceAvgValue - outputValue > DEBOUNCE_LIMIT_B) {
    outputValue = debounceAvgValue;
  }

  // offset 计算
  if (outputValue <= VALUE_OFFSET_MASK - _offset) {
    outputValue = outputValue + _offset;
  } else {
    outputValue = ((_offset + outputValue) & VALUE_OFFSET_MASK) + 1;
  }

  // 对outputValue进行扩大处理
  int32_t tmp = (int32_t)outputValue - 0x8000; // 计算与0x8000的差值
  tmp         = tmp * multiple;                // 扩大multiple倍
  tmp         = 0x8000 + tmp;                  // 计算扩大后的值

  // 限制在有效范围内
  uint16_t currentExpandedValue;
  if (tmp > 0xFFFF) {
    currentExpandedValue = 0xFFFF;
  } else if (tmp < 0x0000) {
    currentExpandedValue = 0x0000;
  } else {
    currentExpandedValue = (uint16_t)tmp;
  }

  // 对扩大后的数据进行插值处理，使数值更连续
  static uint16_t lastExpandedValue = 0x8000; // 保存上次扩大后的值
  static uint8_t firstTime          = 1;

  if (firstTime) {
    lastExpandedValue = currentExpandedValue;
    firstTime         = 0;
    outputValue       = currentExpandedValue;
  } else {
    // 计算当前值与上次值的差异
    uint16_t valueDiff = currentExpandedValue > lastExpandedValue ? currentExpandedValue - lastExpandedValue : lastExpandedValue - currentExpandedValue;

    // 如果变化较大，进行平滑插值（降低历史数据权重以提高响应速度）
    if (valueDiff > 256) {
      outputValue = (uint16_t)(((uint32_t)lastExpandedValue + (uint32_t)currentExpandedValue * 3) >> 2); // 1:3加权平均
    } else if (valueDiff > 64) {
      outputValue = (uint16_t)(((uint32_t)lastExpandedValue + (uint32_t)currentExpandedValue * 7) >> 3); // 1:7加权平均，快速响应
    } else {
      outputValue = (uint16_t)(((uint32_t)lastExpandedValue + (uint32_t)currentExpandedValue * 15) >> 4); // 1:15加权平均，最快响应
    }

    lastExpandedValue = outputValue; // 更新历史值
  }

  return outputValue;
}

// 重设Offset，使经过Offset处理后的编码器值为 0x8000
void Roller_ResetOffset()
{
  uint16_t newVal;
  uint16_t rawVal = Roller_GetRawValue();
  if (rawVal <= VALUE_DEFAULT) {
    newVal = VALUE_DEFAULT - rawVal;
  } else {
    newVal = VALUE_OFFSET_MASK - rawVal + VALUE_DEFAULT + 1;
  }
  _offset = newVal;
}