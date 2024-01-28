#include "pico/stdlib.h"
#include "arm_math.h"
#include "picodexed.h"
#include "pico_perf.h"

#define POLYPHONY 8
#define LED_PIN PICO_DEFAULT_LED_PIN

CPicoDexed picoDexed;

#define MIDI_NOTES 12
int midiNotes[MIDI_NOTES] = {48, 52, 55, 60, 64, 67, 72, 76, 79, 84, 88, 91};

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

void midiChordOn () {
  for (int i=0; i<POLYPHONY; i++) {
    picoDexed.keydown(midiNotes[i], 100);
  }
}

void midiChordOff () {
  for (int i=0; i<POLYPHONY; i++) {
    picoDexed.keyup(midiNotes[i]);
  }
}

int main(void) {
  //gpio_debug_pins_init(); // For Pico Library timing pins
  ledInit();
  // Use GPIO 2-5 for timing signals for testing
  timingInit(2);
  timingInit(3);
  timingInit(4);
  timingInit(5);

  ledFlash(2);
  sleep_ms(500);
  
  picoDexed.Init();
  picoDexed.loadVoiceParameters(sDefaultVoice);

  ledFlash(3);
  
  uint32_t millicount=0;
  bool isOn = false;
  while (1) {
    uint32_t millitime = millis();
    if (millitime > millicount) {
      if (isOn) {
        ledOff();
        midiChordOff();
        millicount = millis() + 4000;
      }
      else {
        ledOn();
        midiChordOn();
        millicount = millis() + 1000;
      }
      isOn = !isOn;
    }

    picoDexed.Process();
  }
  
  return 0;
}
