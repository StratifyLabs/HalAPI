cmake_minimum_required(VERSION 3.12)

project(HalAPI CXX)
include(CTest)

add_subdirectory(library library)
option(HAL_API_IS_TEST "Enable test builds for HalAPI" OFF)

if (HAL_API_IS_TEST)
  add_subdirectory(tests tests)
endif ()

