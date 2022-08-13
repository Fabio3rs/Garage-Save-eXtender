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
    "gsx/CExtraSaveData.cpp"
    "gsx/CLog.cpp"
    "gsx/crc32.cpp"
    "gsx/CVehicleExtraData.cpp"
    "gsx/Exports.cpp"
    "gsx/grgExtraManager.cpp"
    "gsx/InternalCallbacks.cpp"
)

file(GLOB PLUGIN_SDK_DEPENDENCIES
    "plugin-sdk/plugin_sa/game_sa/CVector.cpp"
    "plugin-sdk/shared/DynAddress.cpp"
    "plugin-sdk/shared/Events.cpp"
    "plugin-sdk/shared/GameVersion.cpp"
    "plugin-sdk/shared/Patch.cpp"
)

file(GLOB ASI_SOURCES
    "gsx/main.cpp"
)

include_directories(SYSTEM cereal/include)
include_directories(SYSTEM plugin-sdk)
include_directories(SYSTEM plugin-sdk/shared)
include_directories(SYSTEM plugin-sdk/shared/game)
include_directories(SYSTEM BEFORE injector/include)

add_library(gsx_internals STATIC ${PLUGIN_SDK_DEPENDENCIES} ${SOURCES})
target_compile_definitions(gsx_internals PUBLIC -DGTASA=1)
target_compile_options(gsx_internals PUBLIC -no-pie -fno-pic)

add_library(gsx SHARED ${ASI_SOURCES} ${SOURCES})
target_compile_definitions(gsx PUBLIC -DGTASA=1)
target_compile_options(gsx PUBLIC -no-pie -fno-pic -static)
target_link_libraries(gsx PUBLIC gsx_internals -static)

set_target_properties(
    gsx 
    PROPERTIES
        OUTPUT_NAME "gsx"
        SUFFIX ".asi"
)

add_subdirectory(gsx/tests)

add_subdirectory(plugins/SaveLicensePlate)
