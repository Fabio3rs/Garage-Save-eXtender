cmake_minimum_required(VERSION 3.16)
project("SaveLicensePlate" CXX)

file(GLOB INTERFACE_SOURCES
    "${CMAKE_SOURCE_DIR}/API/Cpp/*.cpp"
)

file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(SaveLicensePlate SHARED ${PLUGIN_SDK_DEPENDENCIES} ${INTERFACE_SOURCES} ${SOURCES})
target_include_directories(SaveLicensePlate PUBLIC ${CMAKE_SOURCE_DIR}/API/Cpp)
target_compile_definitions(SaveLicensePlate PUBLIC -DGTASA=1)
target_compile_options(SaveLicensePlate PUBLIC -no-pie -fno-pic -static)
target_link_libraries(SaveLicensePlate PUBLIC -static)

set_target_properties(
    SaveLicensePlate 
    PROPERTIES
        OUTPUT_NAME "SaveLicensePlate"
        SUFFIX ".asi"
)

