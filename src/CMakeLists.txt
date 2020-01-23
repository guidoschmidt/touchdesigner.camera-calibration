project(CameraCalibration)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
include_directories(BEFORE ${OpenCV_INCLUDE_DIRS})

add_executable(CameraCalibrator
  App.cpp
  CameraCalibrator.cpp)
target_include_directories(CameraCalibrator PRIVATE .)
target_link_libraries(CameraCalibrator ${OpenCV_LIBS})

add_library(CameraCalibrationDAT
  SHARED
  ./CameraCalibrator.cpp
  ./touchdesigner/OpenCVCameraCalibrationDAT.cpp)
target_include_directories(CameraCalibrationDAT
  PRIVATE
  .
  ./touchdesigner/)
source_group("Header Files"
  FILES
  ./touchdesigner/CPlusPlus_Common.h
  ./touchdesigner/DAT_CPlusPlusBase.h
  ./touchdesigner/GL_Extensions.h
  ./tochdesigner/OpenCVCameraCalibrationDAT.hpp)
target_link_libraries(CameraCalibrationDAT ${OpenCV_LIBS})
