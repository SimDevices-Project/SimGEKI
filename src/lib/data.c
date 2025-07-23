#include "data.h"
#include "string.h"

__attribute__((aligned(8))) RamData dat;

static volatile const RamData DEFAULT_DATA __attribute__((section(".flashdat"))) = {
    .RollerOffset = 0x0000,
    .SleepTimeout = 0x000927C0, // 600000 ms = 10 minutes
    ._unused      = {0}};

FlashData *Data     = (FlashData *)FLASH_FAST_DATA_BASE_ADDR;
RamData *GlobalData = &dat;

/*********************************************************************
 * @fn      FLASH_read
 *
 * @brief   Read Flash
 *
 * @return  none
 */
// void FLASH_read(uint32_t addr, uint8_t *pData, uint32_t len)
// {
//   uint32_t i;
//   for (i = 0; i < len; i++) {
//     *pData++ = *(uint8_t *)addr++;
//   }
// }

void LoadData()
{
  /* HCLK = SYSCLK/2 */
  // RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;

  // FLASH_read((uint32_t)Data, (uint8_t *)((uint32_t)GlobalData), FLASH_FAST_DATA_SIZE);
  // memset((uint8_t *)GlobalData, 0xAA, FLASH_FAST_DATA_SIZE);

  // GlobalData->RollerOffset = 0xAAAA;
  memset(&dat, 0x00, FLASH_FAST_DATA_SIZE);
  memcpy(&dat, (uint8_t *)Data, FLASH_FAST_DATA_SIZE);

  /* HCLK = SYSCLK */
  // RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
}

void SaveData()
{
  uint32_t Offset = 0x00;

  /* HCLK = SYSCLK/2 */
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;

  while ((Offset < FLASH_FAST_DATA_SIZE)) {
    FLASH_Unlock_Fast();
    // FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
    // 擦除1页
    FLASH_ErasePage_Fast((uint32_t)Data + Offset);
    // 写入1页
    FLASH_ProgramPage_Fast((uint32_t)Data + Offset, (uint32_t *)((uint32_t)GlobalData + Offset));
    FLASH_Lock_Fast();
    // 地址增加 256 字节
    Offset = Offset + 256;
  }

  /* HCLK = SYSCLK */
  RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
}