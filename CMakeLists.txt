cmake_minimum_required (VERSION 3.6)

project (CM7-DSP)
enable_language (C CXX ASM)

set (TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools")
set (ST_LIB_DIR "${CMAKE_SOURCE_DIR}/STM32Cube_FW_F7_V1.15.0")

find_package(Boost)
include_directories (
    ${CMAKE_SOURCE_DIR}/inc
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc
    ${ST_LIB_DIR}/Drivers/BSP/STM32F7xx_Nucleo_144/
    ${ST_LIB_DIR}/Drivers/CMSIS/Include
    ${ST_LIB_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${Boost_INCLUDE_DIRS}
)
# Identify linker directories
link_directories(
    ${ST_LIB_DIR}/Drivers/CMSIS/Lib/GCC
)

# ========================================================================
set (STARTUP
    ${CMAKE_SOURCE_DIR}/src/startup_stm32f722xx.s
)

set (STM_SRC
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc_ex.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
    ${ST_LIB_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
)

set (DEMO_SRC
    ${CMAKE_SOURCE_DIR}/src/stm32f7xx_nucleo_144.c
    ${CMAKE_SOURCE_DIR}/src/stm32f7xx_hal_msp.c
    ${CMAKE_SOURCE_DIR}/src/stm32f7xx_it.c
    ${CMAKE_SOURCE_DIR}/src/system_stm32f7xx.c
    ${CMAKE_SOURCE_DIR}/src/main.c
)

set (APP_SRC
    ${CMAKE_SOURCE_DIR}/src/system_stm32f7xx.c
    ${CMAKE_SOURCE_DIR}/src/handlers.c
    ${CMAKE_SOURCE_DIR}/src/core.cc
    ${CMAKE_SOURCE_DIR}/src/dsp_adc.cc
    ${CMAKE_SOURCE_DIR}/src/main.cc
)

set (LINKER_SCRIPT 
    ${CMAKE_SOURCE_DIR}/linker/STM32F722ZETx_FLASH.ld)
# ========================================================================
set (CM_SYSTEM_DEFINE
    "-DSTM32F722xx -DARM_MATH_CM7 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER"
)

set (CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} \
    -mcpu=cortex-m7 -mthumb \
    -mfloat-abi=hard -mfpu=fpv5-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    ${CM_SYSTEM_DEFINE}"
)

set (CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} \
    -mcpu=cortex-m7 -mthumb \
    -mfloat-abi=hard -mfpu=fpv5-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    -Wall \
    -std=c++11 \
    ${CM_SYSTEM_DEFINE}"
)

set (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} \
    --specs=nano.specs \
    -Wl,--no-wchar-size-warning -lc \
    -Wl,-gc-sections"
)

# ========================================================================
# DEBUG prefixed variables automatically add to the base variables above.
set (CMAKE_C_FLAGS_DEBUG
    "-O0 -g \
    -DBUILD_SEMIHOSTING_ENABLE"
)

set (CMAKE_CXX_FLAGS_DEBUG 
    "-O0 -g \
    -DBUILD_SEMIHOSTING_ENABLE"
)

set (CMAKE_EXE_LINKER_FLAGS_DEBUG
    "--specs=rdimon.specs \
    -u _scanf_float -u _printf_float"
)
# ========================================================================
set (CMAKE_C_FLAGS_RELEASE
    "-O2 -g"
)

set (CMAKE_CXX_FLAGS_RELEASE
    "-O2 -g"
)

set (CMAKE_EXE_LINKER_FLAGS_RELEASE
    "--specs=nosys.specs"
)
# ========================================================================

add_executable (${PROJECT_NAME}-DEMO ${STARTUP} ${STM_SRC} ${DEMO_SRC})
target_link_libraries (${PROJECT_NAME}-DEMO arm_cortexM7l_math)

add_executable (${PROJECT_NAME} ${STARTUP} ${STM_SRC} ${APP_SRC})
target_link_libraries (${PROJECT_NAME} arm_cortexM7l_math)

