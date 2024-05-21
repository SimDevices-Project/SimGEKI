/*
 * ch32v203 eide demo
 * version: v1.0
 * Copyright (c) 2022 Taoyukai
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"

/**
 * @brief 
 * 
 * @return int 
 */
int main(void)
{
    Delay_Init();
    USART_Printf_Init(115200);
    printf("ch32v203 hello world\r\n");

    while(1)
    {
        ;
    }
}
