add_library(GMI_math STATIC)

set(GMI_MATH_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/gmi/math/")

target_sources(
    GMI_math
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS "${CMAKE_SOURCE_DIR}/include/"
    FILES
        ${GMI_MATH_INCLUDE_DIR}/Easing.h
        ${GMI_MATH_INCLUDE_DIR}/Rect.h
        ${GMI_MATH_INCLUDE_DIR}/Size.h
        ${GMI_MATH_INCLUDE_DIR}/Vec2.h
        ${GMI_MATH_INCLUDE_DIR}/math.h
        ${GMI_MATH_INCLUDE_DIR}/random.h
)
