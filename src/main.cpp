#include <stdio.h>
#include "pico/stdlib.h"
#include "arm_math.h"
#include "picodexed.h"
#include "pico_perf.h"

#define POLYPHONY 8

CPicoDexed picoDexed;

uint8_t sDefaultVoice[156] =	// Brass 1
{
			 49,  99,  28,  68,  98,  98,  91,   0, 
			 39,  54,  50,   1,   1,   4,   0,   2, 
			 82,   0,   1,   0,   7,  77,  36,  41, 
			 71,  99,  98,  98,   0,  39,   0,   0, 
			  3,   3,   0,   0,   2,  98,   0,   1, 
			  0,   8,  77,  36,  41,  71,  99,  98, 
			 98,   0,  39,   0,   0,   3,   3,   0, 
			  0,   2,  99,   0,   1,   0,   7,  77, 
			 76,  82,  71,  99,  98,  98,   0,  39, 
			  0,   0,   3,   3,   0,   0,   2,  99, 
			  0,   1,   0,   5,  62,  51,  29,  71, 
			 82,  95,  96,   0,  27,   0,   7,   3, 
			  1,   0,   0,   0,  86,   0,   0,   0, 
			 14,  72,  76,  99,  71,  99,  88,  96, 
			  0,  39,   0,  14,   3,   3,   0,   0, 
			  0,  98,   0,   0,   0,  14,  84,  95, 
			 95,  60,  50,  50,  50,  50,  21,   7, 
			  1,  37,   0,   5,   0,   0,   4,   3, 
			 24,  66,  82,  65,  83,  83,  32,  32, 
			 32,  49,  32,  63
};

int main(void) {
  stdio_init_all();
  printf("PicoDexed...\n");
  //gpio_debug_pins_init(); // For Pico Library timing pins
  ledInit();
  // Use GPIO 2-3 for timing signals for testing
  timingInit(2);
  timingInit(3);
  
  ledFlash(2);
  sleep_ms(500);
  
  picoDexed.Init();
  picoDexed.loadVoiceParameters(sDefaultVoice);

  ledFlash(3);
  
  while (1) {
    picoDexed.Process();
  }
  
  return 0;
}
