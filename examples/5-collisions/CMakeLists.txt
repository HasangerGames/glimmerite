add_executable(5-collisions collisions.cpp)

target_link_libraries(5-collisions PRIVATE glimmerite::client)

install(TARGETS 5-collisions DESTINATION examples)
