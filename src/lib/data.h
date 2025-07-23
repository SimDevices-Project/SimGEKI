#ifndef __FLASH_H_
#define __FLASH_H_

#include "bsp.h"

#define FLASH_FAST_DATA_BASE_ADDR 0x0800F800
#define FLASH_FAST_DATA_SIZE      0x0800

#define FLASH_SLOW_DATA_BASE_ADDR 0x08033000
#define FLASH_SLOW_DATA_SIZE      0x5000

typedef struct {
  const uint16_t RollerOffset;
  const uint32_t SleepTimeout;
  const uint8_t _unused[FLASH_FAST_DATA_SIZE - 6];
} __packed FlashData;

typedef struct {
  uint16_t RollerOffset;
  uint32_t SleepTimeout;
  uint8_t _unused[FLASH_FAST_DATA_SIZE - 6];
} __packed RamData;

// extern FlashData *Data;
extern RamData *GlobalData;

void LoadData();
void SaveData();

#endif // !__FLASH_H_