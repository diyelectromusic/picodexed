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
#include "pico/stdlib.h"
#include "pico_perf.h"

#define LED_PIN PICO_DEFAULT_LED_PIN

#define NUM_TIMING_PINS 36 // covers all GPIO pins on a RP2040
int timingPinValues[NUM_TIMING_PINS];

void ledInit (void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void ledOn (void) {
    gpio_put(LED_PIN, 1);
}

void ledOff (void) {
    gpio_put(LED_PIN, 0);
}

void ledFlash (int flashes) {
    for (int i=0; i<flashes; i++) {
        ledOn();
        sleep_ms(250);
        ledOff();
        sleep_ms(250);
    }
}

void timingInit (int pin) {
	  if (pin >= NUM_TIMING_PINS) {
		    return;
	  }
	  gpio_init(pin);
	  gpio_set_dir(pin, GPIO_OUT);
	  timingPinValues[pin] = 0;
	  gpio_put(pin, 0);
}

void timingToggle (int pin) {
	  if (pin >= NUM_TIMING_PINS) {
		    return;
	  }
	  if (timingPinValues[pin]) {
	    	gpio_put(pin, 0);
		    timingPinValues[pin] = 0;
	  } else {
		    gpio_put(pin, 1);
		    timingPinValues[pin] = 1;
	  }
}

void timingOn (int pin) {
	  if (pin >= NUM_TIMING_PINS) {
    		return;
	  }
	  gpio_put(pin, 1);
	  timingPinValues[pin] = 1;
}

void timingOff (int pin) {
  	if (pin >= NUM_TIMING_PINS) {
    		return;
	  }
	  gpio_put(pin, 0);
	  timingPinValues[pin] = 0;
}
