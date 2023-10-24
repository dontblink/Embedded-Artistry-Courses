if($ENV{ARM_CORTEX_M4_TOOLCHAIN_INCLUDED})
    return()
endif()

set(ENV{ARM_CORTEX_M4_TOOLCHAIN_INCLUDED} true)

set(CPU_NAME cortex-m4)
set(CPU_FLAGS, "-mcpu=cortex-m4 -mthumb")
set(VFP_FLAGS, "-mfloat-abi=hard -mfpu=fpv4-sp-d16 ")

include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake)
