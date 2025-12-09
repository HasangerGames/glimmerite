cmake_minimum_required(VERSION 4.0)
project(glimmerite)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE GLIMMERITE_SRC
    "src/*.h" "src/*.cpp"
    "include/*.h" "include/*.cpp"
    "shaders/*.sc"
)

add_library(glimmerite STATIC ${GLIMMERITE_SRC})

add_executable(helloworld examples/1-helloworld/helloworld.cpp)

set(BGFX_DIR "${CMAKE_CURRENT_LIST_DIR}/vendored/bgfx.cmake/bgfx")
set(BX_DIR "${CMAKE_CURRENT_LIST_DIR}/vendored/bgfx.cmake/bx")
set(BIMG_DIR "${CMAKE_CURRENT_LIST_DIR}/vendored/bgfx.cmake/bimg")

# makes cmake shut up about bgfx.cmake's use of add_custom_command
set(CMAKE_POLICY_DEFAULT_CMP0175 "OLD")
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

add_subdirectory(vendored/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(vendored/glaze EXCLUDE_FROM_ALL)

target_include_directories(glimmerite
    PUBLIC
    src
    include
)

bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS shaders/vs_sprite.sc
    VARYING_DEF ${CMAKE_CURRENT_LIST_DIR}/shaders/varying.def.sc
    INCLUDE_DIRS ${BGFX_DIR}/src
    OUTPUT_DIR ../src/shaders
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS shaders/fs_sprite.sc
    VARYING_DEF ${CMAKE_CURRENT_LIST_DIR}/shaders/varying.def.sc
    INCLUDE_DIRS ${BGFX_DIR}/src
    OUTPUT_DIR ../src/shaders
    AS_HEADERS
)

target_link_libraries(glimmerite PUBLIC
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    bgfx
    bx
    bimg_decode
    glaze::glaze
)
target_link_libraries(helloworld PRIVATE glimmerite SDL3::SDL3 bgfx bx bimg_decode)
