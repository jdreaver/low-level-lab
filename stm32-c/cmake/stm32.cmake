# CMake configuration for compiling stm32 targets

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_EXECUTABLE_SUFFIX_C .elf)

# Tell CMake to try and make a static library when testing this compiler.
# Otherwise it tries to make an executable but that fails because of "undefined
# reference to _exit"
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set compiler/linker flags relevant to stm32
#
# My board is NUCLEO-F401RE, which has a Cortex-M4 processor
set(BOARD_NAME "STM32F401xE" CACHE STRING "Name of target board")
set(MCPU "cortex-m4" CACHE STRING "Name of target CPU")
add_compile_definitions(${BOARD_NAME})
# N.B. The combination of both --specs tells the compiler to use newlib-nano,
# which has a much, much smaller code size (it is compiled with -Os, among other
# optimizations). As a data point, a simple program with printf goes from over
# 31kB to about 5kB.
set(CMAKE_C_FLAGS_INIT "-mthumb -mthumb-interwork -mcpu=${MCPU} --specs=nano.specs --specs=nosys.specs")

function(stm32_generate_binary_file TARGET)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET}${CMAKE_EXECUTABLE_SUFFIX_C} ${TARGET}.bin
        BYPRODUCTS ${TARGET}.bin
        COMMENT "Generating binary file ${CMAKE_PROJECT_NAME}.bin"
    )
endfunction()

function(stm32_print_size_of_target TARGET)
    add_custom_target(${TARGET}_elf_size
        ALL COMMAND ${CMAKE_SIZE} ${TARGET}${CMAKE_EXECUTABLE_SUFFIX_C}
        COMMENT "Target Sizes: "
        DEPENDS ${TARGET}
    )
endfunction()

function(stm32_flash_target TARGET)
    add_custom_target(${TARGET}_flash
        COMMAND sudo st-flash write ${TARGET}.bin 0x8000000
	COMMAND sudo st-flash reset
        COMMENT "Flashing to STM32 board: "
        DEPENDS ${TARGET}
    )
endfunction()
