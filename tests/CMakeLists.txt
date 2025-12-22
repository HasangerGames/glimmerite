add_executable(GridTest gridTest.cpp)
target_link_libraries(GridTest GMI_math)

add_test(
    NAME GridTest
    COMMAND GridTest
)
