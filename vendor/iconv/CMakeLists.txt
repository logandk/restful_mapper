cmake_minimum_required(VERSION 2.6)
project(iconv)

# Build libcharset
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libcharset)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libcharset/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcharset/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcharset/include/localcharset.h.in ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/include/localcharset.h)

file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/config.h "\n#define LIBDIR \"\"\n")
file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/config.h "\n#define HAVE_WORKING_O_NOFOLLOW 1\n")

add_library(charset libcharset/lib/localcharset.c)

install(TARGETS charset DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/include/localcharset.h DESTINATION include)

# Build libiconv
add_definitions(-DLIBICONV_PLUG)
if (BORLAND)
  add_definitions(-w-par)
  add_definitions(-w-csu)
  add_definitions(-w-ccc)
  add_definitions(-w-rch)
  add_definitions(-w-aus)
  add_definitions(-w-eff)
  SET(EILSEQ 2)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/iconv.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/iconv.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/lib/config.h)

file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/config.h "\n#define ICONV_CONST \"\"\n")

add_library(iconv lib/iconv.c)

install(TARGETS iconv DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/iconv.h DESTINATION include)
