#include "led.h"

#define RGB_PORT_COUNT      3
#define RGB_COUNT_PER_PORT  1

#define WS2812_TIME_0BIT    50
#define WS2812_TIME_1BIT    115

#define WS2812_FREQ         800000
#define TIMER_CLOCK_FREQ    144000000

#define BUFF_FRONT_OFFSET   1

#define TIMER_PER_BUFF_SIZE (RGB_COUNT_PER_PORT * 24 + 1 + BUFF_FRONT_OFFSET)
#define TIMER_PERIOD        ((TIMER_CLOCK_FREQ / WS2812_FREQ) - 1)

uint32_t colorList[RGB_PORT_COUNT][RGB_COUNT_PER_PORT]; // GRB format
uint8_t colorPWM[RGB_PORT_COUNT][TIMER_PER_BUFF_SIZE];  // GRB format in 24 bits

void setRgbColor32(uint8_t port, uint8_t index, uint32_t color)
{
  colorList[port][index] = color;
  for (uint8_t i = 0; i < 24; i++) {
    colorPWM[port][BUFF_FRONT_OFFSET + index * 24 + i] = ((color >> (23 - i)) & 0x01) ? WS2812_TIME_1BIT : WS2812_TIME_0BIT;
  }
}

void setRgbColor(uint8_t port, uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
  setRgbColor32(port, index, ((uint32_t)g << 16) | ((uint32_t)r << 8) | (uint32_t)b);
}

void setRgbColorAll(uint8_t r, uint8_t g, uint8_t b)
{
  for (uint8_t i = 0; i < RGB_PORT_COUNT; i++) {
    for (uint8_t j = 0; j < RGB_COUNT_PER_PORT; j++) {
      setRgbColor(i, j, r, g, b);
    }
  }
}

void setRgbColorPort(uint8_t port, uint8_t r, uint8_t g, uint8_t b)
{
  for (uint8_t i = 0; i < RGB_COUNT_PER_PORT; i++) {
    setRgbColor(port, i, r, g, b);
  }
}

void Timer3_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  // 使能外部时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  // 引脚配置
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // 时钟源配置
  TIM_InternalClockConfig(TIM3); // 内部时钟模式

  // 时基单元配置
  TIM_PrescalerConfig(TIM3, 0, TIM_PSCReloadMode_Immediate); // 设置预分频器
  TIM_CounterModeConfig(TIM3, TIM_CounterMode_Up);           // 计数模式
  TIM_SetAutoreload(TIM3, TIMER_PERIOD);                     // 自动重装载值
  TIM_SetClockDivision(TIM3, TIM_CKD_DIV1);                  // 时钟滤波器值

  // 输出通道配置
  TIM_OC4PolarityConfig(TIM3, TIM_OCPolarity_High); // 设置输出极性
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); // 必须要手动使能CCR寄存器的影子寄存器（类似缓冲器）的功能（默认不使能）

  TIM_OC3PolarityConfig(TIM3, TIM_OCPolarity_High); // 设置输出极性
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); // 必须要手动使能CCR寄存器的影子寄存器（类似缓冲器）的功能（默认不使能）

  // // RGB 0, PB1 Channel4
  TIM_SetCompare4(TIM3, 0);                            // 设置比较值
  TIM_SelectOCxM(TIM3, TIM_Channel_4, TIM_OCMode_PWM1); // 设置输出通道模式
  TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Enable);      // 使能输出通道

  // RGB 1, PB0 Channel3
  TIM_SetCompare3(TIM3, 0);                            // 设置比较值
  TIM_SelectOCxM(TIM3, TIM_Channel_3, TIM_OCMode_PWM1); // 设置输出通道模式
  TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);      // 使能输出通道

  // DMA配置
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize         = TIMER_PER_BUFF_SIZE;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;

  // RGB 0, PB1 Tim3, Channel4, DMA1, Channel3
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (TIM3->CH4CVR);
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)colorPWM[0];
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel3, ENABLE);

  // RGB 1, PB0 Channel3, DMA1, Channel2
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (TIM3->CH3CVR);
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)colorPWM[1];
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel2, ENABLE);

  // 使能定时器
  TIM_DMACmd(TIM3, TIM_DMA_CC4 | TIM_DMA_CC3, ENABLE);
  TIM_Cmd(TIM3, ENABLE); // 使能定时器3
}

void WS2812_Refresh()
{
  // 如果DMA繁忙，则拒绝本次刷新
  if (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET) { return; }
  if (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET) { return; }

  DMA_ClearFlag(DMA1_FLAG_TC3); // 清除传输完成标志
  DMA_ClearFlag(DMA1_FLAG_TC2); // 清除传输完成标志

  TIM_Cmd(TIM3, DISABLE);

  // RGB0 （PB1）
  DMA_Cmd(DMA1_Channel3, DISABLE);
  DMA_SetCurrDataCounter(DMA1_Channel3, TIMER_PER_BUFF_SIZE);
  DMA_Cmd(DMA1_Channel3, ENABLE);

  // RGB1 （PB0）
  DMA_Cmd(DMA1_Channel2, DISABLE);
  DMA_SetCurrDataCounter(DMA1_Channel2, TIMER_PER_BUFF_SIZE);
  DMA_Cmd(DMA1_Channel2, ENABLE);

  TIM_Cmd(TIM3, ENABLE);
}

void WS2812_Init(void)
{
  setRgbColorAll(0, 0, 0);
  for (uint8_t i = 0; i < RGB_PORT_COUNT; i++) {
    colorPWM[i][TIMER_PER_BUFF_SIZE - 1] = 0;
  }

  Timer3_Config();
}

void initRgbColor()
{
  WS2812_Init();
}

void showRgbColor()
{
  WS2812_Refresh();
}