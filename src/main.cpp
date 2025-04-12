/*
// Simple DIY Electronic Music Projects - picoDexed
//    diyelectromusic.wordpress.com
//
// Some aspects of this code are based on the MiniDexed and Synth_Dexed projects.
// See CREDITS listed here for details: https://github.com/diyelectromusic/picodexed

      MIT License
      
      Copyright (c) 2025 diyelectromusic (Kevin)
      
      Permission is hereby granted, free of charge, to any person obtaining a copy of
      this software and associated documentation files (the "Software"), to deal in
      the Software without restriction, including without limitation the rights to
      use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
      the Software, and to permit persons to whom the Software is furnished to do so,
      subject to the following conditions:
      
      The above copyright notice and this permission notice shall be included in all
      copies or substantial portions of the Software.
      
      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
      FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
      COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHERIN
      AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
      WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cstdio>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "arm_math.h"
#include "picodexed.h"
#include "pico_perf.h"

#define POLYPHONY 8

CPicoDexed picoDexed;

int main(void) {
  // Overclock the Pico to 250MHz instead of the default 133MHz.
  // I have it on good authority (read, my Mastodon follows)
  // that this is an ok thing to do.  So there.
#if PICODEXED_OVERCLOCK==1
  set_sys_clock_khz(250000, false);
#endif

  stdio_init_all();
  fprintf(stderr,"PicoDexed %s ...\n", PICODEXED_VERSION);
  fprintf(stderr,"\nConfiguration:\n");
  fprintf(stderr,"  Sample Rate:  %d\n", PICODEXED_SAMPLE_RATE);
  fprintf(stderr,"  Polyphony:    %d\n", PICODEXED_POLYPHONY);
#if PICODEXED_OVERCLOCK==1
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
