find_package(doctest REQUIRED)

macro(AddTestFile command_name test_name test_file)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE glimmerite::glimmerite doctest::doctest)

    add_test(
        NAME ${command_name}
        COMMAND ${test_name}
    )
endmacro()

AddTestFile(GridTest grid-test grid.test.cpp)
