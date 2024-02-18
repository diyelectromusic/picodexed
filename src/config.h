#ifndef _config_h
#define _config_h

#define PICODEXED_VERSION "v0.01xxx"

#define MIDI_CHANNEL 1
#define MIDI_SYSEX_DEVICE_ID 0  // Yamaha SysEx Device Number maps onto 0..F..

// Sensible configurations:
//    24000, 16, 1 16-note polyphony at 24kHz if overclocked to 250MHz
//    24000,  8, 0  8-note polyphony at 24kHz
//    48000,  8, 1  8-note polyphony at 48kHz if overclocked to 250MHz
//    48000,  4, 0  4-note polyphony at 48kHz
//
#define DEXED_SAMPLE_RATE    24000
#define DEXED_POLYPHONY      16
#define DEXED_OVERCLOCK      1

#define I2S_DATA_PIN    9
#define I2S_BCLK_PIN    10
#define I2S_LRCLK_PIN   11  // Implied by BCLK=10

#define MIDI_TX_PIN     4   // Not used at present
#define MIDI_RX_PIN     5

#define DEBUG_UART_TX_PIN  0
#define DEBUG_UART_RX_PIN  1

#endif
