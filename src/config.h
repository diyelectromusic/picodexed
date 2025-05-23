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
#ifndef _config_h
#define _config_h

#define PICODEXED_VERSION "v0.03xx"

#define MIDI_CHANNEL 1
#define MIDI_SYSEX_DEVICE_ID 0  // Yamaha SysEx Device Number maps onto 0..F..

// Sensible configurations for I2S:
//    24000, 16, 1 16-note polyphony at 24kHz if overclocked to 250MHz
//    24000,  8, 0  8-note polyphony at 24kHz
//    48000,  8, 1  8-note polyphony at 48kHz if overclocked to 250MHz
//    48000,  4, 0  4-note polyphony at 48kHz
//
// Sensible configuration for PWM:
//    24000,  8, 0  8-note polyphony at 24kHz
//
// At present, overclocking messes with PWM.
//
#define PICODEXED_SAMPLE_RATE    24000
#define PICODEXED_POLYPHONY      16
#define PICODEXED_OVERCLOCK      1

// PIO Use:
//   PIO 0 - audio (I2S or PWM)
//   PIO 1 - rotary encoder

#define AUDIO_PWM       1
#define AUDIO_I2S       2

#define AUDIO_OUTPUT    AUDIO_I2S

// PWM requires an output circuit, e.g. as described here:
// https://embeddedcomputing.com/technology/open-source/development-kits/raspberry-pi-pico-pwm-audio-output-circuit
//
#define PWM_PIN         20

#define I2S_DATA_PIN    9
#define I2S_BCLK_PIN    10
#define I2S_LRCLK_PIN   11  // Implied by BCLK=10

#define MIDI_UART       1
#define MIDI_TX_PIN     4   // Not used at present
#define MIDI_RX_PIN     5

#define DEBUG_UART         0  // Provided here for info, but actually
#define DEBUG_UART_TX_PIN  0  // has to be set in CMakeLists.txt
#define DEBUG_UART_RX_PIN  1  // to be active...

#define DISPLAY_I2C_BUS  1  // Pico GPIO are on either I2C bus 0 or 1
#define DISPLAY_I2C_SDA  2
#define DISPLAY_I2C_SCL  3
#define DISPLAY_I2C_ADDR 0x3C
#define DISPLAY_W 128
#define DISPLAY_H 32

#define ENCODER_A_PIN   6
#define ENCODER_B_PIN   7 // Not actually used - code assumes it is ENCODER_A_PIN + 1
#define ENCODER_SW_PIN  8 // Not used
#define ENCODER_REV     0 // Reverse encoder sense (0 or 1)

#endif
