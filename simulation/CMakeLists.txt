cmake_minimum_required(VERSION 3.16)
project(rain_compute_pc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rain_compute SHARED
    ../main/rain_compute.cpp
)

target_include_directories(rain_compute
    PUBLIC ../main
)

# PC build flag
target_compile_definitions(rain_compute
    PRIVATE PLATFORM_PC
)

# Windows export fix
if (WIN32)
    target_compile_definitions(rain_compute PRIVATE RAIN_COMPUTE_EXPORTS)
endif()
