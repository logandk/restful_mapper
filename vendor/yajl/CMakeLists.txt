cmake_minimum_required(VERSION 2.6)
project(yajl)

SET (YAJL_MAJOR 2)
SET (YAJL_MINOR 0)
SET (YAJL_MICRO 4)

if (BORLAND)
  add_definitions(-DLLONG_MAX=_I64_MAX)
  add_definitions(-DLLONG_MIN=_I64_MIN)
endif()

if (BORLAND OR MSYS OR MINGW)
  add_definitions(-Dsprintf_s=snprintf)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/api/yajl_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/api/yajl_common.h ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_common.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/api/yajl_gen.h ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_gen.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/api/yajl_parse.h ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_parse.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/api/yajl_tree.h ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_tree.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_library(yajl src/yajl.c src/yajl_lex.c src/yajl_parser.c src/yajl_buf.c src/yajl_encode.c src/yajl_gen.c src/yajl_alloc.c src/yajl_tree.c src/yajl_version.c)

install(TARGETS yajl DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_version.h DESTINATION include/yajl)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_common.h DESTINATION include/yajl)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_gen.h DESTINATION include/yajl)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_parse.h DESTINATION include/yajl)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/yajl/yajl_tree.h DESTINATION include/yajl)

