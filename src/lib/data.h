#ifndef __FLASH_H_
#define __FLASH_H_

#include "bsp.h"

#define FLASH_FAST_DATA_BASE_ADDR 0x0800F000
#define FLASH_FAST_DATA_SIZE      0x1000

#define FLASH_SLOW_DATA_BASE_ADDR 0x08033000
#define FLASH_SLOW_DATA_SIZE      0x5000

typedef struct {
  const uint16_t RollerOffset;
  const uint8_t _unused[FLASH_FAST_DATA_SIZE - 2];
} __packed FlashData;

typedef struct {
  uint16_t RollerOffset;
  uint8_t _unused[FLASH_FAST_DATA_SIZE - 2];
} __packed RamData;

// extern FlashData *Data;
extern RamData *GlobalData;

void LoadData();
void SaveData();

#endif // !__FLASH_H_