#include "timeout.h"
#include <string.h>

#define TIMER_CLOCK_FREQ 144000000
#define MAX_TIMER_COUNT  16

typedef enum {
    TIMER_STATE_INACTIVE = 0,
    TIMER_STATE_ACTIVE,
    TIMER_STATE_PENDING_REMOVE
} timer_state_t;

typedef struct {
    uint16_t time;          // 16位定时器，适用于60s以内的定时
    uint16_t period;        // 16位周期
    void (*callback)(void);
    timer_state_t state;    // 定时器状态
} Timer_TypeDef;

typedef struct {
    Timer_TypeDef intervals[MAX_TIMER_COUNT];
    Timer_TypeDef timeouts[MAX_TIMER_COUNT];
    volatile uint32_t timer_ticks;  // volatile确保中断更新对主循环可见
    uint8_t active_timer_count;     // 活跃定时器计数
} timer_manager_t;

static timer_manager_t g_timer_mgr = {0};

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
    // 原子递增，避免竞态条件
    __atomic_fetch_add(&g_timer_mgr.timer_ticks, 1, __ATOMIC_RELAXED);
    
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_SetCounter(TIM4, 0);
  }
}

uint8_t setInterval(void (*callback)(void), uint16_t period)
{
  if (callback == NULL || period == 0) {
    return 0xFF; // 参数验证
  }

  // 由于Timer_Process在主循环调用，大部分操作都在主线程中
  // 只有timer_ticks会被中断修改，其他数据结构修改都是安全的
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    if (g_timer_mgr.intervals[i].state == TIMER_STATE_INACTIVE) {
      g_timer_mgr.intervals[i].callback = callback;
      g_timer_mgr.intervals[i].period = period;
      g_timer_mgr.intervals[i].time = 0;
      g_timer_mgr.intervals[i].state = TIMER_STATE_ACTIVE;
      g_timer_mgr.active_timer_count++;
      
      return i;
    }
  }
  
  return 0xFF; // 没有可用槽位
}

uint8_t setTimeout(void (*callback)(void), uint16_t period)
{
  if (callback == NULL || period == 0) {
    return 0xFF;
  }

  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    if (g_timer_mgr.timeouts[i].state == TIMER_STATE_INACTIVE) {
      g_timer_mgr.timeouts[i].callback = callback;
      g_timer_mgr.timeouts[i].period = period;
      g_timer_mgr.timeouts[i].time = 0;
      g_timer_mgr.timeouts[i].state = TIMER_STATE_ACTIVE;
      g_timer_mgr.active_timer_count++;
      
      return i;
    }
  }
  
  return 0xFF;
}

// 清除函数，适配主循环轮询
void clearInterval(uint8_t id)
{
  if (id >= MAX_TIMER_COUNT) return;
  
  // 在主循环中调用，只需标记为待删除
  if (g_timer_mgr.intervals[id].state == TIMER_STATE_ACTIVE) {
    g_timer_mgr.intervals[id].state = TIMER_STATE_PENDING_REMOVE;
  }
}

void clearTimeout(uint8_t id)
{
  if (id >= MAX_TIMER_COUNT) return;
  
  if (g_timer_mgr.timeouts[id].state == TIMER_STATE_ACTIVE) {
    g_timer_mgr.timeouts[id].state = TIMER_STATE_PENDING_REMOVE;
  }
}

// 针对主循环轮询的处理函数，考虑非严格定时和中断插入
void Timer_Process(void)
{
  // 简单读取当前tick计数，不需要原子交换
  // 因为定时器不要求严格准时，少量的tick丢失是可接受的
  uint32_t current_ticks = g_timer_mgr.timer_ticks;
  static uint32_t last_processed_ticks = 0;
  
  // 计算本次处理的tick增量
  uint32_t elapsed_ticks = current_ticks - last_processed_ticks;
  last_processed_ticks = current_ticks;
  
  // 如果没有时间流逝，快速返回
  if (elapsed_ticks == 0) {
    return;
  }

  // 处理interval定时器
  // 注意：执行过程中可能被中断打断，这是可接受的
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    Timer_TypeDef *timer = &g_timer_mgr.intervals[i];
    
    if (timer->state == TIMER_STATE_PENDING_REMOVE) {
      // 在主循环中删除
      memset(timer, 0, sizeof(Timer_TypeDef));
      timer->state = TIMER_STATE_INACTIVE;
      g_timer_mgr.active_timer_count--;
      continue;
    }
    
    if (timer->state != TIMER_STATE_ACTIVE) {
      continue;
    }
    
    // 防止16位溢出，限制单次累加的时间
    uint16_t add_time = (elapsed_ticks > 65535) ? 65535 : (uint16_t)elapsed_ticks;
    
    if (timer->time + add_time >= timer->period) {
      // 重置计时器，允许一些时间误差
      timer->time = (timer->time + add_time) - timer->period;
      
      // 在主线程上下文执行回调
      // 回调执行期间可能被其他中断打断，这是正常的
      if (timer->callback) {
        timer->callback();
      }
    } else {
      timer->time += add_time;
    }
  }

  // 处理timeout定时器
  for (uint8_t i = 0; i < MAX_TIMER_COUNT; i++) {
    Timer_TypeDef *timer = &g_timer_mgr.timeouts[i];
    
    if (timer->state == TIMER_STATE_PENDING_REMOVE) {
      memset(timer, 0, sizeof(Timer_TypeDef));
      timer->state = TIMER_STATE_INACTIVE;
      g_timer_mgr.active_timer_count--;
      continue;
    }
    
    if (timer->state != TIMER_STATE_ACTIVE) {
      continue;
    }
    
    // 防止16位溢出，限制单次累加的时间
    uint16_t add_time = (elapsed_ticks > 65535) ? 65535 : (uint16_t)elapsed_ticks;
    
    if (timer->time + add_time >= timer->period) {
      // 执行回调（可能被中断打断）
      if (timer->callback) {
        timer->callback();
      }
      
      // 清除一次性定时器
      memset(timer, 0, sizeof(Timer_TypeDef));
      timer->state = TIMER_STATE_INACTIVE;
      g_timer_mgr.active_timer_count--;
    } else {
      timer->time += add_time;
    }
  }
}

xdata void Timeout_Init()
{
  // 初始化定时器管理器
  memset(&g_timer_mgr, 0, sizeof(timer_manager_t));
  Timer4_Config();
}