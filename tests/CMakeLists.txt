add_executable(GridTest gridTest.cpp)
target_link_libraries(GridTest glimmerite::math)

add_test(
    NAME GridTest
    COMMAND GridTest
)

add_executable(BitStreamTest bitStreamTest.cpp)
target_link_libraries(BitStreamTest glimmerite::misc)

add_test(
    NAME BitStreamTest
    COMMAND BitStreamTest
)
