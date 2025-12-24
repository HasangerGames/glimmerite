add_executable(2-sprites sprites.cpp)

target_link_libraries(2-sprites PRIVATE glimmerite::client)

install(TARGETS 2-sprites DESTINATION examples)
