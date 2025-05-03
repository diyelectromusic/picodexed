# PicoDexed
Port of Synth_Dexed to the Raspberry Pi Pico

More details can be found here: https://diyelectromusic.wordpress.com/2024/01/09/raspberry-pi-pico-synth_dexed/
Updates for the latest version can be found here: https://diyelectromusic.com/2025/04/12/raspberry-pi-pico-synth_dexed-revisited/

This is just experimental and likely to develop, change or break somewhat erratically as I thinker with it.  It remains to be seen if it will go anywhere or not.  For now, it's just a bit of fun.

** No fitness for purpose can be implied.  Use at your own risk. **

Kevin

## Basic Build Instructions

1. Install the Pico SDK, toolchain and core libraries as per https://github.com/raspberrypi/pico-sdk.
    * I used the "Manually Configure your Environment" instructions in Appendix C of the "Getting Started with the Raspberry Pi Pico" C/C++ guide ([here](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf))
2. Clone this repository.
3. Run the getsubmod.sh script to initialise Synth_Dexed.
5. Apply the patches to Synth_Dexed that can be found in synth_dexed.
4. Run the getvoices.sh script to get and initialise the ROM voices.
4. Set the PICO_SDK_PATH variable to point to your Pico SDK installation.
5. Go to the build directory and run `cmake ..` followed by `make`.

There are some pre-built prototype UF2 files in the build area which can be dragged directly onto a Pico in boot mode.

Hardware requirements:
* [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/)
* I2S DAC.  Has been tested with a PCM5102 and the [Pimoroni Pico Audio Pack](https://shop.pimoroni.com/products/pico-audio-pack)
* Optional: MIDI IN ([more here](https://diyelectromusic.wordpress.com/2024/02/18/raspberry-pi-pico-synth_dexed-part-5/))
* Optional: SSD1306 128x32 I2C OLED display.
* Optional: Rotary encoder (e.g. KY-040 module).

It might work with other I2S audio interfaces too if the GPIO usage is updated appropriately.

There is a build that uses PWM too, but it is not recommended. It is only 8-note polyphonic and requires additional filter circuitry [as described here](https://diyelectromusic.wordpress.com/2024/02/18/raspberry-pi-pico-synth_dexed-part-5/).

**Note: The Raspberry Pi Pico is overclocked to 250MHz.**

## Current Features

* 16-note polyphony at a sample rate of 24000Hz.
* USB and Serial MIDI support.
* 8 banks of 32 voices (rom1a, rom1b, rom2a, rom2b, rom3a, rom3b, rom4a, rom4b) selectable over MIDI.
* Voice parameter changes via SysEx - e.g. using Dexed.
* Range of controllers as supported by Synth_Dexed.
* I2S audio output (recommended); PWM audio output(optional).
* SSD1306 I2C display support.
* Rotary Encoder support.

## More Information:

* Details of the hardware build can be found here: https://diyelectromusic.wordpress.com/2024/02/18/raspberry-pi-pico-synth_dexed-part-5/
* Details of the MIDI implementation can be found here: https://diyelectromusic.wordpress.com/2024/02/16/raspberry-pi-pico-synth_dexed-part-4/
* More details of the latest changes, the core architecture and UI support can be found here: https://diyelectromusic.com/2025/04/12/raspberry-pi-pico-synth_dexed-revisited/

## Contributions

This is just an information store for my personal playing around with this at the moment.  As such I'm not accepting contributions at this time.

There is no implied fitness for purpose or guarantee of usefulness or success.  Everything is experimental and subject to change without notice, but published freely here for general interest for anyone who would like to follow along for the ride.

## Credits

Builds on knowledge, work and code from the following:
* [Synth_Dexed](https://codeberg.org/dcoredump/Synth_Dexed) library by Holger Wirtz.
* [MiniDexed](https://github.com/probonopd/MiniDexed) initial port and code by Rene Stange and Holger Wirtz based on an idea by probonopd as discussed here: https://github.com/rsta2/circle/issues/274.
* Earle Philhower’s [Raspberry Pi Pico Arduino core for all RP2040 boards](https://github.com/earlephilhower/arduino-pico).
* Chris Hockuba’s [Raspberry Pi Pico version of CMSIS 5](https://gitlab.com/khockuba/cmsis-pi-pico)
* Francois Best's [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)
* David Schramm's [Pico SSD1306 code](https://github.com/daschr/pico-ssd1306)
* GitJer's [Pico Rotary Encoder code](https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/Rotary_encoder)

As well as exmaples from the Raspberry Pi C/C++ SDK and the Piromoni C/C++ examples for their Pico Audio Pack.
