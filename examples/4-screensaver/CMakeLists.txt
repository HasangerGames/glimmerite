add_executable(4-screensaver screensaver.cpp)

target_link_libraries(4-screensaver PRIVATE glimmerite::client)

install(TARGETS 4-screensaver DESTINATION examples)
