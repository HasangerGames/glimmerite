add_library(glimmerite_misc STATIC
    BitStream.cpp
)

set(GMI_MISC_INCLUDE_DIR "${GMI_INCLUDE_DIR}/gmi/misc")
target_sources(
    glimmerite_misc
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS ${GMI_INCLUDE_DIR}
    FILES
        ${GMI_MISC_INCLUDE_DIR}/BitStream.h
        ${GMI_MISC_INCLUDE_DIR}/Buffer.h
)

set_target_properties(glimmerite_misc PROPERTIES LINKER_LANGUAGE CXX)

add_library(glimmerite::misc ALIAS glimmerite_misc)
