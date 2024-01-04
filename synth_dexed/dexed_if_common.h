#ifndef _dexed_if_common_h
#define _dexed_if_common_h

#include <pico.h>
#include <pico/time.h>

typedef bool boolean;

#define constrain(amt, low, high) ({ \
  __typeof__(amt) _amt = (amt); \
  __typeof__(low) _low = (low); \
  __typeof__(high) _high = (high); \
  (_amt < _low) ? _low : ((_amt > _high) ? _high : _amt); \
})

static inline int32_t signed_saturate_rshift(int32_t val, int bits, int rshift)
{
  int32_t out, max;

  out = val >> rshift;
  max = 1 << (bits - 1);
  if (out >= 0)
  {
    if (out > max - 1) out = max - 1;
  }
  else
  {
    if (out < -max) out = -max;
  }
  return out;
}

static inline uint32_t millis (void)
{
        return uint32_t (to_ms_since_boot(get_absolute_time()));
}
#endif