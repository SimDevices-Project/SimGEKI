#include "timeout.h"

#define TIMER_CLOCK_FREQ 144000000

#define MAX_TIMER_COUNT  16
#define MAX_MICROTASK_COUNT 4

typedef struct {
  uint32_t time;
  uint32_t period;
  void (*callback)(void);
  uint8_t isInterval;
} Timer_TypeDef;

static Timer_TypeDef timers[MAX_TIMER_COUNT] = {0};
static void (*microtasks[MAX_MICROTASK_COUNT])(void);
static uint8_t microtaskHead = 0;
static uint8_t microtaskTail = 0;
static volatile uint8_t microtaskCount = 0;

volatile uint32_t timerSet = 0;

xdata void Timer4_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period        = 1000 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler     = TIMER_CLOCK_FREQ / 1000000 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 15;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
}

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    timerSet += 1;
    // TIM_SetCounter(TIM4, 0);
  }
}

static uint8_t setTimer(void (*callback)(void), uint32_t period, uint8_t isInterval)
{
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    if (timers[i].callback == NULL) {
      timers[i].period     = period;
      timers[i].time       = 0;
      timers[i].isInterval = isInterval;
      timers[i].callback   = callback;
      return i;
    }
  }
  return 0xFF;
}

uint8_t setInterval(void (*callback)(void), uint32_t period)
{
  return setTimer(callback, period, 1);
}

uint8_t setTimeout(void (*callback)(void), uint32_t period)
{
  return setTimer(callback, period, 0);
}

static void clearTimer(uint8_t id)
{
  if (id < MAX_TIMER_COUNT) {
    timers[id].callback = NULL;
    timers[id].period   = 0;
    timers[id].time     = 0;
  }
}

void clearInterval(uint8_t id)
{
  clearTimer(id);
}

void clearTimeout(uint8_t id)
{
  clearTimer(id);
}

uint8_t queueMicrotask(void (*callback)(void))
{
  if (callback == NULL) {
    return 0;
  }
  uint32_t interruptState = __get_MSTATUS();
  __disable_irq();
  __asm volatile ("" ::: "memory");
  if (microtaskCount == MAX_MICROTASK_COUNT) {
    __set_MSTATUS(interruptState);
    return 0;
  }
  microtasks[microtaskTail] = callback;
  microtaskTail = (microtaskTail + 1) % MAX_MICROTASK_COUNT;
  microtaskCount++;
  __asm volatile ("" ::: "memory");
  __set_MSTATUS(interruptState);
  return 1;
}

static void processMicrotasks(void)
{
  while (microtaskCount != 0) {
    uint32_t interruptState = __get_MSTATUS();
    __disable_irq();
    __asm volatile ("" ::: "memory");
    if (microtaskCount == 0) {
      __set_MSTATUS(interruptState);
      return;
    }
    void (*callback)(void) = microtasks[microtaskHead];
    microtaskHead = (microtaskHead + 1) % MAX_MICROTASK_COUNT;
    microtaskCount--;
    __asm volatile ("" ::: "memory");
    __set_MSTATUS(interruptState);
    callback();
  }
}

void Timer_Process()
{
  processMicrotasks();
  TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
  uint32_t timerSetRec = timerSet;
  timerSet = 0;
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    if (timers[i].callback == NULL) {
      continue;
    }
    timers[i].time += timerSetRec;
    if (timers[i].time >= timers[i].period) {
      timers[i].time -= timers[i].period;
      if (timers[i].isInterval) {
        timers[i].callback();
      } else {
        void (*callback)(void) = timers[i].callback;
        clearTimer(i);
        callback();
      }
      processMicrotasks();
    }
  }
}

xdata void Timeout_Init()
{
  Timer4_Config();
}
