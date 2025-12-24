add_executable(GridTest gridTest.cpp)
target_link_libraries(GridTest glimmerite::math)

add_test(
    NAME GridTest
    COMMAND GridTest
)
