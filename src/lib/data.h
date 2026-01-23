#ifndef __FLASH_H_
#define __FLASH_H_

#include "bsp.h"

#define FLASH_FAST_DATA_BASE_ADDR 0x0800F800
#define FLASH_FAST_DATA_SIZE      0x0800

#define FLASH_SLOW_DATA_BASE_ADDR 0x08033000
#define FLASH_SLOW_DATA_SIZE      0x5000

typedef struct {
  const uint16_t RollerOffset; // 偏移量校准值
  const uint32_t SleepTimeout; // 休眠超时时间，单位：毫秒
  const uint8_t DeviceMode; // 设备工作模式，1:SGIO4, 2:私有HID，3:键盘鼠标
  const uint8_t _unused[FLASH_FAST_DATA_SIZE - 7];
} __packed FlashData;

typedef struct {
  uint16_t RollerOffset;
  uint32_t SleepTimeout;
  uint8_t DeviceMode;
  uint8_t _unused[FLASH_FAST_DATA_SIZE - 7];
} __packed RamData;

// extern FlashData *Data;
extern RamData *GlobalData;

void LoadData();
void SaveData();

#endif // !__FLASH_H_