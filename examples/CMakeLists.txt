add_subdirectory(1-helloworld)
add_subdirectory(2-sprites)
add_subdirectory(3-graphics)
add_subdirectory(4-screensaver)
add_subdirectory(5-collisions)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_BINARY_DIR}/Debug/assets)
