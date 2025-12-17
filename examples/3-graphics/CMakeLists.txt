add_executable(3-graphics graphics.cpp)

target_link_libraries(3-graphics PRIVATE glimmerite)

install(TARGETS 3-graphics DESTINATION examples)
