#ifndef  __BSP_H

#define __BSP_H

#include "stdio.h"
#include "ch32v20x.h"

#define xdata __attribute__((section(".stext"))) 

#ifndef TARGET_SIMGEKI


#endif

#endif // ! __BSP_H