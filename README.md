# PicoDexed
Port of Synth_Dexed to the Raspberry Pi Pico

More details can be found here: https://diyelectromusic.wordpress.com/2024/01/09/raspberry-pi-pico-synth_dexed/

This is just experimental and likely to develop, change or break somewhat erratically as I thinker with it.  It remains to be seen if it will go anywhere or not.  For now, it's just a bit of fun.

Kevin

## Basic Build Instructions

1. Install the Pico SDK, toolchain and core libraries as per https://github.com/raspberrypi/pico-sdk.
2. Clone this repository.
3. Run the getsubmod.sh script to initialise CMSIS and Synth_Dexed.
4. Set the PICO_SDK_PATH variable to point to your Pico SDK installation.
5. Go to the build directory and run `cmake ..` followed by `make`.

Hardware requirements:
* [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/)
* [Pimoroni Pico Audio Pack](https://shop.pimoroni.com/products/pico-audio-pack)

It might work with other I2S audio interfaces too if the GPIO usage is updated appropriately.  At some point it will probably support PWM too.

## Credits

Builds on knowledge, work and code from the following:
* [Synth_Dexed](https://codeberg.org/dcoredump/Synth_Dexed) library by Holger Wirtz.
* [MiniDexed](https://github.com/probonopd/MiniDexed) initial port and code by Rene Stange and Holger Wirtz based on an idea by probonopd as discussed here: https://github.com/rsta2/circle/issues/274.
* Earle Philhower’s [Raspberry Pi Pico Arduino core for all RP2040 boards](https://github.com/earlephilhower/arduino-pico).
* Chris Hockuba’s [Raspberry Pi Pico version of CMSIS 5](https://gitlab.com/khockuba/cmsis-pi-pico)https://gitlab.com/khockuba/cmsis-pi-pico.

As well as exmaples from the Raspberry Pi C/C++ SDK and the Piromoni C/C++ examples for their Pico Audio Pack.
