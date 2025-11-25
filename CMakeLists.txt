cmake_minimum_required(VERSION 4.0)
project(glimmerite)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE GLIMMERITE_SRC
        "src/*.h" "src/*.cpp"
        "include/*.h" "include/*.cpp"
)

add_library(glimmerite STATIC ${GLIMMERITE_SRC})

add_executable(helloworld examples/1-helloworld/helloworld.cpp)

target_include_directories(glimmerite
    PUBLIC
    include
    ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(glimmerite PRIVATE SDL3::SDL3)
target_link_libraries(helloworld PRIVATE glimmerite SDL3::SDL3)
