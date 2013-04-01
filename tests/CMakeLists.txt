cmake_minimum_required(VERSION 2.6)
project(tests)

set(BUILD_SHARED_LIBS OFF)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

#if (WIN32)
#  set (gtest_disable_pthreads 1)
#endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/gtest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/gtest/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/yajl/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/yajl/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/curl/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/curl/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/iconv/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/iconv/lib)

FILE(GLOB test_files "test_*.cpp")
add_executable(tests ${test_files})
target_link_libraries(tests gtest gtest_main restful_mapper yajl curl ws2_32 iconv charset)

