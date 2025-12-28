cmake_minimum_required(VERSION 3.31)

project(glimmerite CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

option(BUILD_EXAMPLES "Build example games"  ON)
option(BUILD_CLIENT   "Build client library" ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(GMI_DIR         "${glimmerite_SOURCE_DIR}" CACHE PATH "Root directory of Glimmerite")
set(GMI_SRC_DIR     "${GMI_DIR}/src")
set(GMI_INCLUDE_DIR "${GMI_DIR}/include")

# libraries only used by the client
if (BUILD_CLIENT)
    #
    # BGFX
    #
    set(BGFX_DIR "${GMI_DIR}/vendored/bgfx.cmake/bgfx")
    set(BX_DIR   "${GMI_DIR}/vendored/bgfx.cmake/bx")
    set(BIMG_DIR "${GMI_DIR}/vendored/bgfx.cmake/bimg")

    # makes cmake shut up about bgfx.cmake's use of add_custom_command
    set(CMAKE_POLICY_DEFAULT_CMP0175 "OLD")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

    if (EMSCRIPTEN)
        set(BGFX_CONFIG_MULTITHREADED OFF CACHE INTERNAL "" FORCE)
    endif()

    add_subdirectory(vendored/bgfx.cmake EXCLUDE_FROM_ALL)

    if (EMSCRIPTEN)
        target_link_options(shaderc PRIVATE -lnodefs.js -sNODERAWFS -sSTACK_SIZE=1048576) # TODO Remove once this fix gets added to bgfx.cmake
    endif()

    #
    # SDL3
    #
    # TODO: use system package once newer SDL3 releases
    # currently SDL3_Mixer depends on SDL3 git master
    # find_package(SDL3 CONFIG COMPONENTS SDL3-shared)
    # if (NOT SDL3_FOUND)

    # disable SDL subsystems we don't need
    set(SDL_GPU     OFF CACHE INTERNAL "" FORCE)
    set(SDL_RENDER  OFF CACHE INTERNAL "" FORCE)
    set(SDL_CAMERA  OFF CACHE INTERNAL "" FORCE)
    set(SDL_POWER   OFF CACHE INTERNAL "" FORCE)
    set(SDL_SENSOR  OFF CACHE INTERNAL "" FORCE)
    set(SDL_DIALOG  OFF CACHE INTERNAL "" FORCE)
    set(SDL_TRAY    OFF CACHE INTERNAL "" FORCE)

    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    #endif()

    #
    # SDL3 Mixer, TODO: use system package when SDL3 Mixer has a stable release
    #
    add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)

    #
    # Earcut, used for rendering polygons
    #
    set(EARCUT_BUILD_TESTS  OFF CACHE INTERNAL "" FORCE)
    set(EARCUT_BUILD_BENCH  OFF CACHE INTERNAL "" FORCE)
    set(EARCUT_BUILD_VIZ    OFF CACHE INTERNAL "" FORCE)

    add_subdirectory(vendored/earcut.hpp EXCLUDE_FROM_ALL)
endif() # BUILD_CLIENT

# JSON library
find_package(glaze CONFIG)
if (NOT glaze_FOUND)
    add_subdirectory(vendored/glaze EXCLUDE_FROM_ALL)
endif()

add_subdirectory(src)

if (BUILD_EXAMPLES AND BUILD_CLIENT)
    add_subdirectory(examples)
endif()

if (BUILD_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL)

add_library(glimmerite_glimmerite INTERFACE)
add_library(glimmerite::glimmerite ALIAS glimmerite_glimmerite)

target_link_libraries(glimmerite_glimmerite
    INTERFACE
    glimmerite::math
    glimmerite::client
)
