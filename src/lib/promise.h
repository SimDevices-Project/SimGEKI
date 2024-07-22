#include "bsp.h"

typedef enum {
  PROMISE_STATE_PENDING,
  PROMISE_STATE_RESOLVED,
  PROMISE_STATE_ACTIVED,
} __PROMISE_STATE;

{

};

struct __promise_struct {
  void (*action)(void *(uint8_t));
  void (*next)(uint8_t);
  __PROMISE_STATE state;
}
