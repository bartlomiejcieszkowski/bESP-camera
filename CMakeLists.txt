# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)


set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} Micro-RTSP/micro_rtsp_servera")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(console_cam)

#list(APPEND EXTRA_COMPONENT_DIRS "../Micro-RTSP/micro_rtsp")
