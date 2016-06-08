cmake_minimum_required(VERSION 3.3)

set(CMAKE_XCODE_ORGANIZATIONNAME "EVA Automation, Inc.")

list(
    APPEND SOURCE_FILES
    IPStreamManager.cpp
    RtspManager.cpp
    MjpegManager.cpp
    StreamManager.cpp
	CamParamsEncryption.cpp
    StreamErrorHandler.cpp
)

list(
    APPEND HEADER_FILES
    IPStreamManager.hpp
    RtspManager.hpp
    MjpegManager.hpp
    StreamManager.hpp	
    CamParamsEncryption.hpp
    StreamErrorHandler.hpp
    CamNotificationDefs.hpp
    Common.hpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    find_library(LIBBLOCKSRUNTIME BlocksRuntime REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${LIBBLOCKSRUNTIME})
endif()

find_library(LIBFOUNDATION jetpack_foundation REQUIRED)
find_library(LIBGSTREAMER gstreamer-1.0 REQUIRED)
find_library(LIBGSTRTSP gstrtsp-1.0 REQUIRED)
find_library(LIBGLIB glib-2.0 REQUIRED)
find_library(LIBGOBJECT gobject-2.0 REQUIRED)
find_library(LIBGIO gio-2.0 REQUIRED)
FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  MESSAGE(STATUS "Curl libraries found at: ${CURL_LIBRARIES}")
  MESSAGE(STATUS "Curl includes found at: ${CURL_INCLUDE_DIRS}")
else()
  MESSAGE(SEND_ERROR "Could not find Curl on your system")
ENDIF(CURL_FOUND)
target_link_libraries(${PROJECT_NAME} ${LIBFOUNDATION} ${LIBGSTREAMER} ${LIBGSTRTSP} ${LIBGLIB} ${LIBGOBJECT} ${LIBGIO}  ${CURL_LIBRARIES})

find_path(FOUNDATION_INCLUDE_DIR foundation/foundation.hpp PATH_SUFFIXES jetpack)
find_path(GSTCONFIG_INCLUDE_DIR /gst/gstconfig.h PATH_SUFFIXES lib/gstreamer-1.0/include)
find_path(GST_INCLUDE_DIR /gst/gst.h PATH_SUFFIXES gstreamer-1.0)
find_path(RTSP_INCLUDE_DIR /gst/rtsp/rtsp.h PATH_SUFFIXES gstreamer-1.0)
find_path(GLIB_INCLUDE_DIR glib.h PATH_SUFFIXES glib-2.0)
find_path(GIO_INCLUDE_DIR /gio/gio.h PATH_SUFFIXES glib-2.0)
find_path(GLIBCONFIG_INCLUDE_DIR glibconfig.h PATH_SUFFIXES lib/glib-2.0/include)
find_path(CURL_INCLUDE_DIRS /curl/curl.h  PATH_SUFFIXES libcurl)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${FOUNDATION_INCLUDE_DIR} ${GST_INCLUDE_DIR} ${GSTCONFIG_INCLUDE_DIR}  ${RTSP_INCLUDE_DIR} ${GLIB_INCLUDE_DIR} ${GLIBCONFIG_INCLUDE_DIR} ${GIO_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/evartp)

add_subdirectory("thirdparty")
add_subdirectory("thirdparty/catch")
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.

