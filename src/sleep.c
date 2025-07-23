#include "sleep.h"
#include "timeout.h"
#include "data.h"

#define MAX_SLEEP_CALLBACKS 8

static void (*sleep_callbacks[MAX_SLEEP_CALLBACKS])(void) = {NULL};
static void (*wakeup_callbacks[MAX_SLEEP_CALLBACKS])(void) = {NULL};

uint8_t timerFlag = 0xFF;
#define SLEEP_TIMEOUT GlobalData->SleepTimeout

void Sleep_OnSleep();
void Sleep_OnWakeup();

xdata void Sleep_Init()
{
  clearTimeout(timerFlag);
  Sleep_Alive();
}

void Sleep_Alive()
{
  if(SLEEP_TIMEOUT > 0){
    if(timerFlag == 0xFE) {
      Sleep_OnWakeup();
      timerFlag = 0xFF; // Reset the timer flag
    }
    clearTimeout(timerFlag);
    timerFlag = setTimeout(Sleep_OnSleep, SLEEP_TIMEOUT);
  }
}

void Sleep_OnSleep()
{
  timerFlag = 0xFE; // Set the timer flag to indicate sleep mode
  // Call all registered sleep callbacks
  for (size_t i = 0; i < MAX_SLEEP_CALLBACKS; i++) {
    if (sleep_callbacks[i] != NULL) {
      sleep_callbacks[i]();
    }
  }
}

void Sleep_OnWakeup()
{
  for (size_t i = 0; i < MAX_SLEEP_CALLBACKS; i++) {
    if (wakeup_callbacks[i] != NULL) {
      wakeup_callbacks[i]();
    }
  }
}

uint8_t Sleep_RegisterSleep(void (*callback)(void))
{
  for (size_t i = 0; i < MAX_SLEEP_CALLBACKS; i++) {
    if (sleep_callbacks[i] == NULL) {
      sleep_callbacks[i] = callback;
      return i;
    }
  }
  return 0xFF; // No space available
}

void Sleep_UnregisterSleep(uint8_t index)
{
  if (index < 0 || index >= MAX_SLEEP_CALLBACKS) {
    return; // Invalid index
  }
  if (sleep_callbacks[index] != NULL) {
    sleep_callbacks[index] = NULL; // Clear the callback
  }
}

uint8_t Sleep_RegisterWakeup(void (*callback)(void))
{
  for (size_t i = 0; i < MAX_SLEEP_CALLBACKS; i++) {
    if (wakeup_callbacks[i] == NULL) {
      wakeup_callbacks[i] = callback;
      return i;
    }
  }
  return 0xFF; // No space available
}

void Sleep_UnregisterWakeup(uint8_t index)
{
  if (index < 0 || index >= MAX_SLEEP_CALLBACKS) {
    return; // Invalid index
  }
  if (wakeup_callbacks[index] != NULL) {
    wakeup_callbacks[index] = NULL; // Clear the callback
  }
}