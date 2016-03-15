include(${CMAKE_CURRENT_LIST_DIR}/all.cmake)

set(ACTUAL_SYSTEM_NAME "arm-linux" CACHE STRING "" FORCE)

set(EVA_SDKROOT "${EVA_SDKS_PREFIX}/arm-linux")

set(CMAKE_SYSTEM_NAME "Linux")
set(TRIPLE arm-linux-gnueabihf)

set(SYSROOT "${EVA_SDKROOT}/${TRIPLE}/sysroot")

set(CMAKE_SYSROOT "${SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# If we don't unset CMAKE_MODULE_PATH, we get all of the Darwin CMake modules,
# since CMake is technically installed in the Darwin SDK.
unset(CMAKE_MODULE_PATH)
list(APPEND CMAKE_MODULE_PATH "${SYSROOT}/usr/lib/cmake")

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_AR "${EVA_SDKROOT}/bin/arm-linux-gnueabihf-ar" CACHE STRING "" FORCE)
set(CMAKE_RANLIB "${EVA_SDKROOT}/bin/arm-linux-gnueabihf-ranlib" CACHE STRING "" FORCE)

set(CLANG_FLAGS "-target arm-linux-gnueabihf -march=armv7a --gcc-toolchain=${EVA_SDKROOT} -B${EVA_SDKROOT}/bin --sysroot=${SYSROOT} -fblocks -Qunused-arguments")
set(CLANG_FLAGS "${CLANG_FLAGS} -nostdinc -isystem ${EVA_SDKROOT}/lib/gcc/arm-linux-gnueabihf/gcc-current/include -isystem ${EVA_SDKROOT}/lib/gcc/arm-linux-gnueabihf/gcc-current/include-fixed -isystem ${SYSROOT}/usr/include")

set(CMAKE_C_FLAGS "${CLANG_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CLANG_FLAGS}" CACHE STRING "" FORCE)

set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_INSTALL_PREFIX "/usr" CACHE STRING "" FORCE)

add_definitions(-DTARGET_OS_LINUX=1)
