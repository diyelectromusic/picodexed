cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
project(picodexed C CXX ASM)
pico_sdk_init()
add_executable(picodexed)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(NOISE_NUGGET_SDK_DIR ../noise-nugget-c-sdk)
include(${NOISE_NUGGET_SDK_DIR}/noise_nugget-config.cmake)

noise_nugget_lib(
  picodexed_nn
  ""
)

target_sources(picodexed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pico_perf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mididevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/picodexed.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/serialmidi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/sounddevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/display.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmidi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbtask.c
    ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
    ${CMAKE_CURRENT_LIST_DIR}/libs/ssd1306.c
    ${CMAKE_CURRENT_LIST_DIR}/libs/encoder.cpp
)

add_subdirectory(synth_dexed)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(picodexed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/libs
    ${CMAKE_CURRENT_LIST_DIR}/synth_dexed/Synth_Dexed/src
    ${CMAKE_CURRENT_LIST_DIR}/synth_dexed
)

# PIO libs build
pico_generate_pio_header(picodexed ${CMAKE_CURRENT_LIST_DIR}/libs/pio_rotary_encoder.pio)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(picodexed PUBLIC picodexed_nn synth_dexed pico_stdlib pico_multicore tinyusb_device tinyusb_board pico_audio_i2s pico_audio_pwm hardware_i2c hardware_pio)

target_compile_definitions(picodexed PRIVATE
    PICO_AUDIO_I2S_MONO_OUTPUT=1
    PICO_AUDIO_I2S_MONO_INPUT=1
	USE_AUDIO_I2S=1
    USE_AUDIO_PWM=0
    PICO_DEFAULT_UART=1
    PICO_DEFAULT_UART_TX_PIN=4
    PICO_DEFAULT_UART_RX_PIN=5
)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(picodexed PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(picodexed)
