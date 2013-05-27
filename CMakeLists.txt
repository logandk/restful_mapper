cmake_minimum_required(VERSION 2.6)
project(RestfulMapper)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(BUILD_SHARED_LIBS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/yajl/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/yajl/lib)

add_definitions(-DCURL_STATICLIB)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/curl/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/curl/lib)

add_definitions(-DLIBICONV_PLUG)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/iconv/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/iconv/lib)

if (BORLAND)
  add_definitions(-w-hid)
endif()

add_library(restful_mapper src/api.cpp src/json.cpp src/utf8.cpp)
target_link_libraries(restful_mapper curl yajl iconv charset)

install(TARGETS restful_mapper DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper.h DESTINATION include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/api.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/field.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/helpers.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/internal/iso8601.h DESTINATION include/restful_mapper/internal)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/json.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/mapper.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/meta.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/model.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/model_collection.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/query.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/relation.h DESTINATION include/restful_mapper)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/restful_mapper/internal/utf8.h DESTINATION include/restful_mapper/internal)

