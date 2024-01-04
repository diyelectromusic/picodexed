set (CMSIS_DIR ${CMAKE_CURRENT_LIST_DIR}/../cmsis/cmsis-pi-pico/CMSIS)
set (CMSIS_CORE_INCLUDE_DIR ${CMSIS_DIR}/Core/Include)
set (CMSIS_DSP_INCLUDE_DIR ${CMSIS_DIR}/DSP/Include)
set (CMSIS_DSP_PRIVATE_INCLUDE_DIR ${CMSIS_DIR}/DSP/PrivateInclude)
set (CMSIS_DSP_COMPUTELIB_INCLUDE_DIR ${CMSIS_DIR}/DSP/ComputeLibrary/Include)
set (CMSIS_DSP_SOURCE_DIR ${CMSIS_DIR}/DSP/Source)
set (CMSIS_DSP_COMPUTELIB_SRC_DIR ${CMSIS_DIR}/DSP/ComputeLibrary/Source)

add_library(synth_dexed
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/PluginFx.cpp 
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/dexed.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/dx7note.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/env.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/exp2.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/fm_core.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/fm_op_kernel.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/freqlut.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/lfo.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/pitchenv.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/porta.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/sin.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/EngineMkI.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/EngineOpl.cpp
       ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src/EngineMsfa.cpp
)

target_include_directories(synth_dexed INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Synth_Dexed/src
PRIVATE ${CMAKE_CURRENT_LIST_DIR}
PUBLIC
	${CMSIS_CORE_INCLUDE_DIR}
	${CMSIS_DSP_INCLUDE_DIR}
	${CMSIS_DSP_PRIVATE_INCLUDE_DIR}
	${CMSIS_DSP_COMPUTELIB_INCLUDE_DIR}
)

target_link_libraries(synth_dexed PUBLIC pico_stdlib)
