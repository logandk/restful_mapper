execute_process(COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.bak)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeLists.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

