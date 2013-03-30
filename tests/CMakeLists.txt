cmake_minimum_required(VERSION 2.6)
project(tests)

if (MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

set (gtest_disable_pthreads 1)

set(GTEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/gtest)
add_subdirectory(${GTEST_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/gtest)
include_directories(${GTEST_ROOT}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/RestfulMapper)

FILE(GLOB test_files "test_*.cpp")
add_executable(tests ${test_files})
target_link_libraries(tests gtest gtest_main restful_mapper)

