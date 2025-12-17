cmake_minimum_required(VERSION 3.31)

project(glimmerite CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

# enable testing
if (BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(BGFX_DIR "${CMAKE_CURRENT_LIST_DIR}/vendored/bgfx.cmake/bgfx")
set(BX_DIR "${CMAKE_CURRENT_LIST_DIR}/vendored/bgfx.cmake/bx")
set(BIMG_DIR "${CMAKE_CURRENT_LIST_DIR}/vendored/bgfx.cmake/bimg")
# makes cmake shut up about bgfx.cmake's use of add_custom_command
set(CMAKE_POLICY_DEFAULT_CMP0175 "OLD")
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

add_subdirectory(vendored/bgfx.cmake EXCLUDE_FROM_ALL)

# TODO: use system package once newer SDL3 releases
# currently SDL3_Mixer depends on SDL3 git master
# find_package(SDL3 CONFIG COMPONENTS SDL3-shared)
# if (NOT SDL3_FOUND)

# disable SDL subsystems we don't need
set(SDL_GPU OFF CACHE INTERNAL "" FORCE)
set(SDL_RENDER OFF CACHE INTERNAL "" FORCE)
set(SDL_CAMERA OFF CACHE INTERNAL "" FORCE)
set(SDL_POWER OFF CACHE INTERNAL "" FORCE)
set(SDL_SENSOR OFF CACHE INTERNAL "" FORCE)
set(SDL_DIALOG OFF CACHE INTERNAL "" FORCE)
set(SDL_TRAY OFF CACHE INTERNAL "" FORCE)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
#endif()

add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)

find_package(glaze CONFIG)
if (NOT glaze_FOUND)
    add_subdirectory(vendored/glaze EXCLUDE_FROM_ALL)
endif()

set(EARCUT_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(EARCUT_BUILD_BENCH OFF CACHE INTERNAL "" FORCE)
set(EARCUT_BUILD_VIZ OFF CACHE INTERNAL "" FORCE)
add_subdirectory(vendored/earcut.hpp EXCLUDE_FROM_ALL)

#
# Sprite shader
#
bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/vs_sprite.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      ${CMAKE_BINARY_DIR}/include/generated/shaders
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/fs_sprite.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      ${CMAKE_BINARY_DIR}/include/generated/shaders
    AS_HEADERS
)

#
# Color shader
#
bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/color/vs_color.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/color/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      ${CMAKE_BINARY_DIR}/include/generated/shaders
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/color/fs_color.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/color/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      ${CMAKE_BINARY_DIR}/include/generated/shaders
    AS_HEADERS
)

add_library(glimmerite STATIC
    src/Application.cpp
    src/Container.cpp
    src/Graphics.cpp
    src/Renderer.cpp
    src/SoundManager.cpp
    src/Sprite.cpp
    src/TextureManager.cpp
    src/TweenManager.cpp
    src/gmi.cpp

    shaders/sprite/varying.def.sc
    shaders/sprite/fs_sprite.sc
    shaders/sprite/vs_sprite.sc

    shaders/color/varying.def.sc
    shaders/color/fs_color.sc
    shaders/color/vs_color.sc
)

target_include_directories(glimmerite PUBLIC include)

target_include_directories(glimmerite PRIVATE ${CMAKE_BINARY_DIR}/include/generated/shaders)

target_link_libraries(glimmerite PUBLIC
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    bgfx
    bx
    bimg_decode
    glaze::glaze
    earcut_hpp
)

option(BUILD_EXAMPLES "Build example games" ON)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

feature_summary(WHAT ALL)
