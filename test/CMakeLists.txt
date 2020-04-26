
add_library(catch_main STATIC test_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)

set(test_names crossover enclosure driver)

foreach(test_name IN LISTS test_names)

    add_executable(${test_name}_test ${test_name}.cpp)
    add_dependencies(${test_name}_test gspeaker_core catch_main)
    target_link_libraries(${test_name}_test PRIVATE catch_main
                                                    gspeaker_core
                                                    Catch2::Catch2)

    target_include_directories(${test_name}_test PUBLIC ${CMAKE_SOURCE_DIR}/src)

    set_target_properties(${test_name}_test PROPERTIES CXX_STANDARD 14
                                                       CXX_STANDARD_REQUIRED YES
                                                       CXX_EXTENSIONS NO
                                            COMPILE_FLAGS "-Wall")
    target_compile_definitions(${test_name}_test
                               PRIVATE GSPK_TEST_FILE_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(${test_name}_test ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_test)

endforeach()
