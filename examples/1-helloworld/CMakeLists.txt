add_executable(helloworld helloworld.cpp)

target_link_libraries(
    helloworld
    PRIVATE glimmerite SDL3::SDL3 bgfx bx bimg_decode
)

install(TARGETS helloworld DESTINATION examples)
