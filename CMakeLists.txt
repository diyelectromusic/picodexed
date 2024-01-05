cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
project(picodexed)
pico_sdk_init()
add_executable(picodexed)

set (CMSIS_DIR ${CMAKE_CURRENT_LIST_DIR}/cmsis/cmsis-pi-pico/CMSIS)
set (CMSIS_CORE_INCLUDE_DIR ${CMSIS_DIR}/Core/Include)
set (CMSIS_DSP_INCLUDE_DIR ${CMSIS_DIR}/DSP/Include)
set (CMSIS_DSP_PRIVATE_INCLUDE_DIR ${CMSIS_DIR}/DSP/PrivateInclude)
set (CMSIS_DSP_COMPUTELIB_INCLUDE_DIR ${CMSIS_DIR}/DSP/ComputeLibrary/Include)
set (CMSIS_DSP_SOURCE_DIR ${CMSIS_DIR}/DSP/Source)
set (CMSIS_DSP_COMPUTELIB_SRC_DIR ${CMSIS_DIR}/DSP/ComputeLibrary/Source)

target_sources(picodexed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
    ${CMSIS_DSP_SOURCE_DIR}/SupportFunctions/SupportFunctions.c
    ${CMSIS_DSP_SOURCE_DIR}/BasicMathFunctions/BasicMathFunctions.c
    ${CMSIS_DSP_SOURCE_DIR}/FastMathFunctions/FastMathFunctions.c
    ${CMSIS_DSP_SOURCE_DIR}/FilteringFunctions/FilteringFunctions.c
    ${CMSIS_DSP_SOURCE_DIR}/CommonTables/CommonTables.c
    ${CMSIS_DSP_COMPUTELIB_SRC_DIR}/arm_cl_tables.c
)

add_subdirectory(synth_dexed)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(picodexed PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/synth_dexed
    ${CMAKE_CURRENT_LIST_DIR}/synth_dexed/Synth_Dexed/src
	${CMSIS_CORE_INCLUDE_DIR}
	${CMSIS_DSP_INCLUDE_DIR}
	${CMSIS_DSP_PRIVATE_INCLUDE_DIR}
	${CMSIS_DSP_COMPUTELIB_INCLUDE_DIR}
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(picodexed PUBLIC synth_dexed pico_stdlib tinyusb_device tinyusb_board pico_audio_i2s)

target_compile_definitions(picodexed PRIVATE
	USE_AUDIO_I2S=1
)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(picodexed PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(picodexed)
