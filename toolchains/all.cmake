set(EVA_SDKS_PREFIX "/usr/local/eva/sdks")

######################################
# COMPILATION FLAGS
######################################

# ಠ_ಠ
# http://stackoverflow.com/questions/19866926/cmake-toolchain-file-wont-use-my-custom-cflags

set(CMAKE_CONFIGURATION_TYPES Debug Release Production CACHE TYPE INTERNAL FORCE)

set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_PRODUCTION "-O3" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_PRODUCTION "-O3" CACHE STRING "" FORCE)

macro(append_flags flags) # configs
    foreach(config ${ARGN})
        set(CMAKE_C_FLAGS_${config} "${CMAKE_C_FLAGS_${config}} ${flags}")
        set(CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}} ${flags}")
    endforeach()
endmacro()

append_flags("-g" DEBUG RELEASE PRODUCTION)
append_flags("-DENGINEERING_BUILD" DEBUG RELEASE)
append_flags("-DNDEBUG" RELEASE PRODUCTION)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "" FORCE)
set(CMAKE_XCODE_ATTRIBUTE_RUN_CLANG_STATIC_ANALYZER YES CACHE STRING "" FORCE)
