cmake_minimum_required(VERSION 2.6)
project(vendor)
include(ExternalProject)

set(CURL_FLAGS -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCURL_STATICLIB=ON)

ExternalProject_Add(
  curl
  URL http://curl.haxx.se/download/curl-7.29.0.tar.gz
  PREFIX curl
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/curl ${CURL_FLAGS}
)

ExternalProject_Add(
  yajl
  URL https://github.com/lloyd/yajl/archive/2.0.4.tar.gz
  PREFIX yajl
  PATCH_COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/yajl/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/yajl/src/yajl/CMakeLists.txt
    VERBATIM
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/yajl
)

ExternalProject_Add(
  iconv
  URL http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
  PREFIX iconv
  PATCH_COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/iconv/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/iconv/src/iconv/CMakeLists.txt
    VERBATIM
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/iconv
)

if (WIN32)
  set(GTEST_FLAGS -Dgtest_disable_pthreads=1)
endif()

ExternalProject_Add(
  gtest
  URL https://googletest.googlecode.com/files/gtest-1.6.0.zip
  PREFIX gtest
  INSTALL_COMMAND
    ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/lib
    &&
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/src/gtest-build/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/lib
    &&
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/src/gtest-build/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/lib
    &&
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/src/gtest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/include
    VERBATIM
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/gtest ${GTEST_FLAGS}
)

