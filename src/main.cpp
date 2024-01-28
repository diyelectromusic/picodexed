#include "pico/stdlib.h"
#include "pico_audio.hpp"
#include "arm_math.h"
#include "dexedadaptor.h"
#include "pico_perf.h"

#define DEXED_SAMPLE_RATE 24000
#define POLYPHONY 8
#define DEXED_NUM_SAMPLES 256
#define PICO_NUM_SAMPLES  256

#define PICO_AUDIO_PACK_I2S_DATA 9
#define PICO_AUDIO_PACK_I2S_BCLK 10

#define LED_PIN PICO_DEFAULT_LED_PIN

CDexedAdapter dexed(POLYPHONY,DEXED_SAMPLE_RATE);

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

int16_t SampleBuffer[DEXED_NUM_SAMPLES];
unsigned SampleIndex=0;
void initSampleBuffer (void) {
  dexed.getSamples(SampleBuffer, DEXED_NUM_SAMPLES);
/*  for (int i=0; i<NUM_SAMPLES/2; i++) {
    SampleBuffer[i] = 32000;
    SampleBuffer[i+NUM_SAMPLES/2] = -32000;
  }*/
  SampleIndex = 0;
}

int16_t getNextSample (void) {
  if (SampleIndex >= DEXED_NUM_SAMPLES) {
    // Refil the buffer and reset
    timingOn(4);
    dexed.getSamples(SampleBuffer, DEXED_NUM_SAMPLES);
    timingOff(4);
    SampleIndex = 0;
  }
  return SampleBuffer[SampleIndex++];
}

void fillSampleBuffer(struct audio_buffer_pool *ap) {
  struct audio_buffer *buffer = take_audio_buffer(ap, true);
  int16_t *samples = (int16_t *) buffer->buffer->bytes;
  dexed.getSamples(samples, buffer->max_sample_count);
  buffer->sample_count = buffer->max_sample_count;
  give_audio_buffer(ap, buffer);
}

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

void midiChordOn () {
assert(POLYPHONE < MIDI_NOTES);
  for (int i=0; i<POLYPHONY; i++) {
    dexed.keydown(midiNotes[i], 100);
  }
}

void midiChordOff () {
  for (int i=0; i<POLYPHONY; i++) {
    dexed.keyup(midiNotes[i]);
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
  for (int i=0; i<5; i++) {
    ledOn();
    sleep_ms(250);
    ledOff();
    sleep_ms(250);
  }
  
  dexed.activate();

  initSampleBuffer();
  
  struct audio_buffer_pool *ap = init_audio(DEXED_SAMPLE_RATE, PICO_AUDIO_PACK_I2S_DATA, PICO_AUDIO_PACK_I2S_BCLK, 0, 0, PICO_NUM_SAMPLES);
  
  //dexed.loadInitVoice();
  dexed.loadVoiceParameters(sDefaultVoice);

  uint32_t millicount=0;
  bool isOn = false;
  while (1) {
    timingToggle(2);
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

//    timingOn(3);
//    update_buffer(ap, getNextSample);
    fillSampleBuffer(ap);
//    timingOff(3);
  }
  
  return 0;
}
