#include "timeout.h"

#define TIMER_CLOCK_FREQ 144000000

#define MAX_TIMER_COUNT  16

typedef struct {
  uint16_t time;
  uint16_t period;
  void (*callback)(void);
} Timer_TypeDef;

Timer_TypeDef interval[MAX_TIMER_COUNT] = {0};
Timer_TypeDef timeout[MAX_TIMER_COUNT] = {0};

uint16_t timerSet = 0;

xdata void Timer4_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period        = 1000 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler     = TIMER_CLOCK_FREQ / 1000000 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
}

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void)
{
  timerSet += TIM_GetCounter(TIM4) + 1;

  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_SetCounter(TIM4, 0);
  }
}

uint8_t setInterval(void (*callback)(void), uint16_t period)
{
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    if (interval[i].callback == NULL) {
      interval[i].callback = callback;
      interval[i].period   = period;
      interval[i].time     = 0;
      return i;
    }
  }
  return 0xFF;
}

uint8_t setTimeout(void (*callback)(void), uint16_t period)
{
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    if (timeout[i].callback == NULL) {
      timeout[i].callback = callback;
      timeout[i].period   = period;
      timeout[i].time     = 0;
      return i;
    }
  }
  return 0xFF;
}


void clearInterval(uint8_t id)
{
  if (id < MAX_TIMER_COUNT) {
    interval[id].callback = NULL;
    interval[id].period   = 0;
    interval[id].time     = 0;
  }
}

void clearTimeout(uint8_t id)
{
  if (id < MAX_TIMER_COUNT) {
    timeout[id].callback = NULL;
    timeout[id].period   = 0;
    timeout[id].time     = 0;
  }
}

void Timer_Process()
{
  uint16_t timerSetRec = timerSet;
  uint8_t i;
  timerSet = 0;
  for (i = 0; i < MAX_TIMER_COUNT; i++) {
    if (interval[i].callback == NULL) {
      continue;
    }
    interval[i].time += timerSetRec;
    if (interval[i].time >= interval[i].period) {
      interval[i].time = 0;
      TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
      interval[i].callback();
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    }
  }
  for (i = 0; i < MAX_TIMER_COUNT; i++) {
    if (timeout[i].callback == NULL) {
      continue;
    }
    timeout[i].time += timerSetRec;
    if (timeout[i].time >= timeout[i].period) {
      timeout[i].time = 0;
      TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
      timeout[i].callback();
      clearTimeout(i);
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    }
  }
}

xdata void Timeout_Init()
{
  Timer4_Config();
}