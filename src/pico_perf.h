#ifndef _pico_perf_h
#define _pico_perf_h

void timingInit (int pin);
void timingToggle (int pin);
void timingOn (int pin);
void timingOff (int pin);

void ledInit (void);
void ledOn (void);
void ledOff (void);
void ledFlash (int flashes);

#endif
