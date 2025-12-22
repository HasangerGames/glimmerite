add_executable(1-helloworld helloworld.cpp)

target_link_libraries(1-helloworld PRIVATE GMI_client)

install(TARGETS 1-helloworld DESTINATION examples)
