cmake_minimum_required(VERSION 2.8)
set(OE_PROJECT_NAME OpenEmulator)
project(${OE_PROJECT_NAME} C CXX)

# Set up convenient variables
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(OE_LIBRARY_CMAKES ${SOURCE_DIR}/cmake)

# Include some custom FIND_PACKAGE macros.
#include(${OE_LIBRARY_CMAKES}/FindPulseAudio.cmake)

# Make sure all the neccessary libraries are installed
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
#FIND_PACKAGE(wxWidgets REQUIRED net gl core base)
#INCLUDE(${wxWidgets_USE_FILE})


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-D_LINUX)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux") 

add_definitions(-DGL_GLEXT_PROTOTYPES)

# Set up OE library paths
set(LIBDISKIMAGE_DIR ${SOURCE_DIR}/libdiskimage)
set(LIBEMULATION_DIR ${SOURCE_DIR}/libemulation)
set(LIBEMULATION_HAL_DIR ${SOURCE_DIR}/libemulation-hal)
set(LIBUTIL_DIR ${SOURCE_DIR}/libutil)

# Get sources and headers for the OE libraries
include(${OE_LIBRARY_CMAKES}/libdiskimage.cmake)
include(${OE_LIBRARY_CMAKES}/libemulation.cmake)
include(${OE_LIBRARY_CMAKES}/libemulation-hal.cmake)
include(${OE_LIBRARY_CMAKES}/libutil.cmake)

# Setup include directories
include_directories(
  ${LIBXML2_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${LIBDISKIMAGE_INCLUDE_DIRS}
  ${LIBEMULATION_INCLUDE_DIRS}
  ${LIBEMULATION_HAL_INCLUDE_DIR}
  ${LIBUTIL_INCLUDE_DIRS}
  /opt/local/include)

set(OE_SRCS
  ${SOURCE_DIR}/wx/main.cpp)

set(OE_EXECUTABLE openemulator)

add_executable(${OE_EXECUTABLE} ${OE_SRCS})
target_link_libraries(${OE_EXECUTABLE} 
  ${LIBXML2_LIBRARIES}
  ${PNG_LIBRARY}
  ${OPENGL_LIBRARIES}
  ${LIBDISKIMAGE}
  ${LIBEMULATION}
  ${LIBEMULATION_HAL}
  ${LIBUTIL}
  /opt/local/lib)
