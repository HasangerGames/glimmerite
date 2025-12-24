add_library(glimmerite_math STATIC
    Shape.cpp
    collision.cpp
)

set(GMI_MATH_INCLUDE_DIR "${GMI_INCLUDE_DIR}/gmi/math")
target_sources(
    glimmerite_math
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS ${GMI_INCLUDE_DIR}
    FILES
        ${GMI_MATH_INCLUDE_DIR}/Easing.h
        ${GMI_MATH_INCLUDE_DIR}/Grid.h
        ${GMI_MATH_INCLUDE_DIR}/Rect.h
        ${GMI_MATH_INCLUDE_DIR}/Shape.h
        ${GMI_MATH_INCLUDE_DIR}/Size.h
        ${GMI_MATH_INCLUDE_DIR}/Vec2.h
        ${GMI_MATH_INCLUDE_DIR}/collision.h
        ${GMI_MATH_INCLUDE_DIR}/math.h
        ${GMI_MATH_INCLUDE_DIR}/random.h
)

set_target_properties(glimmerite_math PROPERTIES LINKER_LANGUAGE CXX)

add_library(glimmerite::math ALIAS glimmerite_math)
