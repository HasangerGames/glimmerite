add_library(glimmerite_util STATIC
    # .cpp files go here
)

set(GMI_UTIL_INCLUDE_DIR "${GMI_INCLUDE_DIR}/gmi/util")
target_sources(
        glimmerite_util
        PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS ${GMI_INCLUDE_DIR}
        FILES
        ${GMI_UTIL_INCLUDE_DIR}/util.h
)

set_target_properties(glimmerite_util PROPERTIES LINKER_LANGUAGE CXX)

add_library(glimmerite::util ALIAS glimmerite_util)
