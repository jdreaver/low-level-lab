#!/usr/bin/env bash

# Updates header files from ST and ARM

set -eu

# Fetch files from the STM32CubeF4 repo
stm_cube_f4_url_base="https://raw.githubusercontent.com/STMicroelectronics/STM32CubeF4"

# TODO: The STM32CubeF4 repo includes some core ARM files, but they are quite
# old. Maybe fetch them from ARM directly?
declare -a cmsis_paths=(
    "Drivers/CMSIS/Core/Include/cmsis_compiler.h"
    "Drivers/CMSIS/Core/Include/cmsis_gcc.h"
    "Drivers/CMSIS/Core/Include/cmsis_version.h"
    "Drivers/CMSIS/Core/Include/core_cm4.h"
    "Drivers/CMSIS/Core/Include/mpu_armv7.h"
    "Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xe.s"
    "Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f401xe.h"
    "Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
    "Projects/STM32F401RE-Nucleo/Templates_LL/SW4STM32/NUCLEO-F401RE/STM32F401RETx_FLASH.ld"
)

for path in "${cmsis_paths[@]}"; do
    wget "$stm_cube_f4_url_base/master/$path"
done

# ST seems to use this file for keeping track of clock settings in their
# auto-generated applications. But we're managing that ourselves. This file only
# exists so we don't have to comment out a line in all of the device header
# files which contain macro definitions for all of the peripheral registers.
echo '#pragma once' > system_stm32f4xx.h
