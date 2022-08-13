cmake_minimum_required(VERSION 3.16)
project(Testes_GSX CXX)

file(
    GLOB MAIN_TESTS_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(maintest ${MAIN_TESTS_SOURCES})

target_link_libraries(maintest gtest gtest_main)
target_compile_options(maintest PUBLIC -static)
target_link_options(maintest PUBLIC -static)

gtest_discover_tests(maintest)

