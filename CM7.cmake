set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# This first option allows the baremetal compiler to pass the compiler check.
set(GCC_PREFIX /opt/gcc-arm-none-eabi-7-2018-q2-update/bin/)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_ASM_COMPILER ${GCC_PREFIX}/arm-none-eabi-gcc)
set(CMAKE_C_COMPILER   ${GCC_PREFIX}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${GCC_PREFIX}/arm-none-eabi-g++)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
