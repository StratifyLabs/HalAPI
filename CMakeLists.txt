cmake_minimum_required(VERSION 3.12)
cmsdk2_minimum_required(2.1.2)

project(HalAPI
  LANGUAGES CXX
  VERSION 1.3.1)
include(CTest)

add_subdirectory(library library)
option(HAL_API_IS_TEST "Enable test builds for HalAPI" OFF)

if (HAL_API_IS_TEST)
  add_subdirectory(tests tests)
endif ()

