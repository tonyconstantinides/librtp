cmake_minimum_required(VERSION 3.3)

set(CMAKE_XCODE_ORGANIZATIONNAME "EVA Automation, Inc.")

project(evartp)

list(
    APPEND SOURCE_FILES
    src/RtspManager.cpp
    src/runnable/main.cpp
)

list(
    APPEND HEADER_FILES
    src/RtspManager.hpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wno-unknown-pragmas")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    find_library(LIBBLOCKSRUNTIME BlocksRuntime REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${LIBBLOCKSRUNTIME})
endif()

find_library(LIBFOUNDATION jetpack_foundation REQUIRED)
find_path(FOUNDATION_INCLUDE_DIR foundation/foundation.hpp PATH_SUFFIXES jetpack)
target_link_libraries(${PROJECT_NAME} ${LIBFOUNDATION})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${FOUNDATION_INCLUDE_DIR})

# find_library(LIBGSTREAMER gstreamer REQUIRED)
# find_path(GSTREAMER_INCLUDE_DIR gstreamer/gstreamer.h PATH_SUFFIXES gstreamer)
# target_link_libraries(${PROJECT_NAME} ${LIBGSTREAMER})
# target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${GSTREAMER_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# an example of installing the headers, fix up once we know where the headers are located
# install(
#     DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/"
#     DESTINATION lib
#     FILES_MATCHING PATTERN "*${CMAKE_${PLATFORM_LIBRARY_TYPE}_LIBRARY_SUFFIX}"
#     PATTERN "CMakeFiles" EXCLUDE
# )
