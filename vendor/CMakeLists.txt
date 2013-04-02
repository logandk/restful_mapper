cmake_minimum_required(VERSION 2.6)
project(vendor)
include(ExternalProject)

set(CURL_FLAGS -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCURL_STATICLIB=ON -DCMAKE_USE_OPENSSL=OFF -DCURL_ZLIB=OFF -DHTTP_ONLY=ON)

if (BORLAND)
  set(CURL_PATCH_COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/curl/borland-patch.cmake)
endif()

if (WIN32)
  ExternalProject_Add(
    curl
    URL http://curl.haxx.se/download/curl-7.29.0.tar.gz
    PREFIX curl
    PATCH_COMMAND ${CURL_PATCH_COMMAND}
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/curl ${CURL_FLAGS}
  )
endif()

ExternalProject_Add(
  yajl
  URL https://github.com/lloyd/yajl/archive/2.0.4.tar.gz
  PREFIX yajl
  PATCH_COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/yajl/patch.cmake
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/yajl
)

if (WIN32)
  ExternalProject_Add(
    iconv
    URL http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
    PREFIX iconv
    PATCH_COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/iconv/patch.cmake
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/iconv
  )
endif()

if (WIN32)
  set(GTEST_FLAGS -Dgtest_disable_pthreads=1)
endif()

ExternalProject_Add(
  gtest
  URL https://googletest.googlecode.com/files/gtest-1.6.0.zip
  PREFIX gtest
  INSTALL_COMMAND ${CMAKE_COMMAND} -DCMAKE_STATIC_LIBRARY_PREFIX=${CMAKE_STATIC_LIBRARY_PREFIX} -DCMAKE_STATIC_LIBRARY_SUFFIX=${CMAKE_STATIC_LIBRARY_SUFFIX} -P ${CMAKE_CURRENT_SOURCE_DIR}/gtest/install.cmake
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/gtest ${GTEST_FLAGS}
)

