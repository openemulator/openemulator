####################################################
# MAIN OPENEMULATOR CMAKEFILE                      #  
#                                                  #
# Maintained by Tobias Eriksson <tobier@tobier.se> #
####################################################
cmake_minimum_required(VERSION 2.8)
set(OE_PROJECT_NAME OpenEmulator)
project(${OE_PROJECT_NAME} C CXX)

####################################################
# PREREQUISITES                                    #
####################################################
# Set up convenient directory variables
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(OE_LIBRARY_CMAKES ${SOURCE_DIR}/cmake)

# Make sure we find any custom CMake modules
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OE_LIBRARY_CMAKES} )

# Set up OE library paths
set(LIBDISKIMAGE_DIR ${SOURCE_DIR}/libdiskimage)
set(LIBEMULATION_DIR ${SOURCE_DIR}/libemulation)
set(LIBEMULATION_HAL_DIR ${SOURCE_DIR}/libemulation-hal)
set(LIBUTIL_DIR ${SOURCE_DIR}/libutil)

####################################################
# DEPENDENCIES                                     #
####################################################
FIND_PACKAGE(LibZip REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(PortAudio REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(LibSndFile REQUIRED)
FIND_PACKAGE(SampleRate REQUIRED)
FIND_PACKAGE(wxWidgets REQUIRED net gl core base)

# wxWidgets
SET(wxWidgets_USE_LIBS)
INCLUDE(${wxWidgets_USE_FILE})

####################################################
# DEFINITIONS                                      #
####################################################
add_definitions(-DGL_GLEXT_PROTOTYPES)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-D_LINUX)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux") 

####################################################
# SOURCES                                          #
####################################################
# Get sources and headers for the OE libraries
include(${OE_LIBRARY_CMAKES}/libdiskimage.cmake)
include(${OE_LIBRARY_CMAKES}/libemulation.cmake)
include(${OE_LIBRARY_CMAKES}/libemulation-hal.cmake)
include(${OE_LIBRARY_CMAKES}/libutil.cmake)

# Add the sources for the GUI application
set(OE_SRCS
  ${SOURCE_DIR}/wx/OEApp.cpp)

####################################################
# HEADERS                                          #
####################################################
# Setup include directories
include_directories(
  ${LIBDISKIMAGE_INCLUDE_DIRS}
  ${LIBEMULATION_INCLUDE_DIRS}
  ${LIBEMULATION_HAL_INCLUDE_DIR}
  ${LIBUTIL_INCLUDE_DIRS}
  ${SAMPLERATE_INCLUDE_DIR}
  ${LIBSNDFILE_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
  ${PORTAUDIO_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${LIBZIP_INCLUDE_DIR})

####################################################
# LINKING                                          #
####################################################
set(OE_EXECUTABLE openemulator)
add_executable(${OE_EXECUTABLE} ${OE_SRCS})
target_link_libraries(${OE_EXECUTABLE} 
  ${LIBEMULATION_HAL}
  ${LIBEMULATION}
  ${LIBDISKIMAGE}
  ${LIBUTIL}
  ${SAMPLERATE_LIBRARY}
  ${LIBSNDFILE_LIBRARY}
  ${PNG_LIBRARIES}
  ${PORTAUDIO_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
  ${LIBXML2_LIBRARIES}
  ${LIBZIP_LIBRARY}
  ${wxWidgets_LIBRARIES})