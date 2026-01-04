#
# Sprite shader
#
bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/vs_sprite.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      "${CMAKE_BINARY_DIR}/include/generated/shaders"
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/fs_sprite.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      "${CMAKE_BINARY_DIR}/include/generated/shaders"
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
    OUTPUT_DIR      "${CMAKE_BINARY_DIR}/include/generated/shaders"
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS         "${CMAKE_CURRENT_SOURCE_DIR}/shaders/color/fs_color.sc"
    VARYING_DEF     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/color/varying.def.sc"
    INCLUDE_DIRS    ${BGFX_DIR}/src
    OUTPUT_DIR      "${CMAKE_BINARY_DIR}/include/generated/shaders"
    AS_HEADERS
)

add_library(glimmerite_client STATIC
    Application.cpp
    Container.cpp
    gmi.cpp
    Graphics.cpp
    Renderer.cpp
    SoundManager.cpp
    Sprite.cpp
    TextureManager.cpp
    TweenManager.cpp

    stb_image.h

    shaders/shaders.h

    shaders/sprite/varying.def.sc
    shaders/sprite/fs_sprite.sc
    shaders/sprite/vs_sprite.sc

    shaders/color/varying.def.sc
    shaders/color/fs_color.sc
    shaders/color/vs_color.sc
)

set(GMI_CLIENT_INCLUDE_DIR ${GMI_INCLUDE_DIR}/gmi/client)
target_sources(glimmerite_client
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS ${GMI_INCLUDE_DIR}
    FILES
        ${GMI_CLIENT_INCLUDE_DIR}/Affine.h
        ${GMI_CLIENT_INCLUDE_DIR}/Application.h
        ${GMI_CLIENT_INCLUDE_DIR}/Color.h
        ${GMI_CLIENT_INCLUDE_DIR}/Container.h
        ${GMI_CLIENT_INCLUDE_DIR}/Drawable.h
        ${GMI_CLIENT_INCLUDE_DIR}/Graphics.h
        ${GMI_CLIENT_INCLUDE_DIR}/Renderer.h
        ${GMI_CLIENT_INCLUDE_DIR}/SoundManager.h
        ${GMI_CLIENT_INCLUDE_DIR}/Sprite.h
        ${GMI_CLIENT_INCLUDE_DIR}/TextureManager.h
        ${GMI_CLIENT_INCLUDE_DIR}/Transform.h
        ${GMI_CLIENT_INCLUDE_DIR}/TweenManager.h
        ${GMI_CLIENT_INCLUDE_DIR}/Vertex.h
        ${GMI_CLIENT_INCLUDE_DIR}/gmi.h
)

include_directories("${CMAKE_BINARY_DIR}/include/generated/shaders")

target_link_libraries(glimmerite_client PUBLIC
    glimmerite::math
    glimmerite::util
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    bgfx
    bx
    bimg_decode
    glaze::glaze
    earcut_hpp
)

set_target_properties(glimmerite_client PROPERTIES LINKER_LANGUAGE CXX)

if (EMSCRIPTEN)
    target_link_options(glimmerite_client PUBLIC
        "-sASSERTIONS"
        "-sNO_DISABLE_EXCEPTION_CATCHING"
        "-sASYNCIFY"
        "-sEXPORTED_FUNCTIONS=_main,_readImageOnLoad,_readImageOnError"
        "-sEXPORTED_RUNTIME_METHODS=ccall"
        "-sINITIAL_MEMORY=512MB"
        "-sALLOW_MEMORY_GROWTH=1"
    )
endif()

add_library(glimmerite::client ALIAS glimmerite_client)
