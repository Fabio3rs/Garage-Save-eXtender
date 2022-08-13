cmake_minimum_required(VERSION 3.16)
project("Garage Save eXtender" CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_DEFAULT 17)
set(CXX17 ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_EXTENSIONS ON)

include(CTest)
enable_testing()
add_subdirectory(googletest)
include(GoogleTest)

file(GLOB SOURCES
    "gsx/*.cpp"
    "plugin-sdk/plugin_sa/game_sa/CVector.cpp"
    "plugin-sdk/shared/DynAddress.cpp"
    "plugin-sdk/shared/Events.cpp"
    "plugin-sdk/shared/GameVersion.cpp"
    "plugin-sdk/shared/Patch.cpp"
)

include_directories(SYSTEM cereal/include)
include_directories(plugin-sdk)
include_directories(plugin-sdk/shared)
include_directories(plugin-sdk/shared/game)
include_directories(SYSTEM BEFORE injector/include)

add_library(gsx SHARED ${SOURCES})
target_compile_definitions(gsx PUBLIC -DGTASA=1)

add_subdirectory(gsx/tests)
