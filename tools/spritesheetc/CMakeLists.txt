add_executable(spritesheetc spritesheetc.cpp fpng.cpp)

target_compile_options(spritesheetc PRIVATE -msse4.1 -mpclmul)

target_link_libraries(spritesheetc PRIVATE
    resvg
    rectpack2D::rectpack2D
    webp
    glimmerite::util
)
