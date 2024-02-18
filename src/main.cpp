#include <cstdio>
#include <stdio.h>
#include "pico/stdlib.h"
#include "arm_math.h"
#include "picodexed.h"
#include "pico_perf.h"

#define POLYPHONY 8

CPicoDexed picoDexed;

int main(void) {
  // Overclock the Pico to 250MHz instead of the default 133MHz.
  // I have it on good authority (read, my Mastodon follows)
  // that this is an ok thing to do.  So there.
#if DEXED_OVERCLOCK==1
  set_sys_clock_khz(250000, false);
#endif

  stdio_init_all();
  fprintf(stderr,"PicoDexed %s ...\n", PICODEXED_VERSION);
  fprintf(stderr,"\nConfiguration:\n");
  fprintf(stderr,"  Sample Rate:  %d\n", DEXED_SAMPLE_RATE);
  fprintf(stderr,"  Polyphony:    %d\n", DEXED_POLYPHONY);
#if DEXED_OVERCLOCK==1
  fprintf(stderr,"  Overclocking: On\n");
#else
  fprintf(stderr,"  Overclocking: Off\n");
#endif
  //gpio_debug_pins_init(); // For Pico Library timing pins
  ledInit();
  // Use GPIO 2-3 for timing signals for testing
//  timingInit(2);
//  timingInit(3);
  
  ledFlash(2);
  sleep_ms(500);
  
  picoDexed.Init();

  ledFlash(3);
  
  while (1) {
    picoDexed.Process();
  }
  
  return 0;
}
