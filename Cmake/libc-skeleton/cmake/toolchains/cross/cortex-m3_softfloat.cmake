if($ENV{ARM_CORTEX_M3_TOOLCHAIN_INCLUDED})
	return()
endif()

set(ENV{ARM_CORTEX_M3_TOOLCHAIN_INCLUDED} true)

if(NOT CPU_NAME)
	set(CPU_NAME cortex-m3)
endif()

set(CPU_FLAGS, "-mcpu=cortex-m3 -mthumb ${CPU_FLAGS}")
set(VFP_FLAGS, "-mfloat-abi=soft ${VFP_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake)
