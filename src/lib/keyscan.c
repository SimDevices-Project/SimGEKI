#include "keyscan.h"

uint8_t KeyTriggerStatus[KEY_COUNT] = {0};

#define KEY_DEBOUNCE_BITS 16

#if KEY_DEBOUNCE_BITS <= 8
uint8_t KeyDebounceStatus[KEY_COUNT] = {0};
#define KEY_DEBOUNCE_MASK 0xFF
#elif KEY_DEBOUNCE_BITS <= 16
uint16_t KeyDebounceStatus[KEY_COUNT] = {0};
#define KEY_DEBOUNCE_MASK 0xFFFF
#else
uint32_t KeyDebounceStatus[KEY_COUNT] = {0};
#define KEY_DEBOUNCE_MASK 0xFFFFFFFF
#endif

uint8_t KeyStatus[KEY_COUNT] = {0};

xdata void KeyScan_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // 开启GPIOA和GPIOB的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

  // 设置PA0，PA1，PA2，PA3为输出
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // 设置PB10为输出
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // 设置PB3，PB4，PB5为输入
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // 设置PB4为输入
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB, OE);
}

uint8_t GetKeyTriggerStatus(uint8_t keyIndex)
{
  return KeyTriggerStatus[keyIndex];
}

void DebounceKey(uint8_t keyIndex)
{
  // 将按键的触发状态左移一位
  KeyDebounceStatus[keyIndex] <<= 1;
  // 将按键的触发状态加上按键的触发状态
  KeyDebounceStatus[keyIndex] |= GetKeyTriggerStatus(keyIndex);

  if (KeyDebounceStatus[keyIndex] == 0b11 || KeyDebounceStatus[keyIndex] == KEY_DEBOUNCE_MASK) {
    // 将按键的状态设置为1
    KeyStatus[keyIndex] = 1;
  } else if (KeyDebounceStatus[keyIndex] == KEY_DEBOUNCE_MASK - 0b11 || KeyDebounceStatus[keyIndex] == 0) {
    // 将按键的状态设置为0
    KeyStatus[keyIndex] = 0;
  }
}

void KeyScan_Scan()
{
  // 遍历每一行
  for (int row = 0; row < ROW_NUM; row++) {
    // 设置当前行输出低电平，其他行输出高电平
    switch (row) {
      case 0:
        GPIO_ResetBits(GPIOA, ROW1);
        GPIO_SetBits(GPIOA, ROW2 | ROW3 | ROW4);
        GPIO_SetBits(GPIOB, OE);
        break;
      case 1:
        GPIO_ResetBits(GPIOA, ROW2);
        GPIO_SetBits(GPIOA, ROW1 | ROW3 | ROW4);
        GPIO_SetBits(GPIOB, OE);
        break;
      case 2:
        GPIO_ResetBits(GPIOA, ROW3);
        GPIO_SetBits(GPIOA, ROW1 | ROW2 | ROW4);
        GPIO_SetBits(GPIOB, OE);
        break;
      case 3:
        GPIO_ResetBits(GPIOA, ROW4);
        GPIO_SetBits(GPIOA, ROW1 | ROW2 | ROW3);
        GPIO_SetBits(GPIOB, OE);
        break;
      case 4:
        GPIO_ResetBits(GPIOB, OE);
        GPIO_SetBits(GPIOA, ROW1 | ROW2 | ROW3 | ROW4);
        break;
    }

    Delay_Us(4);

    // 检测每一列的输入状态
    for (int col = 0; col < COL_NUM; col++) {
      // 判断当前列是否有输入
      if (GPIO_ReadInputDataBit(GPIOB, (uint16_t)(COL1 << col)) == Bit_RESET) {
        // 设置对应按键的触发状态为1
        KeyTriggerStatus[col * ROW_NUM + row] = 1;
      } else {
        // 设置对应按键的触发状态为0
        KeyTriggerStatus[col * ROW_NUM + row] = 0;
      }
    }
  }
  // 判断 Coin 是否有输入
  if (GPIO_ReadInputDataBit(GPIOA, COIN) == Bit_RESET) {
    // 设置按键 Row x Col 的触发状态为1
    KeyTriggerStatus[ROW_NUM * COL_NUM] = 1;
  } else {
    // 设置按键 Row x Col 的触发状态为0
    KeyTriggerStatus[ROW_NUM * COL_NUM] = 0;
  }

  for (uint8_t i = 0; i < KEY_COUNT; i++) {
    // 消抖处理
    DebounceKey(i);
  }
}

uint8_t KeyScan_GetKeyDebouncedStatus(uint8_t keyIndex)
{
  return KeyStatus[keyIndex];
}

uint16_t KeyScan_GetAllKeyDebouncedStatus()
{
  uint16_t keyStatus = 0;
  for (uint8_t i = 0; i < KEY_COUNT; i++) {
    keyStatus |= KeyStatus[i] << i;
  }
  return keyStatus;
}
