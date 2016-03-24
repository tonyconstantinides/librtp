cmake_minimum_required(VERSION 3.3)

set(CMAKE_XCODE_ORGANIZATIONNAME "EVA Automation, Inc.")

project(evartp)

add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wno-unknown-pragmas -Wno-format")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

