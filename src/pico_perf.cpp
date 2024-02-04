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
