cmake_minimum_required(VERSION 3.16)
project(Testes_GSX CXX)

file(
    GLOB MAIN_TESTS_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/maintest.placeholder"
  COMMAND "${CMAKE_SOURCE_DIR}/make_wine_wrapper.sh" "maintest"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_CUSTOM_TARGET(GenerateTestWrapper ALL
   DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/maintest.placeholder"
)

add_executable(maintest ${MAIN_TESTS_SOURCES})
add_dependencies(maintest GenerateTestWrapper)

target_link_libraries(maintest gsx_internals gtest gtest_main)
target_compile_options(maintest PUBLIC -static)
target_link_options(maintest PUBLIC -static)

gtest_discover_tests(maintest DISCOVERY_MODE PRE_TEST)

