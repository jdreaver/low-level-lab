#!/usr/bin/env bash

# Updates header files from ST and ARM

set -eu

# Remove existing files
rm -f ./*.h ./*.s ./*.ld

# Fetch core files from CMSIS_5 repo
cmsis_5_url_base="https://raw.githubusercontent.com/ARM-software/CMSIS_5"

declare -a cmsis_5_paths=(
    "CMSIS/Core/Include/cmsis_compiler.h"
    "CMSIS/Core/Include/cmsis_gcc.h"
    "CMSIS/Core/Include/cmsis_version.h"
    "CMSIS/Core/Include/core_cm4.h"
    "CMSIS/Core/Include/mpu_armv7.h"
)

for path in "${cmsis_5_paths[@]}"; do
    wget "$cmsis_5_url_base/master/$path"
done

# Fetch files from the STM32CubeF4 repo
stm_cube_f4_url_base="https://raw.githubusercontent.com/STMicroelectronics/STM32CubeF4"

declare -a stm_cube_f4_paths=(
    "Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xe.s"
    "Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f401xe.h"
    "Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
    "Projects/STM32F401RE-Nucleo/Templates_LL/SW4STM32/NUCLEO-F401RE/STM32F401RETx_FLASH.ld"
)

for path in "${stm_cube_f4_paths[@]}"; do
    wget "$stm_cube_f4_url_base/master/$path"
done

# ST seems to use this file for keeping track of clock settings in their
# auto-generated applications. But we're managing that ourselves. This file only
# exists so we don't have to comment out a line in all of the device header
# files which contain macro definitions for all of the peripheral registers.
echo '#pragma once' > system_stm32f4xx.h
