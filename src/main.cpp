#include "pico/stdlib.h"
#include "pico_audio.hpp"
#include "dexedadaptor.h"

#define PICO_AUDIO_PACK_I2S_DATA 9
#define PICO_AUDIO_PACK_I2S_BCLK 10

#define SAMPLE_RATE 44100

CDexedAdapter dexed(1,SAMPLE_RATE);

#define NUM_SAMPLES 128
int16_t SampleBuffer[NUM_SAMPLES];
unsigned SampleIndex=0;
void initSampleBuffer (void) {
  dexed.getSamples(SampleBuffer, NUM_SAMPLES);
/*  for (int i=0; i<NUM_SAMPLES/2; i++) {
    SampleBuffer[i] = 32000;
    SampleBuffer[i+NUM_SAMPLES/2] = -32000;
  }*/
  SampleIndex = 0;
}
int16_t getNextSample (void) {
  if (SampleIndex >= NUM_SAMPLES) {
    // Refil the buffer and reset
    dexed.getSamples(SampleBuffer, NUM_SAMPLES);
    SampleIndex = 0;
  }
  return SampleBuffer[SampleIndex++];
}

#define LED_PIN PICO_DEFAULT_LED_PIN
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

int main(void) {
  ledInit();
  for (int i=0; i<5; i++) {
    ledOn();
    sleep_ms(250);
    ledOff();
    sleep_ms(250);
  }
  
  initSampleBuffer();
  
  struct audio_buffer_pool *ap = init_audio(SAMPLE_RATE, PICO_AUDIO_PACK_I2S_DATA, PICO_AUDIO_PACK_I2S_BCLK);
  
  dexed.loadInitVoice();

  uint32_t millicount=0;
  bool isOn = false;
  while (1) {
    uint32_t millitime = millis();
    if (millitime > millicount) {
      if (isOn) {
        ledOff();
        dexed.keyup(48);
      }
      else {
        ledOn();
        dexed.keydown(48, 100);
      }
      isOn = !isOn;
      millicount = millis() + 1000;
    }
    
    update_buffer(ap, getNextSample);
  }
  
  return 0;
}
