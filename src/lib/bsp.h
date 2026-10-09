#ifndef  __BSP_H

#define __BSP_H

#include "stdio.h"
#include "ch32v20x.h"

/* QingKe V4 gintenr maps MIE/MPIE and is accessible from U mode. */
static inline uint32_t IRQ_Save(void)
{
  uint32_t state;
  __asm volatile ("csrrc %0, 0x800, %1\n\tfence.i"
                  : "=r" (state) : "r" (0x88u) : "memory");
  return state;
}

static inline void IRQ_Restore(uint32_t state)
{
  __asm volatile ("csrw 0x800, %0" : : "r" (state) : "memory");
}

#ifndef TARGET_SIMGEKI


#endif

#endif // ! __BSP_H