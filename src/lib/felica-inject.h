#ifndef __FELICA_INJECT_H
#define __FELICA_INJECT_H

#include "comio.h"

#include <stdbool.h>
#include <stdint.h>

void FelicaInject_Reset(void);
bool FelicaInject_BeginFelicaProbe(const uint8_t idm[8], const uint8_t system_code[2]);
bool FelicaInject_IsProbing(void);
void FelicaInject_FinishFelicaProbe(const uint8_t spad0[16]);
uint8_t FelicaInject_FillPollResponse(AIME_Response *res, const uint8_t idm[8], const uint8_t pmm[8]);
bool FelicaInject_EmulateMifareAuth(const uint8_t uid[4]);
bool FelicaInject_EmulateMifareRead(uint8_t block_number, uint8_t block[16]);

#endif
