#include "data.h"
#include "string.h"

RamData dat;

FlashData *Data;
RamData *GlobalData;

void LoadData()
{
  Data       = (FlashData *)FLASH_FAST_DATA_BASE_ADDR;
  GlobalData = &dat;

  memcpy(GlobalData, Data, FLASH_FAST_DATA_SIZE);

  // GlobalData->RollerOffset = 0xAAAA;
}

void SaveData()
{
  uint32_t Offset = 0x00;

  while ((Offset < FLASH_FAST_DATA_SIZE)) {
    FLASH_Unlock_Fast();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
    // 擦除1页
    FLASH_ErasePage_Fast((uint32_t)Data + Offset);
    // 写入1页
    FLASH_ProgramPage_Fast((uint32_t)Data + Offset, (uint32_t *)((uint32_t)GlobalData + Offset));
    FLASH_Lock_Fast();
    // 地址增加 256 字节
    Offset = Offset + 256;
  }
}